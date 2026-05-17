# Memory and Budgeting Contract

This document defines memory budgets and placement rules to keep behavior deterministic and low risk.

---

## Goals

- deterministic memory use
- explicit stack and queue budgets
- controlled retained-RAM usage
- no hidden dynamic allocation

---

## SRAM Budget Model

Define and maintain:
- per-thread stack budget
- per-queue storage budget
- per-subsystem working buffer budget
- retained memory budget
- package/runtime working budget
- trace/telemetry budget
- static asset staging budget

All budgets must be checked into source control.

---

## Budget Ledger Schema

Memory budgets should be recorded in a generated or maintained ledger.

Conceptual schema:

```text
memory_budget:
  budget_id
  budget_version
  board_revision
  firmware_commit
  build_profile
  target_profile
  linker_script_ref
  map_file_ref
  knobs_hash
  sections[]
  owners[]
  margins[]
  evidence_refs[]
```

Section record:

```text
section_budget:
  memory_region
  linker_section
  owner
  purpose
  budget_bytes
  measured_bytes
  headroom_bytes
  enforcement
```

Owner record:

```text
owner_budget:
  owner
  stacks_bytes
  queues_bytes
  static_buffers_bytes
  retained_bytes
  dma_safe_bytes
  trace_bytes
  notes
```

Rules:

- every owner thread must have stack and queue budget records.
- every DMA-facing buffer must identify memory region and alignment.
- every retained object must identify validity fields and owner.
- map-file evidence must be linked before budgets are treated as proven.
- content/package budgets must be target-profile visible only as abstract limits, not addresses.

---

## Budget Categories

Required categories:

| Category | Purpose |
|---|---|
| owner stacks | ThreadX owner thread stacks |
| owner queues | static queue storage and message pools |
| Platform static buffers | display, audio, storage, communication, sensor, input, shell buffers |
| Engine runtime RAM | runtime host state, state graph interpreter, package context |
| package working RAM | package-visible bounded working memory |
| renderer working RAM | layer planes, compositor scratch, frame state |
| SRAM4 display payloads | final framebuffer, transfer payloads, precomposed low-power sequences |
| retained fast-resume | STOP-class continuity state |
| trace/telemetry | Tracealyzer buffers, SWO/telemetry rings, dashboard capture staging |
| install/storage staging | package import and validation buffers |
| safety margin | reserved headroom per region |

Rules:

- category totals must not hide owner-specific budgets.
- safety margin is a budgeted item, not accidental free space.
- development/instrumented builds may have larger trace budgets than release builds.
- shipping target profiles must not depend on development-only trace buffers.

---

## Retained RAM Contract

For retained structures:
- include `magic`, `version`, and `crc`
- define ownership of writes
- define update frequency limits
- define fallback behavior when invalid

Retained RAM is continuity-only, not durable storage.

---

## SRAM4 Design Contract

SRAM4 is the intended HW5 memory region for two separate jobs:

1. DMA-safe display buffers and low-power display sequence payloads.
2. Small retained fast-resume state across STOP-class sleep.

SRAM4 display use includes:

- final composed panel framebuffer
- display transmit payload buffer
- validated precomposed low-power sequence payloads where used

SRAM4 retained-state use includes:

- runtime/power fast-resume snapshot
- scene/runtime binding IDs needed to resume coherently
- integrity fields such as `magic`, `version`, `crc`, and `valid_mask`

Rules:

- SRAM4 is not durable storage.
- retained fast-resume state is not a replacement for flash-backed save data.
- normal SRAM remains the default location for RTOS stacks, queues, renderer working planes, package working RAM, and owner-thread state.
- external flash remains the durable home for installed package assets, saves, metadata, and fault logs.
- precomposed low-power sequence playback must not depend on normal renderer working RAM.
- package artifacts must not encode SRAM4 addresses or linker-section assumptions.
- exact SRAM4 budget, linker sections, retention behavior, and DMA reachability must be proven on HW5 before shipping profiles grant autonomous display playback.

Budget records must distinguish:

- display framebuffer bytes
- display TX/payload buffer bytes
- low-power sequence payload bytes
- retained fast-resume bytes
- reserved/alignment margin

Known display formulas for HW5 planning:

```text
panel_width = 144
panel_height = 168
line_bytes = panel_width / 8
framebuffer_bytes = line_bytes * panel_height
```

Protocol/TX payload buffers must include Sharp LCD command, row address, per-row dummy bytes, and final dummy bytes according to the display contract.

Rules:

- exact linker section names and retention settings must be recorded before implementation freeze.
- LPBAM/autonomous display sequence payload budgets are granted only by target profiles with measured evidence.
- retained fast-resume state must fit after display and low-power sequence reservations with explicit margin.

---

## Flash Layout Contract

Define fixed regions for:
- boot and app images
- package staging area
- package installed area
- save data area
- metadata/index area
- protected persistent fault-log ring near the end of external flash

Layout changes require migration notes and compatibility review.

Persistent fault-log ring rules:

- fixed protected external-flash region
- not host-exposed
- separate from settings, calibration, saves, package blobs, indexes, and staging
- append/ring records with magic, version, sequence, CRC, and compact fault payload
- exact offset and size assigned in the flash-layout pass

---

## Allocation Rules

- No runtime heap allocation in owner threads.
- Any optional allocator must be bounded and documented.
- Compile-time static allocation is the default.
- Package/runtime allocation must be through bounded Engine-owned arenas where used.
- Platform owners must not allocate from package arenas.
- Packages must not choose memory banks, linker sections, or addresses.
- Instrumentation buffers must be build-profile gated.
- Memory-affecting Platform knobs are `compile_time` or `protected_policy` unless a specific owner proves a live-safe boundary.

---

## Target Profile Exposure

Target profiles may publish abstract memory limits:

- package total bytes
- package asset bytes
- package working RAM bytes
- content parameter blob bytes
- save record bytes
- diagnostics/event rate limits
- runtime unit count and state/action table limits

Target profiles must not publish:

- raw SRAM addresses
- linker symbols
- flash offsets
- stack addresses
- queue addresses
- DMA descriptor addresses
- SRAM4 placement details

---

## Measurement and Enforcement

Required checks:
- map-file stack and section checks in CI
- queue depth stress checks
- retained-RAM corruption tests
- install path flash usage checks
- trace/instrumented build memory overhead reports
- package compatibility budget checks against [[Target_Profile_Schema_Contract]]
- SRAM4 placement and DMA reachability evidence where display DMA or LPBAM is used

Publish results in [[Memory_Reports]].

---

## Validation Cases

1. map file budget report lists every owner stack, queue, and static buffer.
2. SRAM4 report separates framebuffer, TX payload, low-power sequence payloads, retained fast-resume, and margin.
3. package validation fails if package working RAM or asset limits exceed target profile.
4. instrumented build reports trace/telemetry overhead separately from release build.
5. retained RAM corruption falls back to safe defaults.
6. package artifacts contain no memory addresses or linker-section assumptions.
