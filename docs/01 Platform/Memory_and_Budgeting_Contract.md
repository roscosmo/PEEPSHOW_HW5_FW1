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
