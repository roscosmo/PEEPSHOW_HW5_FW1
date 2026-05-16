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

## Installer Cases

- local to installer handoff
- USB export with no competing storage users
- package stage/validate/commit
- safe rollback on failure
- return to shell and remount local storage

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
