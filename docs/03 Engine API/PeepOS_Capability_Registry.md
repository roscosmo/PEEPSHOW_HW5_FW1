# PeepOS Capability Registry

This document defines Engine-visible PeepOS capability names for game-authoring tools, packages, compatibility checks, and the digital twin.

Capabilities are abstract. They do not name pins, buses, peripherals, DMA channels, STM32 HAL handles, RTOS objects, or hardware registers.

Related:

- [[Game_Authoring_API_Contract]]
- [[Target_Profile_Schema_Contract]]
- [[Digital_Twin_Host_Runtime_Contract]]
- [[Runtime_Host_Contract]]
- [[Runtime_Logic_State_API_Contract]]
- [[Package_Contract]]
- [[Power_and_Sleep_Policy]]
- [[Display_and_Rendering_Contract]]
- [[Audio_Contract]]
- [[Audio_API_Contract]]
- [[Input_Index]]
- [[Sensors_Index]]
- [[Sensor_API_Contract]]
- [[Communication_Index]]
- [[Communication_API_Contract]]
- [[Time_And_Power_Intent_API_Contract]]
- [[Diagnostics_API_Contract]]
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
- use a default/resolved sensor value for optional content behavior

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

## Runtime Logic Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `logic.state_graph` | `CONTRACTED` | bounded state/substate graph execution | no |
| `logic.action_table` | `CONTRACTED` | bounded symbolic action tables | no |
| `logic.guards` | `CONTRACTED` | bounded guard/expression evaluation | no |
| `logic.lifecycle_events` | `CONTRACTED` | package-visible lifecycle event delivery | no |
| `logic.calendar_events` | `CONTRACTED` | local-calendar schedule events through time contract | yes if optional |
| `logic.realtime_frame_tick` | `CONTRACTED` | `RT_SCENE` frame-paced update event with declared budget | yes outside realtime units |
| `logic.deterministic_replay` | `CONTRACTED` | deterministic replay of runtime logic in host/digital twin profiles | yes |

Runtime logic capabilities are defined by [[Runtime_Logic_State_API_Contract]]. They do not imply threads, RTOS timers, hardware callbacks, dynamic code loading, or direct Platform access.

---

## Display Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `display.mono_canvas` | `CONTRACTED` | logical 1-bit drawing surface matching the active target profile | no for HW5 package baseline |
| `display.static_hold` | `CONTRACTED` | last frame remains visible while package is idle or suspended | no |
| `display.static_update` | `CONTRACTED` | low-cadence event-driven display updates in static mode | no |
| `display.realtime_frame` | `CONTRACTED` | frame-paced display requests for active realtime scenes | yes for non-realtime packages |
| `display.autonomous_sequence` | `EXPERIMENTAL` | prevalidated autonomous low-power display sequence, expected to depend on LPBAM evidence | yes |

Display changed-region tracking, transfer selection, DMA, and LPBAM setup are Engine/Platform internals and are not package capabilities.

`display.autonomous_sequence` must remain unavailable for shipping packages until LPBAM display behavior is measured and recorded as supported.

---

## Rendering Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `render.layered_compositor` | `CONTRACTED` | bounded `UI -> GAME -> BG` layer compositing | no |
| `render.masked_1bpp` | `CONTRACTED` | black/white sprite or image assets with opacity mask | no |
| `render.tone5_coverage` | `CONTRACTED` | semantic tone5 assets resolved to 1-bit coverage patterns | yes if optional |
| `render.integer_scale` | `CONTRACTED` | integer-scaled sprite/tone rendering within target profile limits | yes if optional |
| `render.tilemap_viewport` | `CONTRACTED` | bounded tilemap region/viewport rendering | yes if optional |
| `render.precomposed_low_power_sequence` | `CONTRACTED` | package may contain precomposed low-power sequence assets | yes |

`render.precomposed_low_power_sequence` means the package can carry validated sequence data. Autonomous low-power playback still requires `display.autonomous_sequence`.

`tone5` is a semantic coverage model. It is not native display color and must not be described as a color-depth format.

---

## Input Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `input.buttons` | `CONTRACTED` | logical button actions and chords | no for HW5 package baseline |
| `input.encoder` | `CONTRACTED` | logical encoder delta actions | yes if optional |
| `input.joystick_vector` | `CONTRACTED` | normalized joystick vector/action data | yes if optional |
| `input.joystick_direction` | `CONTRACTED` | normalized cardinal/diagonal direction data | yes if optional |
| `input.focus` | `CONTRACTED` | Engine focus scopes and action routing | no |
| `input.chords` | `CONTRACTED` | logical button chord bindings through focus policy | yes if optional |
| `input.hold_repeat` | `CONTRACTED` | logical hold and repeat action delivery where policy allows | yes if optional |
| `input.low_power_wake_intent` | `CONTRACTED` | package may declare logical input wake intent | yes if optional |

`BTN_BOOT` is not a game capability.

Start shipping intent is not a game capability.

Input capabilities are logical. GPIO, EXTI, timer counters, I2C registers, raw joystick magnetic data, debounce state, and wake-pin configuration are not package capabilities.

---

## Audio Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `audio.music` | `CONTRACTED` | symbolic music cue requests | yes if optional |
| `audio.sfx` | `CONTRACTED` | symbolic SFX cue requests | yes if optional |
| `audio.bbb` | `CONTRACTED` | bounded BBB tone, sweep, and pattern requests | yes if optional |
| `audio.volume_intent` | `CONTRACTED` | package may request volume/mute intent through Engine policy | no |
| `audio.timeline` | `CONTRACTED` | symbolic cue timeline events for diagnostics, replay, or package logic where supported | yes if optional |

Audio is a creative package primitive. PeepOS does not require packages to remain semantically complete when muted.

Physical output may be muted, suppressed, degraded, or quarantined by Platform policy. Packages consume symbolic audio APIs through [[Audio_API_Contract]] and must not control audio hardware directly.

---

## Asset Capabilities

| Capability | Status | Meaning |
|---|---|---|
| `asset.sprites` | `CONTRACTED` | sprite/image assets addressed by ID |
| `asset.masked_1bpp_sprites` | `CONTRACTED` | black/white masked sprite assets addressed by ID |
| `asset.tone5_sprites` | `CONTRACTED` | tone5 masked sprite assets addressed by ID |
| `asset.tilemaps` | `CONTRACTED` | bounded tilemap/map assets addressed by ID |
| `asset.tilesets` | `CONTRACTED` | bounded tileset assets addressed by ID |
| `asset.animations` | `CONTRACTED` | bounded animation tables |
| `asset.fonts` | `CONTRACTED` | bounded font assets and text layout metadata |
| `asset.text` | `CONTRACTED` | text/localization tables |
| `asset.data_tables` | `CONTRACTED` | bounded package data tables |
| `asset.low_power_sequences` | `CONTRACTED` | precomposed low-power sequence assets |

Asset capabilities are package-data capabilities. They do not imply filesystem access.

---

## Save Capabilities

| Capability | Status | Meaning |
|---|---|---|
| `save.records` | `CONTRACTED` | schema-versioned save record read/write |
| `save.migration` | `CONTRACTED` | approved save migration path |
| `save.reset` | `CONTRACTED` | explicit package-owned save reset flow |
| `save.package_settings` | `CONTRACTED` | schema-defined package-owned settings |
| `save.write_budget` | `CONTRACTED` | package declares bounded write policy and frequency assumptions |

Saves and package settings are not files. Packages access them only through [[Package_Save_Settings_API_Contract]].

Platform settings, calibration, BLE bonding, install metadata, and fault logs are not package save capabilities.

---

## Time And Power Intent Capabilities

| Capability | Status | Meaning |
|---|---|---|
| `time.delayed_event` | `CONTRACTED` | bounded delayed event requests |
| `time.calendar` | `CONTRACTED` | valid PeepOS local date/time read access for packages |
| `time.calendar_schedule` | `CONTRACTED` | bounded package schedules against local date/time rules |
| `time.frame_delta` | `CONTRACTED` | realtime host frame delta for active realtime units |
| `time.wake_reason` | `CONTRACTED` | normalized package-visible wake reason through lifecycle |
| `time.catch_up_policy` | `CONTRACTED` | bounded missed-event reconciliation policy |
| `time.rtc_wake_intent` | `CONTRACTED` | RTC-backed wake/cadence intent without RTC hardware control |
| `power.idle_intent` | `CONTRACTED` | package can report idle/active state |
| `power.low_power_ready` | `CONTRACTED` | package can declare it is ready for low-power hold/suspend behavior |
| `power.latency_hint` | `CONTRACTED` | package declares acceptable response latency |
| `power.cadence_request` | `CONTRACTED` | package can request bounded static, low-power, or realtime cadence |
| `power.activity_hint` | `CONTRACTED` | package can report meaningful active work without owning sleep policy |
| `power.idle_fallback` | `CONTRACTED` | package can declare fallback routing from high-duty work to low-power behavior |

Packages may read PeepOS calendar time where granted, but may not set, correct, resync, or directly access RTC hardware.

These capabilities express intent only. Platform chooses RTC setup, sleep class, clocks, wake-source arming, and resume policy.

Target profiles expose package-facing wake behavior as wake intents and normalized lifecycle wake reasons. Hardware wake-source arming remains Platform policy.

---

## Sensor Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `sensor.light` | `CONTRACTED` | resolved ambient-light value and band | yes if optional |
| `sensor.light_stream` | `CONTRACTED` | bounded active light sampling context where supported | yes if optional |
| `sensor.imu_steps` | `CONTRACTED` | step session, step count snapshot, or delta | yes if optional |
| `sensor.imu_events` | `CONTRACTED` | motion, tap, shake, tilt, or orientation events where supported | yes if optional |
| `sensor.imu_motion_snapshot` | `CONTRACTED` | normalized low-rate motion/orientation snapshot | yes if optional |
| `sensor.imu_motion_stream` | `CONTRACTED` | bounded higher-rate motion context for realtime gameplay | yes if optional |

Sensor raw values are diagnostics/calibration only. Packages consume resolved PeepOS values, sessions, contexts, and events through [[Sensor_API_Contract]].

For validated HW5 profiles, the normal assumption is that HW5 sensor primitives exist as fixed Platform capabilities. Physical sensor failure is handled through Platform/Engine fault lifecycle and diagnostics, not normal package gameplay logic.

---

## Communication Capabilities

| Capability | Status | Meaning | Fallback Required If Optional |
|---|---|---|---|
| `comm.multiplayer` | `CONTRACTED` | generic multiplayer session and bounded messages | yes if optional |
| `comm.companion` | `CONTRACTED` | generic companion-app session and bounded messages | yes if optional |
| `comm.local_loopback` | `PROFILE_OPTIONAL` | host/digital-twin or diagnostic loopback capability | yes |
| `comm.session_required` | `CONTRACTED` | runtime unit may require an active communication session for admission | no if declared as required |
| `comm.message_schema` | `CONTRACTED` | bounded versioned package communication message schemas | no |

Communication contexts are transport-agnostic. Packages consume abstract sessions, peers, and bounded messages through [[Communication_API_Contract]].

Each communication runtime unit must declare either fallback/route behavior or session-required admission behavior.

For HW5 profiles, communication is not a wake source.

---

## Diagnostics Capabilities

| Capability | Status | Meaning |
|---|---|---|
| `diag.markers` | `CONTRACTED` | package may emit lightweight bounded markers |
| `diag.counters` | `CONTRACTED` | package may emit bounded counters |
| `diag.timing` | `CONTRACTED` | package may emit bounded timing scopes in approved profiles |
| `diag.trace_values` | `CONTRACTED` | package may emit bounded structured values in dev/twin profiles |
| `diag.package_fault` | `CONTRACTED` | package may emit package fault codes routed through Engine lifecycle |
| `diag.replay_markers` | `CONTRACTED` | deterministic replay markers for host/digital-twin tests |
| `diag.shipping_minimal` | `CONTRACTED` | shipping package may retain minimal bounded diagnostic evidence |

Diagnostics are rate-limited and do not own debug transports. Package diagnostics are defined by [[Diagnostics_API_Contract]].

---

## Target Profiles

Target profiles grant a concrete set of capabilities and limits.

The authoritative target profile schema is defined in [[Target_Profile_Schema_Contract]].

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
- runtime logic limits
- event queue and transition stack limits
- time/calendar profile
- display profile
- rendering profile
- cadence limits
- enforced inactivity timeout
- forced-idle route requirement
- static periodic cadence cap
- static input-response latency cap
- low-power periodic cadence cap
- low-power wake/update/return policy
- realtime frame budget
- realtime target frame rate
- wake intents and lifecycle wake reasons
- autonomous display sequence availability
- autonomous sequence frame/cadence caps, if available
- input availability
- sensor primitives and context limits
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
