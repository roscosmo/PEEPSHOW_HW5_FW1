# Agent Working Rules

This repository uses `docs/` as an Obsidian vault and architectural source of truth.

The repository root is not the vault. Most project documentation links should be written as Obsidian wikilinks, for example `[[Authority_and_Invariants]]`.

These rules are for coding agents working on PeepShow HW5 firmware, documentation, package tooling, or development tools.

---

## Architecture Boundary

Use this model:

```text
Platform -> Engine -> Packages / Reference Game
```

The Platform owns hardware behavior and must remain game-agnostic.

The Engine owns reusable runtime abstractions, package/content contracts, and game-development APIs. It must remain reusable beyond the Reference Game.

Packages consume Engine APIs. Game-authoring tools generate package data against Engine/package contracts and read-only target profiles.

The Reference Game is the PeepShow proof-of-capability package. It validates the Platform, Engine, and tools, but it does not define hardware behavior, Platform policy, or Engine API semantics.

---

## Documentation Authority

Before firmware implementation or contract changes, check:

1. `docs/01 Platform/Authority_and_Invariants.md`
2. relevant Platform contract
3. relevant Hardware contract
4. relevant Engine API contract
5. relevant package/tooling contract
6. Bring-up tracker, runbook, or evidence artifact

If documents conflict, `Authority_and_Invariants.md` wins until the conflict is resolved explicitly.

Game notes may express intent and capability needs. They may not redefine Platform policy, CubeMX configuration, RTOS ownership, sleep behavior, storage ownership, DMA policy, target-profile limits, or package validation rules.

When behavior changes, update the authoritative contract in the same change. Do not leave code and docs intentionally divergent.

---

## Canonical Runtime Classes

Use these runtime class tokens exactly unless a newer Engine contract replaces them:

- `SHELL`
- `LP_GRAPH`
- `LP_MODULE`
- `RT_SCENE`
- `INSTALLER`

Do not invent package runtime classes in implementation code or package schemas without updating the Engine contracts and capability registry.

---

## Bring-up And Evidence Rules

Bring-up completion requires measured evidence in `docs/02 Bring-up/Brought_Up_Tracker.md`.

Do not mark hardware behavior as known-good without measured HW5 evidence.

Use the subsystem runbooks in `docs/02 Bring-up/` for bring-up procedure and expected evidence. Evidence artifacts should follow `docs/02 Bring-up/Evidence_Artifact_Convention.md`.

If a constant, timing, budget, cadence, wake behavior, or capability is not measured yet, keep it pending in `docs/02 Bring-up/Pending_Measured_Constants_Register.md` or the relevant contract. Do not freeze guessed values as shipping facts.

Target profiles are read-only published facts derived from measured Platform behavior. A profile may contain `pending_validation` values before hardware evidence exists, but shipping package export must not rely on pending capabilities.

The digital twin is implemented after the corresponding hardware behavior has been validated. It is contract-accurate host runtime evidence for Engine/package logic, not hardware bring-up evidence and not an STM32 emulator.

Tracealyzer or other trace snapshots may be used as bring-up evidence when the runbook identifies the expected behavior and the artifact is recorded.

---

## Platform Implementation Rules

Platform owns:

- hardware behavior
- RTOS ownership
- clocks and sleep policy
- peripheral access
- storage and USB arbitration
- fault handling
- bring-up evidence

Every peripheral and shared subsystem has exactly one Platform owner. Other code must send typed requests to the owner.

Do not add cross-thread direct HAL, LL, register, DMA, or peripheral access. ISR code should signal and return quickly.

Platform owner threads own sequencing, retries, health state, fault classification, and user-visible behavior.

Do not bypass Platform owners from Engine, package runtime, debug tooling, or Reference Game code.

---

## Engine And Package Rules

Engine code consumes Platform capabilities; it must not own hardware.

Package-facing APIs expose PeepOS primitives, not HAL, RTOS, filesystem, flash, DMA, pin, register, or board details.

Normal game-authoring tools should never expose raw HAL access, dirty rows, SRAM placement, filesystem paths, DMA descriptors, LPBAM descriptors, RTOS objects, or direct hardware settings.

Package tools validate before compilation/export. Validation should explain failures in PeepOS terms, not low-level hardware terms. Low-level forbidden-token checks are internal guardrails for toolchain defects, corrupted artifacts, malicious packages, or advanced tooling.

Packages may declare:

- runtime units
- capability requirements
- optional capability fallbacks
- content parameters
- package-owned settings
- save schemas
- bounded diagnostics

Packages may not mutate Platform knobs, Platform settings, calibration, BLE bonding, install metadata, power policy, or hardware policy.

---

## Target Profiles And Compatibility

Target profiles are defined by `docs/03 Engine API/Target_Profile_Schema_Contract.md`.

Capability names come from `docs/03 Engine API/PeepOS_Capability_Registry.md`.

Every package build must produce a compatibility report following `docs/06 Assets Pipeline/Package_Compatibility_Report_Contract.md`.

Compatibility reports are tool-side validation artifacts. They do not grant capabilities and do not replace firmware install validation.

Firmware install validation must still re-check package integrity, schema compatibility, runtime class compatibility, and required capability compatibility.

If a target-profile limit changes, stale compatibility reports must be invalidated.

---

## Knobs, Parameters, And Settings

Platform knobs are firmware tuning/configuration inputs for PeepOS internals. They are not game-authoring controls.

Generated knob outputs must not be edited manually. Update the source schema and regenerate.

Content parameters are package-authored balancing or behavior values.

Package-owned settings are package/user preferences backed by package save/settings schemas.

Do not promote package balancing values or game preferences into Platform knobs.

Hardware-affecting behavior should be represented as bounded capability contexts or Platform-owned policy, not package settings mutation.

---

## Power And Low-Power Policy

PeepOS is a low-power device first.

User inactivity timeouts always apply where Platform policy requires them. Packages must provide declared low-power routes rather than blocking sleep.

`LP_GRAPH` and `LP_MODULE` should be the natural home for long-running low-power package behavior.

`RT_SCENE` is allowed for richer interactive behavior but must declare frame budgets, capability contexts, and fallback/idle routes.

`ULP_ANIM` or autonomous low-power display behavior is available only when the selected target profile grants the relevant display capability with evidence.

LPBAM, LPDMA, STOP modes, SRAM4 retention, display transfer policy, and wake source wiring are Platform/HW concerns. Packages and tools express intent only.

---

## Storage And USB Rules

Storage owner is sole owner of flash and filesystem operations.

Engine/package runtime must not use FAT or host-visible files for active gameplay/runtime execution.

USB MSC is the normal universal package ingress path unless a contract says otherwise.

VBUS presence is external-power evidence only. Do not treat VBUS attach alone as MSC availability, a flashing prompt, or a storage handoff; USB data-host activity/enumeration and the storage/installer contract gate MSC entry.

For USBX MSC bring-up or regression work, follow `docs/02 Bring-up/USB_MSC_Bring-up_and_Recovery_Runbook.md` before speculative USB/storage patches.

USB CDC development mode is a dev workflow for commands, live tuning, telemetry, and file/package transfer where implemented. Composite MSC+CDC should be treated as future/optional unless documented.

Package install is not Platform firmware update. Do not implement native executable game-slot loading, bootloader writes, Platform firmware writes, option-byte changes, or production security lock-down from a package/tooling request unless the relevant Platform update/security contract is explicitly updated first.

Do not add package runtime dependencies on host paths, direct filesystem paths, flash offsets, erase pages, or raw storage regions.

---

## Diagnostics And Development Tools

Package diagnostics are bounded Engine records: markers, counters, timing scopes, trace values, and package fault codes.

Platform diagnostics own hardware faults, protected logs, transport routing, HardFault capture, and bring-up evidence.

Development dashboards, USB CDC, SWO, Tracealyzer, and telemetry transports are dev-tool or Platform concerns. Packages may emit bounded diagnostics but do not own export channels.

Digital twin, telemetry dashboard, live tuning tools, and orchestration CLI should use the same contracts as device runtime wherever possible.

Agent-run build, flash, and debugger inspection are allowed only through bounded wrappers documented in `docs/02 Bring-up/Bounded_Build_Flash_Debug_Runbook.md`.

Do not start raw interactive build/flash/GDB sessions. Hardware-attached actions such as flashing, reset, ST-Link GDB server startup, live target attach, or target resume require explicit user intent for that run.

Safe unattended GDB use is limited to batch-mode register/status reads and known non-resume `debug.gdb` helpers. Do not automatically run `continue`, `run`, stepping commands, `_wait` helpers, or helpers that require `Ctrl-C` unless the user explicitly approves an externally timed scenario.

---

## Documentation Style

Keep documentation game-agnostic unless it lives under `docs/05 Reference Game/`.

Use exact contract terms. If a name is too vague, rename it before implementation.

Prefer explicit statuses such as `pending_validation`, `granted`, `blocked`, `experimental`, `dev_only`, or `shipping`.

When documenting unknown hardware behavior, say what must be measured and where the result will be recorded.

Do not write speculative implementation detail as fact. Use design intent, pending validation, or open question language when hardware has not been validated.

---

## Code Change Discipline

Before making code changes, read the relevant authority and contract documents.

Use existing owner threads, queues, state machines, and helper APIs where they exist.

Keep edits scoped to the requested layer and subsystem.

Do not manually edit generated files unless the repository explicitly documents that workflow.

Do not silently change CubeMX, linker, DMA, clock, pin, power, or storage policy from a game/API request.

If a requested implementation crosses a documented boundary, update the correct contract or ask for clarification before coding.

Do not mark tests, bring-up steps, or validation items complete unless they were actually run or measured.

---

## Git And Workspace Safety

The worktree may contain user changes.

Do not revert or overwrite unrelated changes.

Do not use destructive git commands unless explicitly requested.

When reporting work, mention files changed and any verification that could not be run.
