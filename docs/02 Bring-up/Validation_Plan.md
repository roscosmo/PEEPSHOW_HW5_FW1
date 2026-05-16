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
