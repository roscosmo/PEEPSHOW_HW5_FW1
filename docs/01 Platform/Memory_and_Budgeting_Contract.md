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

All budgets must be checked into source control.

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

---

## Measurement and Enforcement

Required checks:
- map-file stack and section checks in CI
- queue depth stress checks
- retained-RAM corruption tests
- install path flash usage checks

Publish results in [[Memory_Reports]].
