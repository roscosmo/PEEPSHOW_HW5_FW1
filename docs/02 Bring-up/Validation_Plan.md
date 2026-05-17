# Validation Plan and Acceptance Matrix

This document defines what must be proven before platform freeze and before package-facing expansion.

---

## Validation Stages

1. Hardware bring-up validation
2. Platform service validation
3. Runtime host lifecycle validation
4. Low-power and wake validation
5. Installer/storage ownership validation
6. Long-run stability validation
7. Digital twin parity validation, only after HW5 Platform hardware validation is complete

The digital twin stage is blocked until the Platform hardware backend and relevant owner behavior have measured evidence in [[Brought_Up_Tracker]].

---

## Minimum Test Matrix

Per mode (`SHELL`, `LP_GRAPH`, `LP_MODULE`, `RT_SCENE`, `INSTALLER`):
- mode entry/exit
- owner-thread health
- display/audio/input behavior
- storage permissions enforcement
- suspend/resume behavior
- fault recovery path

---

## Display And Rendering Validation Cases

- logical `168 x 144` canvas maps correctly to native `144 x 168` panel orientation
- masked 1bpp assets render with correct opacity semantics
- tone5 assets render to deterministic 1-bit coverage output
- integer-scaled tone5 assets preserve stable coverage phase
- `UI -> GAME -> BG` layer order matches [[Rendering_API_Contract]]
- display payload DMA reads correctly from the approved SRAM4 display buffer region
- precomposed low-power sequence assets resolve to final 1bpp content before playback
- autonomous display playback remains unavailable unless LPBAM/SRAM4 evidence supports it

---

## Power Validation Cases

- idle current at each runtime class
- wake source correctness for all enabled sources
- STOP2 residency ratio under expected workload
- repeated stop/resume cycle test (long-run)

---

## Runtime Lifecycle Cases

For each host:
1. mount success path
2. suspend during active work
3. resume after sleep wake
4. unmount and switch to another host
5. failure injection during each lifecycle phase

Tracealyzer snapshot evidence should be used where practical to verify owner-thread scheduling and runtime lifecycle ordering. Snapshot trace policy is defined in [[Tracealyzer_Snapshot_Evidence_Contract]].

---

## Runtime Logic And State Validation Cases

Package-facing runtime logic validation must prove the boundary defined by [[Runtime_Logic_State_API_Contract]].

Required cases:

1. valid `LP_GRAPH` state graph validates and runs from its declared entry node.
2. missing entry state fails package validation.
3. transition to undeclared state or undeclared runtime unit fails package validation.
4. unbounded action loop fails validation in every build profile.
5. `LP_GRAPH` high-frequency polling timer fails validation.
6. bounded local-calendar schedule and catch-up policy validate through the time contract.
7. `LP_MODULE` without approved `module_type` fails validation.
8. `RT_SCENE` without frame budget, idle detection, suspend/resume policy, or fallback route fails validation.
9. `RT_SCENE` frame overrun emits package diagnostics where profile allows and follows lifecycle policy.
10. package runtime logic cannot receive hardware owner faults as normal gameplay branches.
11. suspend/resume preserves or reconstructs package state according to declared persistence classes.
12. digital twin replay of a fixed input/time/sensor trace produces identical runtime logic state and diagnostics output.

---

## Input And Focus Validation Cases

Package-facing input validation must prove the boundary defined by [[Input_Focus_API_Contract]].

Required cases:

1. button events map to logical package actions without exposing GPIO, EXTI, or debounce internals.
2. encoder events map to logical delta actions without exposing timer counters or `ENC_EN`.
3. joystick events map to normalized direction/vector actions without exposing TMAG registers or raw magnetic readings.
4. `BTN_BOOT` is unavailable to package input maps.
5. Start shipping and power-intent events suppress package focus and are not delivered as normal package actions.
6. focus scope push/pop/fallback behavior is bounded and deterministic.
7. optional input bindings fall back safely when the selected target profile does not provide the source.
8. low-power wake input intent resumes through the normal lifecycle path before package action delivery.
9. digital twin input replay produces the same package-visible action stream as hardware for the same logical trace.

---

## Sensor API Validation Cases

Package-facing sensor validation must prove the boundary defined by [[Sensor_API_Contract]].

Required cases:

1. ambient-light package primitive resolves to level and band without exposing ADC, GPIO, or `PHOT_EN`.
2. step session baseline reset changes package delta only and does not reset the hardware step counter.
3. IMU event package primitive receives normalized motion/tap/shake/tilt/orientation events without exposing registers.
4. high-rate motion stream in `LP_GRAPH` fails tool validation.
5. bounded motion stream in `RT_SCENE` validates only when the target profile grants `sensor.imu_motion_stream`.
6. per-step MCU wake behavior fails validation for normal packages.
7. optional sensor feature fails validation if content fallback behavior is missing.
8. required sensor primitive fault at runtime is logged as Platform degraded capability and handled through Engine lifecycle policy.
9. digital twin sensor replay produces deterministic package-visible values/events.
10. digital twin sensor fault injection produces diagnostics and lifecycle evidence, not HW5 bring-up evidence.

---

## Audio API Validation Cases

Package-facing audio validation must prove the boundary defined by [[Audio_API_Contract]].

Required cases:

1. symbolic music cue resolves to a valid package audio asset without exposing SAI, DMA, mixer, or amp control.
2. SFX priority/preemption is deterministic within the target profile voice limit.
3. BBB pattern validates frequency, duration, step count, repeat count, curve, and envelope bounds.
4. package volume/mute intent is overridden by global mute without corrupting package state.
5. muted or suppressed output does not fail package validation and does not require semantic fallback.
6. audio-centric package behavior validates when assets, contexts, and runtime behavior are bounded.
7. package audio path performs no FileX/FAT, host-path, or editor-source streaming during active runtime.
8. package logic cannot subscribe to DMA callbacks, SAI completion, LPTIM interrupts, or buffer refill timing.
9. digital twin audio timeline replay is deterministic with and without host audio output.
10. digital twin audio fault injection produces diagnostics and lifecycle evidence, not HW5 bring-up evidence.

---

## Communication API Validation Cases

Package-facing communication validation must prove the boundary defined by [[Communication_API_Contract]].

Required cases:

1. communication profile validates session contexts, role intent, message schema, payload bounds, and rate limits.
2. package cannot reference BLE, NINA, UART, GAP, GATT, module commands, pins, flow control, or bonding storage.
3. communication wake intent fails HW5 target-profile validation.
4. `LP_GRAPH` package that depends on receiving communication messages fails HW5 validation.
5. session-required runtime unit remains in declared admission route when no session exists.
6. optional communication runtime unit follows declared fallback/route behavior.
7. peer disconnect and session timeout are delivered as package-visible session events.
8. BLE owner fault is logged as Platform/Engine diagnostic and not exposed as UART/NINA error to package gameplay code.
9. digital twin multi-instance communication replay is deterministic for a fixed trace.
10. digital twin delayed/drop/disconnect/fault injection validates package session behavior without acting as HW5 BLE bring-up evidence.

---

## Time And Power Intent Validation Cases

Package-facing time and power-intent validation must prove the boundary defined by [[Time_And_Power_Intent_API_Contract]].

Required cases:

1. package reads valid PeepOS local calendar time without RTC register or hardware timer access.
2. package cannot set, correct, resync, or directly program RTC/calendar time.
3. first-setup or recovery flow establishes valid system time before launching calendar-dependent packages.
4. delayed and local-calendar schedules produce bounded package events.
5. long sleep resumes package with elapsed suspended/calendar time and bounded missed-event summary.
6. unbounded catch-up policy fails validation.
7. low-power polling cadence above target profile limits fails validation.
8. `RT_SCENE` without idle fallback fails validation.
9. user inactivity timeout forces declared low-power route despite active cadence request.
10. HW5 communication wake intent fails validation.
11. digital twin deterministic replay produces the same time, schedule, wake, and lifecycle event sequence for a fixed trace.
12. digital twin accelerated sleep simulation is not used as HW5 current, wake-latency, RTC, or physical sleep evidence.

---

## Diagnostics API Validation Cases

Package-facing diagnostics validation must prove the boundary defined by [[Diagnostics_API_Contract]].

Required cases:

1. package marker emits a bounded timeline record in dev/twin profile.
2. package counter emits fixed-schema numeric value within rate limits.
3. package timing scope reports bounded package/runtime timing without hardware callbacks.
4. shipping profile preserves minimal package fault code and runtime-unit evidence.
5. verbose trace values are rejected or stripped in shipping profile unless release policy allows them.
6. package diagnostics cannot reference SWD, SWO, UART, USB, BLE, storage regions, hardware registers, RTOS objects, raw pointers, memory dumps, or filesystem paths.
7. package fault routes through Engine lifecycle policy.
8. Platform fault remains Platform diagnostic and is not exposed as normal package diagnostic API.
9. digital twin replay produces deterministic diagnostics for a fixed trace.
10. exported diagnostic summary copies bounded package records without exposing protected storage directly.

---

## Installer Cases

- local to installer handoff
- USB export with no competing storage users
- MSC personality exposes no CDC developer control in v1
- CDC developer personality exposes no MSC staging/export volume in v1
- CDC package upload routes through firmware-owned staging and package validation
- CDC live-safe tuning rejects non-live-safe knobs and routes accepted edits through owner requests
- package stage/validate/commit
- safe rollback on failure
- return to shell and remount local storage

---

## Development Tooling Validation Cases

Development tooling validation must prove the boundary defined by [[USB_Development_Mode_Contract]], [[Live_Tuning_And_Knobs_Contract]], and [[Telemetry_And_Debug_Dashboard_Contract]].

Required cases:

1. live-tuning registry is generated from the same source as the compile-time knobs schema.
2. `runtime_live_safe` knob can be listed, described, set, and applied through the owning subsystem.
3. out-of-range live value is clamped or rejected according to metadata.
4. `compile_time`, `boot_applied`, and `protected_policy` knobs are not exposed as normal live controls.
5. raw memory address write commands are unavailable.
6. owner apply failure preserves the previous valid value.
7. session overlay export records firmware commit, knobs hash, board revision, changed values, and tool version.
8. validation evidence records active tuning overlay and USB personality when live tuning is used.
9. digital twin replay records and reapplies the active tuning overlay for deterministic runs.
10. dashboard decoder accepts a capture manifest and telemetry stream with matching schema versions.
11. dashboard clearly identifies source type: hardware live, hardware exported, Tracealyzer-linked, or digital twin.
12. state vector, owner FSM, sleep/wake, display, storage/USB, runtime, package diagnostics, and live-tuning events appear in the expected panels.
13. dashboard controls use documented CDC/live-tuning/package-upload protocols and cannot bypass owner routing or package validation.
14. telemetry evidence records firmware commit, board revision, knobs hash, schema versions, source profile, and artifact path.
15. telemetry payloads do not expose protected storage, raw memory, HAL handles, RTOS objects, or arbitrary filesystem paths.

Tracealyzer snapshot validation must prove the boundary defined by [[Tracealyzer_Snapshot_Evidence_Contract]].

Required cases:

1. trace build exposes owner thread names.
2. each owner thread blocks when idle and wakes on its expected event path.
3. snapshot captures display/storage/input/sensor/audio/runtime scenarios inside the trace window.
4. sleep quiesce snapshot shows owner acknowledgements before sleep entry.
5. wake snapshot shows wake reason classification before owner resume validation completes.
6. trace-enabled evidence records trace configuration, buffer size, firmware commit, knobs hash, and artifact path.

---

## Digital Twin Parity Cases

Digital twin validation begins only after the corresponding Platform behavior is proven on HW5 hardware.

Required cases:

1. host twin exposes the same contract-visible state vector as the hardware backend.
2. same package runs through the same Engine lifecycle on host and hardware backends.
3. host twin simulates `SHELL`, `LP_GRAPH`, `LP_MODULE`, `RT_SCENE`, and `INSTALLER` contract behavior.
4. host twin enforces measured inactivity timeout and cadence clamp policy.
5. host twin simulates hold/static/realtime display behavior from measured Platform profile.
6. LPBAM/autonomous display capability appears only when measured HW5 evidence supports it.
7. host twin provides deterministic input, sensor, communication, and fault-injection traces.
8. host twin deterministic replay produces stable state-vector and output artifacts.
9. host twin evidence is recorded separately from hardware bring-up evidence.

---

## Exit Criteria For Platform Freeze

Platform freeze can be approved only when:
1. all mandatory tests pass
2. no unresolved unknown wake causes remain
3. no cross-thread ownership violations remain
4. docs and implementation are in sync

---

## Evidence Requirements

Every test run record must include:
- board revision and firmware commit
- test case ID and expected behavior
- pass/fail result
- artifact links (logs, captures, traces)
- issue IDs for failures

Evidence classes:

- HW5 hardware evidence: measured on physical HW5 hardware; may satisfy bring-up and hardware known-good requirements.
- Digital twin evidence: produced by the host runtime after hardware validation; may satisfy package, Engine, lifecycle, replay, and contract-parity requirements.

Digital twin evidence must not be used to mark physical wake, current, peripheral, storage-media, display-electrical, sensor-electrical, or BLE-module behavior known-good.
