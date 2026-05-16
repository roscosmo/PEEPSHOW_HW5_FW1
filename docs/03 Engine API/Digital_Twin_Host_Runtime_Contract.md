# Digital Twin Host Runtime Contract

This document defines the PeepShow host-side digital twin runtime.

The digital twin is a contract-accurate desktop implementation of the PeepShow Platform capability surface for game and content development. It is not an STM32 emulator.

Related:

- [[Game_Authoring_API_Contract]]
- [[PeepOS_Capability_Registry]]
- [[Runtime_Host_Contract]]
- [[Runtime_Host_Internal_State_Machines]]
- [[Package_Contract]]
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

## Contract Accuracy Model

The twin must match:

- runtime class semantics
- mount/start/suspend/resume/stop/unmount lifecycle
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

## Sensor Simulation

Sensors are simulated as contract-level data sources.

Allowed twin sensor sources:

- scripted ambient-light tracks
- step-count tracks
- motion/tap/shake/tilt/orientation events
- joystick vector traces
- button and encoder input traces
- unavailable or invalid sensor states for fallback testing

Rules:

- simulated sensor data uses the same normalized Engine/Platform data contracts as hardware
- raw hardware registers are not exposed
- mock sensor use is not hardware bring-up evidence
- sensor timing is deterministic in test/replay mode
- sensor capability availability is declared by the twin profile

---

## Input Simulation

The twin maps host input into PeepOS logical input events.

Allowed host sources:

- keyboard
- gamepad
- mouse where mapped by tools
- scripted input traces
- recorded input replay

Rules:

- mappings produce logical button, encoder, joystick, focus, and action events
- no raw GPIO or EXTI behavior is exposed
- `BTN_BOOT` remains unavailable as normal game input
- Start power/shipping behavior is modeled only through Platform power-intent paths

---

## Audio Simulation

The twin may either play audio or validate audio silently.

Supported behavior:

- music cue requests
- SFX cue requests
- BBB pattern/tone/sweep requests
- volume and mute intent
- unavailable-audio fault injection

Rules:

- cue admission and rejection should match Engine/Platform contracts
- audio hardware details are not modeled
- timing-sensitive audio behavior is contract-level only unless explicitly measured and mapped into the host profile

---

## Communication Simulation

The twin may simulate communication through host loopback or multi-instance sessions.

Supported behavior:

- session advertise/join/leave
- bounded message send/receive
- disconnects
- timeouts
- unavailable communication capability

Rules:

- BLE/NINA commands are not exposed
- bonding and pairing internals are not modeled as hardware behavior
- message schema and size limits must match package contracts

---

## Save And Storage Simulation

The twin uses sandboxed host storage to emulate Engine save/package behavior.

Rules:

- saves use the same schema and versioning rules as packages
- package assets are loaded from compiled package data, not arbitrary runtime paths
- save write failure and interrupted write behavior must be injectable
- host storage paths must not become package runtime APIs
- twin storage evidence is not flash or filesystem bring-up evidence

---

## Time Models

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
- frame checksums or screenshots
- save changes
- emitted diagnostics
- compatibility report
- rejected or clamped request list

---

## Fault Injection

The twin assumes nominal Platform hardware owner health unless fault injection is requested.

Injectable contract-level faults should include:

- sensor unavailable
- stale sensor sample
- save read failure
- save write failure
- audio unavailable
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
- state graph behavior
- input/action routing
- save schema behavior
- deterministic replay behavior
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
9. package handles simulated unavailable optional sensor capability through declared fallback.
10. deterministic replay produces stable frame checksums and final state vector.
11. injected save write failure preserves package logic safety.
12. twin evidence is recorded separately from hardware bring-up evidence.

---

## Rule

The digital twin mirrors proven PeepShow Platform contracts for game and package development.

It must not become a source of hardware truth.
