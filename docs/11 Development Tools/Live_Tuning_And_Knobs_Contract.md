# Live Tuning and Knobs Contract

This document defines how PeepShow development tools may inspect and change live-safe tuning values without reflashing firmware.

Live tuning is a developer workflow. It is not raw memory poking and it is not a normal game/package API.

Related:

- [[Development_Tooling_Index]]
- [[Knobs_and_Tuning_Contract]]
- [[USB_Development_Mode_Contract]]
- [[Debug_and_Observability]]
- [[Debug_Workflows]]
- [[Authority_and_Invariants]]
- [[RTOS_Ownership_and_Queue_Topology]]
- [[Brought_Up_Tracker]]

---

## Scope

Defines:

- live-safe tuning model
- generated tuning registry requirements
- owner-routed apply behavior
- session overlays and persistence rules
- host tooling behavior
- validation and evidence requirements

Does not define:

- the exact CDC packet format
- host GUI implementation
- firmware generator implementation details
- package balancing schemas beyond the boundary with package tooling

---

## Core Principle

The authoritative knob source remains the repository knob schema and JSON.

```text
config/knobs.json
config/knobs.schema.json
    |
tools/gen_knobs.py
    |
generated firmware constants
generated live-tuning metadata
generated host/editor metadata
```

Live tuning may temporarily override approved values during a developer session.

A useful live value becomes permanent only when the source knob file, schema, package content, or package tuning data is updated and rebuilt through the normal pipeline.

---

## Non-Goals

Live tuning must not:

- mutate arbitrary memory
- bypass owner-thread requests
- edit generated headers directly
- silently change firmware defaults
- patch Platform policy in shipping builds
- expose hardware controls to package/game tools
- replace measured bring-up evidence

GDB memory writes may remain an emergency/debugger technique, but they are not the supported live-tuning workflow.

---

## Tuning Classes

Every tunable value should declare one tuning class.

| Class | Meaning | Live Editable |
|---|---|---|
| `compile_time` | memory layout, stack sizes, queue depths, object counts, static hardware choices | no |
| `boot_applied` | defaults applied during init or boot | no, or reboot-required |
| `runtime_live_safe` | can be changed at a safe runtime boundary through owner validation | yes in developer mode |
| `package_tunable` | package/reference-game balancing or content tuning through Engine/package tooling | yes where package policy allows |
| `protected_policy` | power, storage, PMIC, safety, or architecture policy with high risk | no, except explicit bring-up firmware policy |

Rules:

- live tools may only expose `runtime_live_safe` and approved `package_tunable` values by default.
- `protected_policy` values require explicit bring-up firmware or maintainer-only policy before any live change path exists.
- `compile_time` values never become live-editable because they affect static allocation, object shape, or build-time layout.
- `boot_applied` values may be staged for next boot only if the owner subsystem supports a safe staged update.

---

## Live Tuning Metadata

Every live-editable value must have generated metadata.

Required fields:

```text
live_tunable:
  id
  path
  owner
  class
  type
  min/max or enum values
  default_value
  current_value_policy
  apply_policy
  persistence_policy
  build_profile_gate
  target_profile_gate
  units
  timebase_domain
  description
  safety_notes
```

Rules:

- `path` is stable and human-readable.
- `owner` names the Platform owner, Engine service, or package/runtime service that validates and applies the value.
- `type` must be explicit.
- numeric ranges must be explicit.
- enum values must be explicit.
- timing values must declare their timebase domain.
- descriptions are host/editor metadata, not firmware logic.
- metadata must be generated from the same source as the compile-time knob contract.

---

## Owner-Routed Apply Model

Live tuning commands flow through owner requests.

```text
host tool
    |
CDC dev protocol or digital twin control adapter
    |
live tuning registry
    |
owner-routed apply request
    |
owner validates, clamps, rejects, stages, or applies
    |
structured result + telemetry
```

Owners may return:

| Result | Meaning |
|---|---|
| `applied` | value is active now |
| `staged` | value will apply at the declared safe boundary |
| `clamped` | value was adjusted to the owner-approved range |
| `rejected` | value is invalid or unsafe |
| `requires_reboot` | value can only apply after reboot/reinit |
| `unsupported` | selected build/target/profile does not allow this tunable |

Rules:

- command handlers must not write the target variable behind the owner.
- owner apply functions must run at a safe boundary.
- owner apply functions must be bounded.
- changes must be observable through response or telemetry.
- failed apply must leave the previous value valid.

---

## Apply Policies

Allowed apply policies:

| Apply Policy | Meaning |
|---|---|
| `immediate_if_idle` | apply now only if owner is idle |
| `next_event` | apply before next owner event dispatch |
| `next_frame` | apply at next frame boundary |
| `next_static_update` | apply at next static/low-power update boundary |
| `next_unit_transition` | apply when runtime unit changes |
| `next_boot` | stage for next boot only |
| `rebuild_required` | not live-editable |

Rules:

- display/render values should apply at render or frame boundaries.
- input filter values should apply between classifier windows or after input owner quiesce.
- audio values should apply at cue/buffer-safe boundaries.
- sensor values should apply through sensor context boundaries.
- storage and power protected policy values must not be live-edited without explicit bring-up policy.

---

## Session Overlays

Live tuning edits are session overlays unless explicitly persisted.

Overlay records should include:

```text
tuning_overlay:
  firmware_commit
  knobs_hash
  board_revision
  target_profile
  changed_values[]
  timestamp
  tool_version
```

Rules:

- overlays must record the base knob-set hash.
- overlays must be exportable as a developer artifact.
- overlays must not silently become firmware defaults.
- overlays used during bring-up evidence must be recorded with the test result.
- applying an overlay to a mismatched firmware/knob hash should warn or reject.

---

## Persistence Policy

Persistence is optional and owner-specific.

Allowed persistence forms:

- session-only overlay
- staged next-boot development setting
- exported patch for `config/knobs.json`
- package tuning export through package tooling

Rules:

- Platform persistent writes route through `thStorage`.
- persistence must be power-fail safe.
- protected Platform settings, calibration, bonding records, installed indexes, and fault logs are not edited through generic live tuning.
- generated headers are never edited by live tools.
- host tools should make source-update/export explicit.

---

## Package Tunables

Package tunables are separate from Platform knobs.

Examples:

- Reference Game balancing values
- animation timing for package content
- pet hunger/energy rates
- encounter weights
- dialogue timing
- package-local difficulty defaults

Rules:

- package tunables must use Engine/package schemas.
- package tunables may be previewed through the digital twin or CDC developer mode where allowed.
- package tunables must compile into package data or package-owned settings.
- package tunables must not mutate Platform settings or hardware policy.

---

## CDC Command Shape

The USB CDC developer protocol is defined in [[USB_Development_Mode_Contract]].

Expected live-tuning commands:

```text
knob.list
knob.describe path
knob.get path
knob.set path value
knob.apply path
knob.stage path value
knob.overlay.export
knob.overlay.clear
```

Rules:

- commands operate on stable paths, not raw addresses.
- values are encoded by schema.
- every command has bounded payload size.
- invalid values are rejected before owner apply where possible.
- owner apply still validates the value.

---

## Digital Twin Behavior

The digital twin should use the same live-tuning metadata where practical.

Rules:

- twin live tuning follows the same class and bounds rules.
- twin may provide faster UI iteration, but it must still model owner-routed apply semantics.
- twin-only changes are not hardware evidence.
- replay captures must record any active tuning overlay.

---

## Evidence Requirements

Any validation or bring-up run using live tuning must record:

- firmware commit
- board revision
- knobs hash
- active tuning overlay
- tool version
- active USB personality, if CDC was used
- whether instrumentation or live tuning may affect timing/power

Do not compare current, wake timing, or STOP behavior against baseline runs unless the active tuning overlay and instrumentation state are known.

---

## Forbidden Live Tuning Targets

The normal live-tuning surface must not expose:

- stack sizes
- queue depths
- RTOS object counts
- flash region layout
- filesystem mount policy
- installed package index internals
- PMIC safety thresholds except explicit bring-up firmware policy
- clock tree or PLL changes
- DMA descriptors
- SRAM4 buffer placement
- LPBAM descriptors
- raw sensor calibration storage
- BLE bonding storage
- persistent fault log internals
- raw memory addresses or pointers

---

## Validation Cases

1. generated live-tuning registry matches `config/knobs.json` and schema metadata.
2. `runtime_live_safe` knob can be listed, described, set, and applied through owner request.
3. out-of-range live value is clamped or rejected according to metadata.
4. non-live-safe knob is not exposed in normal live tuning and is rejected if requested directly.
5. owner apply failure preserves previous valid value.
6. staged next-boot value does not silently alter compile-time defaults.
7. overlay export records firmware commit, knobs hash, board revision, changed values, and tool version.
8. live-tuned validation evidence records active overlay and USB personality.
9. digital twin replay records and reapplies the same tuning overlay for deterministic runs.
10. raw memory address tuning command is unavailable.

---

## Rule

Live tuning is generated, typed, bounded, and owner-routed.

It helps find values quickly, but the repository source remains the authority.
