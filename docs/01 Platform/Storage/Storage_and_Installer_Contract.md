# Storage and Installer Contract

This document defines HW5 storage ownership, flash behavior, region model, USB staging/export mode, and installer isolation.

For execution-level USB bring-up and regression recovery steps, use [[USB_MSC_Bring-up_and_Recovery_Runbook]].

## Hardware and Middleware

| Item | HW5 Selection |
|---|---|
| External flash | `AT25SL128A` serial NOR flash |
| Bus | `OCTOSPI1` quad mode |
| DMA RX | `GPDMA1_CH4`, `GPDMA1_REQUEST_OCTOSPI1` |
| DMA TX | `GPDMA1_CH5`, `GPDMA1_REQUEST_OCTOSPI1` |
| Filesystem middleware | FileX |
| Flash translation | LevelX custom NOR interface |
| USB export | USBX MSC |

External flash is the only persistent storage besides internal MCU flash.

## Ownership

- `thStorage` is the sole owner of external flash, OCTOSPI1, storage DMA, FileX, LevelX, USB MSC media, mount state, and host-export state.
- Other threads use `qStorageReq` only.
- No direct filesystem or flash operations are allowed outside `thStorage`.
- Engine and Reference Game code consume package/save APIs only.

## Region Model

External `AT25SL128A` regions must be explicitly defined before firmware implementation.

Required regions:

| Region | Purpose | Host Exposed | Owner |
|---|---|---|---|
| settings/config | Platform settings | No | `thStorage` |
| communication bonding | BLE pairing/bonding data | No | `thStorage` through `thComm` request |
| calibration | joystick/input/display/sensor calibration | No | `thStorage` |
| save data | Reference Game and package saves | No | `thStorage` through save API |
| installed package/blob | installed game/content raw blob storage | No | `thStorage` / Engine package API |
| installed index/metadata | active package index, versions, integrity data | No | `thStorage` / package manager |
| persistent fault log ring | boot/fault records, reset evidence, crash summaries | No | `thStorage` through fault supervisor request |
| USB staging/export | package staging, host transfer, debug export surface | Yes | host while exported, `thStorage` otherwise |
| logs/screenshots/debug export | copied/exported diagnostic artifacts | Indirect only | `thStorage` |

Rules:

- settings, calibration, communication bonding records, saves, installed blobs, installed indexes, and persistent fault logs are never directly host-writable.
- USB MSC exposes only staging/export storage.
- persistent fault logs remain in a protected ring region; firmware may copy/export diagnostic summaries into the staging/export volume.
- host access must never expose internal storage regions directly.

## Persistent Fault Log Region

Reserve a small protected ring region near the end of external flash for persistent fault evidence.

Rules:

- region is not host-exposed and is not part of USB staging/export
- region is separate from settings, calibration, bonding, saves, installed packages, indexes, and staging
- writes are append/ring style with magic, version, sequence number, timestamp or boot counter, CRC, and fault class
- a failed write must preserve the previous valid record
- early boot faults before storage is mounted use debugger/SWO/serial where available, then later faults use the protected ring
- export to host is by firmware-copy into staging/export only, never direct mount exposure

Exact offset and size are assigned during the flash-layout pass.

## Data Source Rules

- Runtime/package assets are read from installed raw blob storage or bounded cached RAM.
- FAT/FileX is staging/debug export only.
- No FileX/FAT reads are allowed during active gameplay/audio runtime loops.
- No mount/unmount churn is allowed inside active runtime loops.
- Music streaming sources must be raw installed blob storage into bounded buffers, not FAT.

## External Flash Device FSM

| State | Meaning |
|---|---|
| `FLASH_OFF` | flash path inactive or not yet touched |
| `FLASH_RESET` | bus/device reset sequence in progress |
| `FLASH_PROBE` | JEDEC/device identity and liveness probe |
| `FLASH_CONFIG` | quad/OCTOSPI configuration and protection checks |
| `FLASH_READY` | flash ready for read/program/erase requests |
| `FLASH_BUSY_READ` | read transfer active |
| `FLASH_BUSY_PROGRAM` | program transfer active |
| `FLASH_BUSY_ERASE` | erase operation active |
| `FLASH_DEEP_POWER_DOWN` | flash placed in low-power deep power-down |
| `FLASH_RECOVERING` | bounded recovery after bus/device error |
| `FLASH_ERROR` | flash unavailable or recovery exhausted |

Flash rules:

- deep power-down whenever idle and Platform policy allows
- no sleep entry during active read/program/erase
- every operation has an explicit timeout
- wake/resume must revalidate device liveness before use
- recovery retries are bounded

## Storage Ownership FSM

| State | Meaning |
|---|---|
| `STORAGE_OFFLINE` | storage unavailable, not initialized, or failed |
| `STORAGE_INIT` | storage owner initializing objects and flash path |
| `STORAGE_FLASH_READY` | flash device ready, filesystem not yet mounted |
| `STORAGE_LOCAL_MOUNT` | local FileX/LevelX mount in progress |
| `STORAGE_LOCAL_READY` | firmware owns storage and internal regions are available |
| `STORAGE_QUIESCE_LOCAL` | local users are being drained before export/install/sleep |
| `STORAGE_PREPARE_USB` | staging/export volume is prepared for host ownership |
| `STORAGE_USB_STAGING_EXPORTED` | host owns USB staging/export volume |
| `STORAGE_USB_STAGING_DIRTY` | host wrote or changed staging/export volume |
| `STORAGE_USB_RELEASE` | host export is ending and firmware is reclaiming ownership |
| `STORAGE_INSTALLING` | package install/commit operation active |
| `STORAGE_RECOVERING` | bounded recovery path active |
| `STORAGE_SAFE_MODE` | normal shell blocked because storage/settings/calibration unavailable |
| `STORAGE_ERROR` | unrecovered storage fault |

Storage rules:

- normal shell requires storage because settings and joystick calibration are required for usable operation
- storage failure routes to safe mode, not normal shell
- all local clients must be blocked before USB export
- firmware must reclaim and rescan staging/export after USB release
- install commit must preserve last known valid package/index state

## USB Staging / Export FSM

| State | Meaning |
|---|---|
| `USB_STAGE_OFF` | USB staging/export inactive |
| `USB_STAGE_PREPARE` | staging volume prepared and local users blocked |
| `USB_STAGE_EXPORTED` | USB MSC visible to host |
| `USB_STAGE_HOST_ACTIVE` | host IO observed |
| `USB_STAGE_HOST_DIRTY` | host changed staging/export volume |
| `USB_STAGE_RELEASE_REQUESTED` | user/system requested exit from flashing/export mode |
| `USB_STAGE_RESCAN` | firmware reclaimed volume and is checking contents |
| `USB_STAGE_READY_FOR_INSTALL` | staged package/debug data ready for processing |
| `USB_STAGE_ERROR` | USB staging/export fault |

USB export rules:

- host owns the staging/export FAT volume while MSC is active
- MCU FileX/FAT remains unmounted while host owns the staging/export volume
- `B` button is the minimal local exit/cancel input during flashing/export mode
- display may show a static "flashing" or installer screen and then remain mostly inactive
- runtime rendering/audio/gameplay are disabled or policy-limited during export

## Installer Mode Behavior

Installer/export mode is mostly unusable by design.

Allowed behavior:

- static Sharp Memory LCD status screen
- minimal input monitoring, especially `B` to exit/cancel when safe
- USB MSC host transfer
- storage owner staging/rescan/install flow
- diagnostics explicitly allowed by policy

Disallowed behavior:

- active gameplay
- normal runtime audio
- non-installer storage clients
- host and MCU writing the same FAT/staging region simultaneously

## Save and Settings Rules

- settings writes must be power-fail safe
- BLE pairing/bonding records must be power-fail safe and preserve the last valid record on failed update
- persistent fault-log writes must preserve the previous valid record on failed update
- joystick calibration must be available before normal shell/game input is considered usable
- save schema versions must support migration paths
- write frequency assumptions and wear strategy must be documented
- save/settings regions are not host-writable

## Failure Policy

Storage failure is platform-critical.

If settings/calibration/storage cannot be validated:

- normal shell must not start
- route to `STORAGE_SAFE_MODE`
- expose diagnostics or USB recovery if safe
- never allow gameplay/runtime launch

If external flash is unavailable:

- package/runtime assets are unavailable
- saves/settings may be unavailable
- install/update is unavailable
- safe mode is required

## Validation Cases

1. flash probe/config/read/program/erase succeeds with bounded timing
2. flash deep power-down and wake/revalidate path works
3. local mount reaches `STORAGE_LOCAL_READY`
4. storage failure routes to `STORAGE_SAFE_MODE`
5. USB MSC exports only staging/export volume
6. settings/saves/installed blobs are never host-writable
7. host write/read/delete smoke succeeds on staging/export volume
8. firmware reclaim/rescan detects changed staging contents
9. package install preserves last known valid installed index on interruption
10. runtime asset reads use raw installed blob storage, not FAT/FileX
11. installer/export mode keeps display static and only minimal input active
12. logs/screenshots/debug exports are copied into staging/export without exposing internal regions directly
13. persistent fault-log ring preserves previous valid records and is not host-exposed

Related:

- [[Storage_Index]]
- [[Package_Manager_State_Machine]]
- [[USB_MSC_Bring-up_and_Recovery_Runbook]]
- [[HW5_DMA_Map]]
- [[HW5_Pin_Ownership_Matrix]]
- [[Power_and_Sleep_Policy]]
