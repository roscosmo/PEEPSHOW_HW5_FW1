# Authority and Cross-Cutting Invariants

This document is the single source of truth for rules that apply across PeepShow HW5 subsystems.

If another document conflicts with this one, resolve the conflict immediately.

---

## Scope

Defines:

- Platform, Engine, and Reference Game authority
- ownership and concurrency invariants
- timing and cadence invariants
- storage and installer invariants
- determinism and debug invariants
- documentation priority

Does not define:

- board electrical details, see [[HW5_Hardware_Revision_Contract]]
- per-subsystem FSM details, see [[Subsystem_State_Machines]]
- Reference Game mechanics, see [[Reference_Game_Index]]

---

## Layer Authority

Platform owns:

- hardware behavior
- RTOS ownership
- clocks and sleep policy
- peripheral access
- storage and USB arbitration
- fault handling
- bring-up evidence

Engine owns:

- reusable runtime abstractions
- package/content contracts
- scene/input/rendering abstractions above Platform APIs
- game-development SDK rules

Reference Game owns:

- content
- mechanics
- creatures
- encounters
- lore
- balancing
- game-specific state machines

The Reference Game may request capabilities. It may not redefine Platform behavior.

---

## Canonical Runtime Classes

Use these exact runtime class tokens until explicitly replaced by a newer Engine contract:

- `SHELL`
- `LP_GRAPH`
- `LP_TEMPLATE`
- `RT_SCENE`
- `INSTALLER`

---

## Ownership Model

- Every peripheral and shared subsystem has exactly one Platform owner.
- Other layers must send typed requests only.
- No cross-thread direct HAL/LL register access.
- ISR code must signal and return immediately.

Request payload rules:

- fixed-size POD structs
- no transient pointer ownership
- no function pointers
- bounded work only

---

## Timing Model

- Low-power cadence must be RTC/event driven.
- Real-time cadence must be frame-scheduled and deterministic.
- No mode transition is complete until HAL and RTOS timebases are valid.

Time-domain labels are mandatory for timing knobs:

- `threadx`
- `hal_ms`
- `knob_rtos_tick_hz`

---

## Power and Clock Invariants

- Platform power owner is sole owner of sleep class and clock transitions.
- Quiesce-before-sleep and resume-before-validate are mandatory.
- No clock changes during active DMA or active bus transaction.
- No STOP entry while critical owners are unquiesced.
- Engine and Reference Game express power intent only.

---

## Storage and Installer Invariants

- Storage owner is sole owner of flash and filesystem operations.
- Engine and Reference Game must not use FAT for active runtime execution.
- `INSTALLER` is single-writer mode for host-visible transport.
- Non-installer subsystems are isolated while installer path is active.

---

## Determinism Invariants

- No unbounded loops in runtime-critical paths.
- No hidden retries or random backoff logic.
- No runtime dynamic allocation unless explicitly documented and approved.
- No filesystem streaming in active runtime loops.

---

## Debug Invariants

- HardFault capture is mandatory.
- Breakpoint use must be strategic and bounded.
- Structured trace events are preferred over heavy halt-based debugging.
- STOP behavior must be validated with evidence, not inference.

---

## Knobs Invariants

- All tunable firmware constants flow through the knobs pipeline.
- Generated outputs are never edited manually.
- Knob changes require regeneration and rebuild.
- Knobs must not silently alter architecture boundaries.

---

## Game Documentation Invariant

Game documentation may advance during hardware bring-up.

Game implementation may not drive Platform architecture.

Game notes may define intent and desired capabilities. They may not define:

- pin choices
- peripheral ownership
- DMA policy
- clock policy
- sleep policy
- storage ownership
- hardware fault handling

See [[Game_Documentation_Boundary]].

---

## Document Priority

When implementing:

1. this document
2. relevant Hardware contract
3. relevant Platform subsystem contract
4. relevant Engine API contract
5. bring-up evidence and runbooks
6. Game or Reference Game intent notes

Any rule conflict must be fixed in docs immediately.

