# Pending Measured Constants Register

This document tracks constants, limits, thresholds, timings, budgets, and capability gates that must be measured on HW5 before they become authoritative Platform, Engine, target-profile, or tooling values.

A value listed here is intentionally not final.

It becomes known-good only when measured evidence is linked from [[Brought_Up_Tracker]] and the owning contract/profile is updated.

Related:

- [[Brought_Up_Tracker]]
- [[Evidence_Artifact_Convention]]
- [[Validation_Plan]]
- [[HW5_Hardware_Documentation_Readiness]]
- [[Target_Profile_Schema_Contract]]
- [[Knobs_and_Tuning_Contract]]
- [[Memory_and_Budgeting_Contract]]
- [[Power_and_Sleep_Policy]]
- [[Display_and_Rendering_Contract]]

---

## Rules

- Do not hardcode pending values into shipping policy.
- Provisional values may be used for tool design only.
- Every finalized value must link to evidence.
- Every finalized value must identify the contract, Platform knob, target profile, or tool schema it updates.
- If a measured value changes a package-facing limit, update the relevant target profile.
- If a measured value affects PeepOS implementation policy, update the relevant Platform contract or Platform knob.
- If a measured value affects package authoring, update package/tool validation rules.

---

## Status Values

| Status | Meaning |
|---|---|
| `Pending` | not measured yet |
| `Provisional` | placeholder used for design/tooling only |
| `Measured` | measured but not yet promoted into authority docs |
| `Adopted` | promoted into the owning contract/profile/knob/tool rule |
| `Rejected` | measurement shows this value or capability is not viable |
| `Superseded` | replaced by a newer measurement or policy |

---

## Promotion Rule

A pending measured constant becomes authoritative only when:

1. evidence exists in [[Brought_Up_Tracker]]
2. evidence artifacts follow [[Evidence_Artifact_Convention]]
3. the owning contract is updated
4. the relevant target profile or Platform knob is updated where applicable
5. package/tool validation impact is reviewed
6. the register row is marked `Adopted`, `Rejected`, or `Superseded`

---

## Register

| ID | Value | Domain | Feeds | Evidence Source | Status | Notes |
|---|---|---|---|---|---|---|
| PMC-POWER-001 | sleep current per runtime class | Power | [[Power_and_Sleep_Policy]], [[Target_Profile_Schema_Contract]] | [[Sleep_Wake_Integration_Bring-up_Runbook]], power measurement artifacts | Pending | needed before low-power profile freeze |
| PMC-POWER-002 | wake latency per wake source | Power/Input/Sensor | [[Power_and_Sleep_Policy]], [[Input_Focus_API_Contract]], [[Target_Profile_Schema_Contract]] | [[Sleep_Wake_Integration_Bring-up_Runbook]] | Pending | includes buttons, encoder, RTC, sensor IRQs where supported |
| PMC-POWER-003 | resume latency per runtime class | Power/Runtime | [[Runtime_Host_Contract]], [[Digital_Twin_Host_Runtime_Contract]] | sleep/wake telemetry and trace artifacts | Pending | needed for twin parity and lifecycle budgets |
| PMC-POWER-004 | owner quiesce timeout values | Power/RTOS | [[Subsystem_State_Machines]], [[RTOS_Ownership_and_Queue_Topology]] | Tracealyzer/SWO evidence | Pending | do not finalize until owner threads exist |
| PMC-POWER-005 | enforced inactivity timeout | Power/Engine | [[Time_And_Power_Intent_API_Contract]], [[Target_Profile_Schema_Contract]] | integration testing and UX review | Provisional | expected logical range around 10-15 s, final value evidence/policy gated |
| PMC-POWER-006 | static cadence and input-response caps | Power/Display | [[Power_and_Sleep_Policy]], [[Target_Profile_Schema_Contract]] | current/display tests | Pending | separate periodic cadence from input-response latency |
| PMC-POWER-007 | baseline low-power periodic update cap | Power/Display | [[Power_and_Sleep_Policy]], [[Display_and_Rendering_Contract]] | current/display sleep tests | Pending | wake/update/return cost must be measured |
| PMC-POWER-008 | autonomous low-power display sequence cadence | Power/Display | [[Target_Profile_Schema_Contract]], [[Digital_Twin_Host_Runtime_Contract]] | LPBAM display experiment | Pending | only relevant if LPBAM path works |
| PMC-POWER-009 | 303040 LiPo battery profile and ADP5360 charge/fuel settings | Power/PMIC | [[PMIC_and_Power_Contract]], [[ADP5360_Power_Bring-up_Runbook]] | PPK2 traces, cell review, ADP5360 register readback, charging tests | Pending | seller-stated 450 mAh is unverified; previous LIR2540 assumptions must not carry over |
| PMC-POWER-010 | measured effective battery capacity / runtime basis | Power/Product | [[PMIC_and_Power_Contract]], target profile UX estimates | controlled discharge/runtime tests | Pending | use for battery indicator/runtime claims; not required before initial PMIC probe |
| PMC-POWER-011 | operation energy profiles for Platform actions | Power/Tooling | [[Power_Measurement_and_Trace_Correlation_Runbook]], [[Power_and_Sleep_Policy]], [[Target_Profile_Schema_Contract]] | PPK2 traces correlated with Tracealyzer/SWO/telemetry | Pending | cost table for wake/resume, display flush, sensor burst, BLE activity, audio output, save write, USB enumeration, MSC activity |
| PMC-DISPLAY-001 | LS013B7DH05 pixel polarity | Display | [[Display_and_Rendering_Contract]], renderer implementation | [[LS013B7DH05_Display_Bring-up_Runbook]] | Pending | confirm native bit meaning with pattern test |
| PMC-DISPLAY-002 | row order and line address format | Display | [[Display_and_Rendering_Contract]] | display pattern tests | Pending | validates logical/native coordinate mapping |
| PMC-DISPLAY-003 | byte order and row payload format | Display | [[Display_and_Rendering_Contract]] | display pattern tests | Pending | needed before partial update policy |
| PMC-DISPLAY-004 | dirty granularity and full-frame fallback threshold | Display/Rendering | [[Rendering_API_Contract]], [[Display_and_Rendering_Contract]] | display timing tests | Pending | package tools never control dirty regions |
| PMC-DISPLAY-005 | SRAM4 DMA reachability for display payloads | Display/Memory | [[Memory_and_Budgeting_Contract]], [[HW5_DMA_Map]] | DMA/display flush evidence | Pending | required before DMA/LPBAM display grant |
| PMC-DISPLAY-006 | LPBAM autonomous display sequence support | Display/Power | [[Target_Profile_Schema_Contract]], [[Digital_Twin_Host_Runtime_Contract]] | display + sleep runbooks | Pending | gates `display.autonomous_sequence` |
| PMC-DISPLAY-007 | ULP sequence frame count, byte budget, and cadence | Display/Memory | [[Display_and_Rendering_Contract]], [[Memory_and_Budgeting_Contract]] | LPBAM experiment artifacts | Pending | only adopt if autonomous sequence support is proven |
| PMC-MEM-001 | SRAM4 split | Memory/Display | [[Memory_and_Budgeting_Contract]], linker script | map file and SRAM4 retention/DMA evidence | Pending | framebuffer, TX payload, ULP frames, retained resume blob, margin |
| PMC-MEM-002 | owner thread stack sizes | Memory/RTOS | [[Memory_and_Budgeting_Contract]], Platform knobs | map file and stack watermark evidence | Pending | per owner thread |
| PMC-MEM-003 | owner queue depths | Memory/RTOS | [[RTOS_Ownership_and_Queue_Topology]], Platform knobs | queue stress tests | Pending | per owner queue |
| PMC-MEM-004 | Tracealyzer snapshot buffer size | Debug/Memory | [[Tracealyzer_Snapshot_Evidence_Contract]], [[Memory_and_Budgeting_Contract]] | trace capture tests | Pending | snapshot only unless streaming becomes necessary |
| PMC-MEM-005 | dashboard telemetry ring/event budget | Debug/Telemetry | [[Telemetry_And_Debug_Dashboard_Contract]] | telemetry capture tests | Pending | profile/build gated |
| PMC-MEM-006 | package runtime RAM limit | Engine/Memory | [[Target_Profile_Schema_Contract]], [[Package_Contract]] | map/budget report | Pending | package-facing abstract limit only |
| PMC-MEM-007 | package asset/blob size limits | Assets/Storage | [[Package_Blob_Format_Contract]], [[Target_Profile_Schema_Contract]] | storage/package validation | Pending | depends on flash layout and installer policy |
| PMC-INPUT-001 | button debounce timing | Input | [[Button_Input_Contract]], Platform knobs | [[Button_Input_Bring-up_Runbook]] | Pending | include press/release behavior |
| PMC-INPUT-002 | long press and repeat timing | Input/Shell | [[Input_Focus_API_Contract]], [[Shell_Settings_Calibration_Contract]] | button runbook | Pending | Start/shipping behavior remains Platform-owned |
| PMC-INPUT-003 | rotary encoder sign convention and detent ratio | Input | [[Rotary_Encoder_Input_Contract]] | [[Rotary_Encoder_Bring-up_Runbook]] | Pending | needed for logical delta mapping |
| PMC-INPUT-004 | rotary encoder settle/filter timing | Input/Power | [[Rotary_Encoder_Input_Contract]], Platform knobs | encoder runbook | Pending | includes wake-armed current impact |
| PMC-INPUT-005 | joystick center/deadzone/hysteresis | Input/Sensor | [[Joystick_Hall_Input_Contract]], [[Input_Focus_API_Contract]] | [[TMAG3001_Joystick_Bring-up_Runbook]] | Pending | package sees normalized vector/direction only |
| PMC-INPUT-006 | joystick wake threshold and polarity | Input/Power | [[HW5_Wake_Sources]], [[Power_and_Sleep_Policy]] | joystick + sleep runbooks | Pending | if supported by measured hardware |
| PMC-SENSOR-001 | TEMT6000 dark/room/bright ADC bands | Sensor | [[Light_Sensor_Contract]], [[Sensor_API_Contract]] | [[TEMT6000_Light_Sensor_Bring-up_Runbook]] | Partial | preliminary HW5 evidence: covered `0x3F9F..0x3FAB`, ambient `0x38AF..0x3995`, direct light down to `0x0011`; inverse polarity confirmed; normalized thresholds still pending |
| PMC-SENSOR-002 | light sensor settle/filter timing | Sensor/Power | [[Light_Sensor_Contract]], Platform knobs | light sensor runbook | Pending | required before stream limits; current `fw0` probe uses provisional `5 ms` settle and `250 ms` sample cadence only for bring-up |
| PMC-SENSOR-003 | LIS2DUX12 lowest-power step counting mode | Sensor/Power | [[IMU_Contract]], [[Target_Profile_Schema_Contract]] | [[LIS2DUX12_IMU_Bring-up_Runbook]] | Pending | ST baseline first, optimize later |
| PMC-SENSOR-004 | IMU event thresholds | Sensor/Input | [[Sensor_API_Contract]] | IMU runbook | Pending | tap/shake/tilt/orientation behavior |
| PMC-SENSOR-005 | IMU motion stream context limits | Sensor/Runtime | [[Sensor_API_Contract]], [[Target_Profile_Schema_Contract]] | IMU + power tests | Pending | sample rate, event rate, duration, wake behavior, and runtime-class validity |
| PMC-AUDIO-001 | MAX98357A enable settle timing | Audio/Power | [[Audio_Contract]], Platform knobs | [[Audio_Output_Bring-up_Runbook]] | Pending | needed for pop-free output policy |
| PMC-AUDIO-002 | BBB safe frequency/duration bounds | Audio | [[Audio_API_Contract]], [[Audio_Contract]] | audio runbook | Pending | package BBB requests remain symbolic/bounded |
| PMC-AUDIO-003 | mixer voice budget | Audio/Engine | [[Audio_API_Contract]], [[Target_Profile_Schema_Contract]] | audio runtime tests | Pending | music plus SFX budget |
| PMC-AUDIO-004 | audio buffer sizes and sample rate confirmation | Audio/Memory | [[Memory_and_Budgeting_Contract]], [[Audio_Contract]] | audio DMA/playback evidence | Pending | profile/build gated |
| PMC-STORAGE-001 | AT25SL128A JEDEC/device ID and status meanings | Storage | [[Storage_and_Installer_Contract]] | [[AT25SL128A_External_Flash_Bring-up_Runbook]] | Pending | driver baseline |
| PMC-STORAGE-002 | erase/program/readback timing | Storage | [[Storage_and_Installer_Contract]], package installer policy | flash runbook | Pending | package install UX and timeout policy |
| PMC-STORAGE-003 | deep power-down and wake timing | Storage/Power | [[Storage_and_Installer_Contract]], [[Power_and_Sleep_Policy]] | flash + sleep tests | Pending | affects suspend/resume and installer behavior |
| PMC-STORAGE-004 | protected fault-log ring offset and size | Storage/Diagnostics | [[Memory_and_Budgeting_Contract]], [[Debug_and_Observability]] | flash layout pass | Pending | protected, not host-exposed |
| PMC-STORAGE-005 | save write budget | Save/Storage | [[Package_Save_Settings_API_Contract]], [[Target_Profile_Schema_Contract]] | storage wear/policy review | Pending | package-facing limit |
| PMC-USB-001 | MSC mount/reclaim timing | USB/Storage | [[Storage_and_Installer_Contract]], [[USB_MSC_Bring-up_and_Recovery_Runbook]] | USB MSC runbook | Pending | host ownership handoff |
| PMC-USB-002 | CDC packet size/rate limits | USB/Dev Tools | [[USB_Development_Mode_Contract]], [[Dev_Orchestration_CLI_Contract]] | CDC dev-mode tests | Pending | developer personality only |
| PMC-COMM-001 | NINA reset/boot timing | Communication | [[BLE_Communication_Contract]] | [[NINA_B112_BLE_Bring-up_Runbook]] | Pending | keep aux pins safe until comm owner controls them |
| PMC-COMM-002 | BLE UART ring buffer sizing | Communication/Memory | [[BLE_Communication_Contract]], [[Memory_and_Budgeting_Contract]] | BLE runbook | Pending | interrupt-driven v1 baseline |
| PMC-COMM-003 | BLE payload and message-rate limits | Communication/Engine | [[Communication_API_Contract]], [[Target_Profile_Schema_Contract]] | BLE/session tests | Pending | package-facing abstract limits |
| PMC-COMM-004 | BLE session timeout/reconnect behavior | Communication | [[Communication_API_Contract]] | BLE runbook | Pending | package sees session events, not NINA faults |
| PMC-COMM-005 | BLE current impact | Communication/Power | [[Power_and_Sleep_Policy]], [[Communication_API_Contract]] | power measurement artifacts | Pending | HW5 communication wake remains blocked unless future evidence changes policy |
| PMC-COMM-006 | interactive session peer-wait grace and refresh policy | Communication/Power/Engine | [[Communication_API_Contract]], [[Time_And_Power_Intent_API_Contract]], [[Target_Profile_Schema_Contract]] | BLE session UX and power tests | Pending | bounded wait for remote turns; must not become keepalive-driven stay-awake policy |
| PMC-TOOL-001 | SWO event rate limit | Debug/Tooling | [[Debug_and_Observability]], [[Telemetry_And_Debug_Dashboard_Contract]] | debug workflow tests | Pending | avoid destabilizing timing |
| PMC-TOOL-002 | dashboard telemetry schema/event rate | Tooling | [[Telemetry_And_Debug_Dashboard_Contract]] | dashboard capture validation | Pending | build/profile gated |
| PMC-TOOL-003 | evidence artifact folder convention adoption | Bring-up | [[Evidence_Artifact_Convention]], [[Brought_Up_Tracker]] | first evidence entry | Pending | validate convention with first real artifact |
| PMC-TOOL-004 | power trace correlation sync strategy | Debug/Power | [[Power_Measurement_and_Trace_Correlation_Runbook]], [[Tracealyzer_Snapshot_Evidence_Contract]] | board pin review and first correlated power capture | Pending | physical GPIO sync if a safe pin exists; otherwise timed/cue fallback with stated precision |

---

## Domain Notes

### Power And Sleep

Power constants should remain profile-gated until measured on HW5.

Do not promote STOP, wake, or current assumptions from the digital twin.

Power estimates should be derived from measured operation energy profiles, battery profile evidence, and target-profile runtime behavior. PPK2 traces and sync markers are Platform evidence only; package tools may consume resulting profile limits or estimates, not raw measurement internals.

### Display And Rendering

Display constants must distinguish:

- panel-native behavior
- renderer logical behavior
- DMA/SRAM4 requirements
- LPBAM/autonomous display support

Package tools may consume resulting target profile limits but must not control dirty rows, transfer mode, SRAM4 placement, or LPBAM descriptors.

### Memory And Budgets

Budget constants require map-file evidence or runtime watermark/stress evidence.

Development/instrumented build budgets must remain separate from release/shipping budgets.

### Input And Sensors

Measured physical behavior should update Platform normalization and calibration policy.

Packages consume logical input and normalized sensor events only.

Sensor values that affect packages should be promoted as target-profile sensor contexts, not flat sensor-wide rate limits.

### Audio

Audio constants should distinguish creative package limits from hardware safety/settle behavior.

Sound may be muted by user/system policy; audio-centric packages are still valid.

### Storage, USB, And Communication

Package-facing limits must remain abstract.

Do not expose flash offsets, FAT paths, BLE/NINA details, or USB internals to package tools.

---

## Review Cadence

Review this register:

- before each bring-up phase starts
- after each evidence entry is added
- before creating or updating a target profile
- before Platform freeze
- before digital twin implementation begins

---

## Rule

Pending measured constants are tracked here so uncertainty stays explicit.

Do not pretend a value is known until HW5 evidence proves it.
