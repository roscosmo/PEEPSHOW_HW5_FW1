# PeepOS Capability Registry

This document defines Engine-visible PeepOS capability names for game-authoring tools, packages, compatibility checks, and the digital twin.

Capabilities are abstract. They do not name pins, buses, peripherals, DMA channels, STM32 HAL handles, RTOS objects, or hardware registers.

Related:

- [[Game_Authoring_API_Contract]]
- [[Digital_Twin_Host_Runtime_Contract]]
- [[Runtime_Host_Contract]]
- [[Package_Contract]]
- [[Power_and_Sleep_Policy]]
- [[Display_and_Rendering_Contract]]
- [[Audio_Contract]]
- [[Input_Index]]
- [[Sensors_Index]]
- [[Communication_Index]]
- [[Brought_Up_Tracker]]

---

## Capability Status

Every capability has a status.

| Status | Meaning |
|---|---|
| `CONTRACTED` | Engine-facing shape is documented, but hardware behavior may still require validation before shipping use. |
| `HW_VALIDATED` | corresponding HW5 behavior has measured evidence in [[Brought_Up_Tracker]]. |
| `PROFILE_OPTIONAL` | package may target it only when the selected target profile grants it or a fallback is declared. |
| `EXPERIMENTAL` | may be used in bring-up, dev, or preview profiles only; shipping packages must not require it. |
| `BLOCKED` | not available to packages until the contract or hardware evidence changes. |

Pre-HW5-arrival documents may define `CONTRACTED` capabilities so tools can be designed, but physical support is not known-good until upgraded to `HW_VALIDATED`.

---

## Fallback Rules

Required capabilities must be available in the selected target profile.

Optional capabilities require fallback behavior.

Fallback behavior may include:

- disable the feature
- substitute static content
- use a lower cadence
- use a simpler input path
- use silent audio behavior
- run local-only instead of multiplayer
- use a default sensor value or last valid snapshot

Fallbacks must be validated before package compilation/export.

---

## Runtime Capabilities

Runtime class is primarily declared in the manifest, but tools may still use these names in compatibility reports.

| Capability | Status | Meaning |
|---|---|---|
| `runtime.lp_graph` | `CONTRACTED` | low-power event/state graph execution |
| `runtime.lp_module` | `CONTRACTED` | Engine-hosted bounded low-power module execution |
| `runtime.rt_scene` | `CONTRACTED` | frame-paced realtime scene execution |

`SHELL` and `INSTALLER` are Platform-owned runtime classes, not normal package target capabilities.

---

## Display Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `display.mono_canvas` | `CONTRACTED` | logical 1-bit drawing surface matching the active target profile | no for HW5 package baseline |
| `display.static_hold` | `CONTRACTED` | last frame remains visible while package is idle or suspended | no |
| `display.static_update` | `CONTRACTED` | low-cadence event-driven display updates in static mode | no |
| `display.realtime_frame` | `CONTRACTED` | frame-paced display requests for active realtime scenes | yes for non-realtime packages |
| `display.dirty_region_hint` | `CONTRACTED` | package may provide invalidation hints; Platform chooses transfer method | no |
| `display.autonomous_sequence` | `EXPERIMENTAL` | prevalidated autonomous low-power display sequence, expected to depend on LPBAM evidence | yes |

`display.autonomous_sequence` must remain unavailable for shipping packages until LPBAM display behavior is measured and recorded as supported.

---

## Input Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `input.buttons` | `CONTRACTED` | logical button actions and chords | no for HW5 package baseline |
| `input.encoder` | `CONTRACTED` | logical encoder delta actions | yes if optional |
| `input.joystick_vector` | `CONTRACTED` | normalized joystick vector/action data | yes if optional |
| `input.joystick_direction` | `CONTRACTED` | normalized cardinal/diagonal direction data | yes if optional |
| `input.focus` | `CONTRACTED` | Engine focus scopes and action routing | no |

`BTN_BOOT` is not a game capability.

Start shipping intent is not a game capability.

---

## Audio Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `audio.music` | `CONTRACTED` | symbolic music cue requests | yes if optional |
| `audio.sfx` | `CONTRACTED` | symbolic SFX cue requests | yes if optional |
| `audio.bbb` | `CONTRACTED` | bounded BBB tone, sweep, and pattern requests | yes if optional |
| `audio.volume_intent` | `CONTRACTED` | package may request volume/mute intent through Engine policy | no |

Audio requests may be rejected or degraded when Platform policy requires silence, sleep, or audio quarantine.

---

## Asset Capabilities

| Capability | Status | Meaning |
|---|---|---|
| `asset.sprites` | `CONTRACTED` | sprite/image assets addressed by ID |
| `asset.tilemaps` | `CONTRACTED` | bounded tilemap/map assets addressed by ID |
| `asset.animations` | `CONTRACTED` | bounded animation tables |
| `asset.text` | `CONTRACTED` | text/localization tables |
| `asset.data_tables` | `CONTRACTED` | bounded package data tables |

Asset capabilities are package-data capabilities. They do not imply filesystem access.

---

## Save Capabilities

| Capability | Status | Meaning |
|---|---|---|
| `save.records` | `CONTRACTED` | schema-versioned save record read/write |
| `save.migration` | `CONTRACTED` | approved save migration path |
| `save.reset` | `CONTRACTED` | explicit package-owned save reset flow |

Saves are not files. Packages access saves only through Engine save APIs.

---

## Time And Power Intent Capabilities

| Capability | Status | Meaning |
|---|---|---|
| `time.delayed_event` | `CONTRACTED` | bounded delayed event requests |
| `time.rtc_wake_intent` | `CONTRACTED` | RTC-backed wake/cadence intent |
| `power.idle_intent` | `CONTRACTED` | package can report idle/active state |
| `power.low_power_ready` | `CONTRACTED` | package can declare it is ready for low-power hold/suspend behavior |
| `power.latency_hint` | `CONTRACTED` | package declares acceptable response latency |

These capabilities express intent only. Platform chooses sleep class, clocks, wake-source arming, and resume policy.

---

## Sensor Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `sensor.light` | `CONTRACTED` | normalized ambient-light snapshot and band | yes if optional |
| `sensor.imu_steps` | `CONTRACTED` | step count snapshot or delta | yes if optional |
| `sensor.imu_events` | `CONTRACTED` | motion, tap, shake, tilt, or orientation events where supported | yes if optional |

Sensor raw values are diagnostics/calibration only. Packages consume normalized snapshots/events.

---

## Communication Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `comm.multiplayer` | `CONTRACTED` | generic multiplayer session and bounded messages | yes unless package is communication-required |
| `comm.companion` | `CONTRACTED` | generic companion-app session and bounded messages | yes unless package is communication-required |
| `comm.local_loopback` | `PROFILE_OPTIONAL` | host/digital-twin or diagnostic loopback capability | yes |

Communication packages must declare offline or unavailable behavior unless the package is explicitly communication-required.

---

## Diagnostics Capabilities

| Capability | Status | Meaning |
|---|---|---|
| `diag.package_events` | `CONTRACTED` | package event markers and package fault codes |
| `diag.replay_markers` | `CONTRACTED` | deterministic replay markers for host/digital-twin tests |

Diagnostics are rate-limited and do not own debug transports.

---

## Target Profiles

Target profiles grant a concrete set of capabilities and limits.

Required initial profiles:

| Profile | Purpose |
|---|---|
| `HW5_PENDING_VALIDATION` | design-time profile before measured hardware evidence; not shipping-authoritative |
| `HW5_VALIDATED_BASELINE` | measured HW5 Platform behavior without assuming LPBAM autonomous display support |
| `HW5_VALIDATED_LPBAM` | measured HW5 Platform behavior with LPBAM autonomous display support |
| `HOST_AUTHORING_PREVIEW` | editor/simulator preview with mocks and placeholders |
| `HOST_DIGITAL_TWIN_HW5` | host twin profile derived from measured HW5 behavior after validation |

Profiles must record:

- capability grant list
- capability status list
- runtime classes
- display profile
- cadence limits
- idle timeout
- input availability
- sensor availability
- audio limits
- communication limits
- save/storage limits
- Platform contract revision
- evidence reference when hardware-derived

---

## Capability Change Control

Adding or changing a capability requires:

1. update this registry
2. update [[Game_Authoring_API_Contract]] if authoring behavior changes
3. update package schema or compatibility reports if serialized data changes
4. update digital twin profile rules if host behavior changes
5. update Platform contract or hardware validation docs if hardware behavior is affected

Reference Game needs may request new capabilities, but accepted capabilities must be reusable beyond the Reference Game.
