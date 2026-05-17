# Digital Twin Host Runtime Contract

This document defines the PeepShow host-side digital twin runtime.

The digital twin is a contract-accurate desktop implementation of the PeepShow Platform capability surface for game and content development. It is not an STM32 emulator.

Related:

- [[Game_Authoring_API_Contract]]
- [[PeepOS_Capability_Registry]]
- [[Target_Profile_Schema_Contract]]
- [[Runtime_Host_Contract]]
- [[Runtime_Host_Internal_State_Machines]]
- [[Runtime_Logic_State_API_Contract]]
- [[Package_Contract]]
- [[Telemetry_And_Debug_Dashboard_Contract]]
- [[Asset_Pipeline_and_Package_Tooling_Contract]]
- [[Validation_Plan]]
- [[Brought_Up_Tracker]]
- [[Platform_Freeze_Charter]]
- [[Power_and_Sleep_Policy]]
- [[Display_and_Rendering_Contract]]
- [[HW5_Hardware_Documentation_Readiness]]

---

## Sequencing Rule

The digital twin is implemented after complete PeepShow hardware and Platform validation.

It is not a bring-up substitute.

It must not define hardware behavior.

It mirrors measured and documented PeepShow Platform behavior after the hardware backend is proven.

Required order:

1. hardware contracts and runbooks
2. HW5 board bring-up with measured evidence
3. Platform owner threads and hardware abstraction validated
4. power, display, input, sensor, storage, audio, communication, installer, and wake behavior proven
5. PeepOS Platform contract frozen enough to target
6. digital twin implemented from the proven Platform contract
7. game/tool development accelerates against the twin

If the digital twin disagrees with measured HW5 behavior, the twin is wrong unless the Platform contract is explicitly changed with evidence.

---

## Purpose

The digital twin exists to run and test:

- the same game code
- the same Engine runtime hosts
- the same package assets
- the same state graph data
- the same runtime logic/action table data
- the same map data
- the same input event model
- the same runtime lifecycle
- the same cadence and wake-intent contracts
- the same logical display resolution and pixel format

Only the Platform backend changes.

```text
Game package / game code / state data
        |
Engine runtime host
        |
PeepShow Platform capability contract
        |
+----------------------+----------------------+
| STM32 HW5 backend    | Host digital twin    |
+----------------------+----------------------+
```

---

## Non-Goals

The digital twin must not emulate:

- STM32 registers
- ThreadX scheduler internals
- DMA transfer timing
- SPI, SAI, I2C, OCTOSPI, UART, or USB waveforms
- PLL switching behavior
- STOP2 electrical behavior
- exact interrupt latency
- measured current draw
- physical sensor noise unless explicitly modeled as a test input

The twin simulates Platform contracts, not electrical hardware.

---

## Contract State Mirror

The digital twin mirrors the contract-visible Platform and Engine state hierarchy.

It should always be able to report the same public state/substate/sub-substate categories that the hardware backend reports.

Conceptual state vector:

```text
peepos_state_vector:
  power_state
  sleep_class
  runtime_class
  runtime_lifecycle_state
  host_internal_state
  display_contract_state
  audio_contract_state
  input_contract_state
  sensor_contract_state
  storage_contract_state
  communication_contract_state
  wake_reason
  active_capabilities
  rejected_capabilities
  granted_cadence
  clamped_requests
```

The STM32 backend reports this from real owner state machines.

The host digital twin produces the same state vector from the host backend.

For normal game testing, the twin assumes Platform hardware owner FSMs reach their nominal OK states unless a test explicitly injects a Platform fault.

Examples:

- display available
- storage available
- input available
- audio available when declared and enabled
- sensor capability available when declared and provided by the twin profile
- communication available when the twin profile enables it

Fault and unavailable-capability behavior is tested through explicit fault injection or profile configuration.

---

## Parity Schema

The digital twin must exchange structured parity data with tools, replay systems, and dashboard captures.

Required parity objects:

```text
twin_profile
state_vector
event_record
replay_manifest
replay_result
parity_report
```

Rules:

- parity data uses Engine/Platform contract names only.
- parity data must not expose host filesystem paths to package runtime.
- parity data must not contain HAL, register, pin, DMA, RTOS, or linker names.
- hardware-derived fields must identify the source target profile and evidence reference.

---

## Twin Profile Schema

The twin profile imports a target profile and declares host-only behavior.

```text
twin_profile:
  twin_profile_id
  twin_profile_version
  source_target_profile_id
  source_target_profile_version
  source_evidence_refs[]
  platform_contract_revision
  engine_contract_revision
  host_backend_version
  time_models[]
  input_adapters[]
  sensor_trace_adapters[]
  audio_output_mode
  communication_backend
  save_backend
  fault_injection_supported[]
  unsupported_features[]
```

Rules:

- `HOST_DIGITAL_TWIN_HW5` must reference a measured HW5 target profile.
- `HOST_AUTHORING_PREVIEW` may use provisional limits, but must label them as preview.
- a twin profile cannot grant a package capability blocked by its source target profile.
- host-only adapters are not package capabilities.

---

## State Vector Schema

State vectors are the main parity comparison surface.

```text
state_vector:
  sequence
  timestamp_model
  target_profile_id
  runtime_class
  runtime_unit_id
  runtime_lifecycle_state
  host_internal_state
  shell_state
  power_state
  sleep_class
  display_contract_state
  audio_contract_state
  input_contract_state
  sensor_contract_state
  storage_contract_state
  communication_contract_state
  active_capabilities[]
  rejected_capabilities[]
  requested_cadence
  granted_cadence
  clamped_requests[]
  wake_reason
  package_diagnostics[]
  platform_diagnostics[]
```

Rules:

- state vector fields must match the contract-visible hardware backend categories.
- state vectors may include compact checksums for large state, assets, display frames, or saves.
- package-visible state and Platform diagnostic state must remain separated.
- normal game logic must not branch on Platform hardware fault internals.

---

## Event Record Schema

Replay and dashboard parity use ordered event records.

```text
event_record:
  sequence
  time
  source
  category
  name
  payload_schema
  payload
  target_profile_id
  runtime_unit_id
  deterministic
```

Allowed source categories:

- `platform`
- `engine`
- `package`
- `input`
- `sensor`
- `time`
- `communication`
- `storage`
- `diagnostic`
- `fault_injection`

Rules:

- events must be schema-versioned.
- event order must be deterministic in replay mode.
- event payloads must be bounded.
- events must not expose raw host paths, raw memory, or hardware internals.

---

## Replay Manifest Schema

Replay manifests define deterministic twin runs.

```text
replay_manifest:
  replay_id
  package_id
  package_hash
  target_profile_id
  twin_profile_id
  content_parameter_hash
  content_parameter_overrides_ref
  initial_save_state_ref
  time_model
  input_trace_ref
  sensor_trace_ref
  communication_trace_ref
  fault_injection_ref
  expected_outputs[]
```

Replay result:

```text
replay_result:
  replay_id
  result
  final_state_vector_hash
  frame_checksum_sequence
  save_delta_hash
  diagnostics_hash
  compatibility_report_ref
  telemetry_capture_ref
  notes
```

Rules:

- replay manifests must record content parameter overrides.
- replay manifests must record active target/twin profile versions.
- replay results are package/Engine evidence, not HW5 hardware evidence.
- failing replay output should preserve enough artifacts for deterministic reproduction.

---

## Parity Report Schema

Parity reports compare expected and actual contract behavior.

```text
parity_report:
  report_id
  profile_pair
  package_hash
  run_context
  compared_fields[]
  matched_fields[]
  mismatched_fields[]
  ignored_fields[]
  hardware_evidence_refs[]
  twin_artifact_refs[]
  conclusion
```

Rules:

- ignored fields must explain why they are not contract-visible or not comparable.
- hardware-only measurements such as current draw, bus timing, and exact interrupt latency are never twin parity requirements.
- mismatch against measured HW5 contract behavior means the twin is wrong unless the Platform contract changes with evidence.

---

## Contract Accuracy Model

The twin must match:

- runtime class semantics
- mount/start/suspend/resume/stop/unmount lifecycle
- runtime logic state, event, guard, action, and transition semantics
- package validation and capability admission behavior
- logical display model
- input event model
- sensor snapshot and event contracts
- save API schema behavior
- communication message contracts
- cadence clamping and wake-intent behavior
- static hold, low-power, realtime, and installer behavior visible to Engine/Game code

The twin must not claim to prove:

- physical wake latency
- STOP current
- EXTI polarity
- display EXTCOMIN correctness
- flash erase/program timing
- DMA reliability
- PMIC behavior
- BLE module behavior
- real sensor register behavior

---

## Runtime Mode Simulation

The twin simulates mode behavior visible to packages and Engine hosts.

Runtime logic execution follows [[Runtime_Logic_State_API_Contract]].

| Mode / Class | Twin Behavior |
|---|---|
| `SHELL` | PeepOS shell/package launch and return behavior where needed for testing |
| `LP_GRAPH` | event, RTC, wake, and state-graph driven execution with long idle gaps |
| `LP_MODULE` | bounded low-power module execution with host-defined update limits |
| `RT_SCENE` | fixed-step or wall-clock frame-paced scene execution with budget reporting |
| `INSTALLER` | package staging, validation, install, rollback, and no-normal-gameplay behavior |

The twin must validate cross-mode transitions:

- active to idle
- static to hold
- hold to wake
- sleep-resident to resume
- realtime to suspend
- installer entry/exit
- package fault to safe return path

---

## Display Power State Simulation

The twin models display behavior through Platform-visible display capability profiles.

It does not emulate SPI or panel electrical behavior.

Required logical states:

| State | Meaning |
|---|---|
| `DISPLAY_HOLD` | last frame remains visible; no game logic runs |
| `DISPLAY_ULP_ANIM` | optional autonomous low-power frame sequence if measured LPBAM support exists |
| `DISPLAY_STATIC` | low-cadence event/state-machine display behavior |
| `DISPLAY_REALTIME` | frame-paced active drawing behavior |

LPBAM-dependent behavior must be driven by measured hardware evidence.

If LPBAM display animation is proven:

- the twin may expose `display.autonomous_sequence`
- autonomous sequences use prevalidated frames only
- no arbitrary game logic runs during autonomous display sequence
- frame count and cadence are capped by measured Platform policy

If LPBAM display animation is not proven or unavailable:

- `display.autonomous_sequence` is unavailable
- low-power display updates require modeled wake/update/return behavior
- low-power update cadence is capped by measured Platform policy

The twin must support both compatibility profiles only after hardware evidence defines the actual profile or allowed fallback policy.

---

## Power And Idle Policy Simulation

The twin enforces PeepOS power policy as a contract, not as a measurement.

Required behavior:

- inactivity timeout advances the simulated Platform toward low-power policy
- packages cannot keep realtime behavior active without declared activity
- background display cadence is clamped by runtime class and Platform profile
- input-triggered updates can be modeled as immediate where Platform policy allows
- STOP-resident logical time may advance without running package code
- wake reasons are classified and delivered through the normal lifecycle/event path

The exact timeout, low-power frame cap, static frame cap, and realtime frame budget are Platform policy values and must come from measured/frozen contracts.

The twin should report:

- requested cadence
- granted cadence
- clamped requests
- idle timer state
- simulated sleep/hold residency
- wake reason
- capability rejections

---

## Rendering Simulation

The twin must use the same package-facing rendering semantics as [[Rendering_API_Contract]].

Required behavior:

- logical `168 x 144` canvas semantics match the active target profile
- `UI -> GAME -> BG` layer order is preserved
- `masked_1bpp` and `tone5_masked` assets render through the same semantic rules as device runtime
- integer scaling uses deterministic coverage patterns
- `ULP_ANIM` uses precomposed low-power sequence assets only
- frame checksums and screenshots derive from the logical rendered result

The twin must not treat host renderer behavior as hardware evidence for Sharp LCD row order, SPI payload correctness, SRAM4 retention, LPDMA reachability, LPBAM behavior, or EXTCOMIN correctness.

---

## Sensor Simulation

Sensors are simulated as contract-level data sources through [[Sensor_API_Contract]].

Allowed twin sensor sources:

- scripted ambient-light tracks
- step-count tracks
- motion/tap/shake/tilt/orientation events
- deterministic motion stream traces
- joystick vector traces
- button and encoder input traces
- explicit sensor fault-injection traces for lifecycle and diagnostics testing

Rules:

- simulated sensor data uses the same normalized Engine/Platform data contracts as hardware
- raw hardware registers are not exposed
- mock sensor use is not hardware bring-up evidence
- sensor timing is deterministic in test/replay mode
- sensor capability availability is declared by the twin profile
- normal `HOST_DIGITAL_TWIN_HW5` profiles assume nominal sensor owner health unless fault injection is enabled
- injected sensor faults test Platform/Engine lifecycle and diagnostics, not ordinary gameplay error branches

---

## Input Simulation

The twin maps host input into PeepOS logical input events through [[Input_Focus_API_Contract]].

Allowed host sources:

- keyboard
- gamepad
- mouse where mapped by tools
- scripted input traces
- recorded input replay

Rules:

- mappings produce logical button, encoder, joystick, focus, and action events
- focus scopes, bindings, fallbacks, and runtime-unit input admission use the same rules as device runtime
- no raw GPIO or EXTI behavior is exposed
- host keyboard, gamepad, and mouse inputs are editor/twin adapters only, not package input sources
- `BTN_BOOT` remains unavailable as normal game input
- Start power/shipping behavior is modeled only through Platform power-intent paths

---

## Audio Simulation

The twin may either play audio or validate audio silently through [[Audio_API_Contract]].

Supported behavior:

- music cue requests
- SFX cue requests
- BBB pattern/tone/sweep requests
- volume and mute intent
- symbolic audio timeline recording
- muted/suppressed output simulation
- audio fault injection

Rules:

- cue admission, priority, preemption, loops, fades, markers, and suppression should match Engine/Platform contracts
- PeepOS does not require packages to remain semantically complete when muted
- host audio output is optional for deterministic tests
- audio hardware details are not modeled
- timing-sensitive audio behavior is contract-level only unless explicitly measured and mapped into the host profile
- twin audio evidence is not HW5 audio bring-up evidence

---

## Communication Simulation

The twin may simulate communication through host loopback or multi-instance sessions using [[Communication_API_Contract]].

Supported behavior:

- session advertise/join/leave
- peer join/leave
- bounded message send/receive
- delayed, dropped, or reordered messages where schema policy allows
- disconnects
- timeouts
- communication fault injection
- session-required admission behavior

Rules:

- BLE/NINA commands are not exposed
- bonding and pairing internals are not modeled as hardware behavior
- message schema and size limits must match package contracts
- HW5 twin profiles must preserve the no-BLE-wake rule unless a future target profile grants communication wake
- twin communication evidence is not HW5 BLE bring-up evidence

---

## Save And Storage Simulation

The twin uses sandboxed host storage to emulate Engine save/package behavior.

Rules:

- saves and package-owned settings use [[Package_Save_Settings_API_Contract]]
- saves use the same schema and versioning rules as packages
- package assets are loaded through [[Package_Asset_Loading_API_Contract]] from compiled `PeepPkg` package data, not arbitrary runtime paths
- save write failure and interrupted write behavior must be injectable
- package setting read/write, migration, reset, and write-budget behavior must be injectable
- host storage paths must not become package runtime APIs
- twin storage evidence is not flash or filesystem bring-up evidence

---

## Time Models

The twin time model follows [[Time_And_Power_Intent_API_Contract]].

The twin must support at least two time models.

| Time Model | Purpose |
|---|---|
| deterministic fixed-step | reproducible tests, replay, golden output |
| interactive wall-clock | human preview and development |

Optional time models:

- accelerated low-power idle simulation
- single-step event evaluation
- recorded timeline replay

All time models must preserve the runtime class and lifecycle contracts.

Calendar time must be controllable in deterministic tests. The twin must replay package-visible local time, elapsed suspend time, schedule delivery, wake reasons, cadence clamps, and inactivity timeout behavior from the selected target profile.

The host time model must not be used as HW5 RTC hardware, wake-latency, current, or physical sleep evidence.

---

## Deterministic Replay

The twin should support deterministic replay for package and runtime validation.

Replay inputs may include:

- package ID and package checksum
- twin profile ID
- initial save state
- input trace
- sensor trace
- communication trace
- time model
- fault injection script

Replay outputs may include:

- final state vector
- final package runtime logic state
- dashboard-facing telemetry events through [[Telemetry_And_Debug_Dashboard_Contract]]
- frame checksums or screenshots
- save changes
- emitted diagnostics through [[Diagnostics_API_Contract]]
- compatibility report
- rejected or clamped request list

Digital twin diagnostics may be more verbose than shipping hardware diagnostics, but must remain deterministic for a fixed trace. Twin diagnostic output is not hardware bring-up evidence.

---

## Fault Injection

The twin assumes nominal Platform hardware owner health unless fault injection is requested.

Injectable contract-level faults should include:

- sensor fault / degraded sensor primitive
- stale sensor sample
- save read failure
- save write failure
- audio fault / suppression
- communication disconnect
- communication timeout
- display present rejected
- cadence request clamped
- capability unavailable
- suspend/resume during active package work

Fault injection validates package fallback behavior. It does not emulate hardware fault physics.

---

## Evidence Classes

Digital twin evidence can prove:

- package validation behavior
- runtime lifecycle behavior
- state graph, action table, and runtime logic behavior
- input/action routing
- save schema behavior
- deterministic replay behavior
- dashboard telemetry determinism for a fixed replay input
- cadence and power-intent compliance against the Platform contract
- cross-mode logic behavior
- fallback handling for unavailable capabilities

Digital twin evidence cannot prove:

- HW5 electrical behavior
- physical wake behavior
- current draw
- peripheral timing
- device register configuration
- bus recovery on real hardware
- real storage media behavior
- real BLE module behavior

Hardware behavior is known-good only when measured on HW5 and recorded in [[Brought_Up_Tracker]].

---

## Required Host Profiles

The first digital twin profile should be derived from measured HW5 Platform behavior after validation.

Target profile fields are defined in [[Target_Profile_Schema_Contract]]. The twin profile adds host adapter and replay behavior on top of that source target profile.

Profile fields should include:

- display capability profile
- LPBAM/autonomous sequence availability
- static display cadence cap
- low-power update cadence cap
- realtime frame budget
- inactivity timeout
- available input capabilities
- available sensor capabilities
- audio capability limits
- communication capability limits
- save/storage limits
- runtime class support

Profiles must be versioned and tied to the Platform contract revision they mirror.

Rules:

- twin profiles must not redefine target profile limits.
- host-only adapters must be clearly labeled.
- source target profile hash/version must be recorded in replay manifests.

---

## Validation Cases

1. digital twin implementation is blocked until required HW5 Platform validation evidence exists.
2. same package runs through the same Engine host lifecycle on hardware backend and host twin backend.
3. twin state vector matches contract-visible Platform/Engine states for mode transitions.
4. `LP_GRAPH` package enters hold/low-power behavior after inactivity timeout.
5. input wake resumes package through the same wake/lifecycle path as the Platform contract.
6. static display update requests are clamped by the measured profile.
7. realtime scene reports frame budget violations.
8. LPBAM/autonomous display capability is available only when measured evidence supports it.
9. sensor fault injection triggers Platform/Engine diagnostics and lifecycle policy outside normal gameplay logic.
10. deterministic replay produces stable frame checksums and final state vector.
11. injected save write failure preserves package logic safety.
12. twin evidence is recorded separately from hardware bring-up evidence.

---

## Rule

The digital twin mirrors proven PeepShow Platform contracts for game and package development.

It must not become a source of hardware truth.
