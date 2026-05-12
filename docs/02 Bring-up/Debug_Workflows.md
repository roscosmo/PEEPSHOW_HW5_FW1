# Debug Workflows

This document defines practical debug workflows and command discipline.

---

## Debug Interface Policy

Primary:
- SWD

Preferred runtime visibility:
- SWO structured events

Optional:
- USB CDC diagnostics (rate-limited, disableable)

No general UART printf logging policy by default.

---

## debug.gdb Contract

Project root `debug.gdb` is authoritative.

Rules:
- keep only high-value breakpoints
- keep breakpoint count bounded
- remove stale experimental helpers
- keep helper names stable

---

## Required Fault Capture

On HardFault capture:
- stacked registers
- PC/LR
- CFSR/HFSR
- MMFAR/BFAR when valid
- current mode/runtime identifiers

HardFault data must be captured before reset.

---

## Bring-Up Debug Workflow

1. validate reset and early clocks
2. validate each peripheral in isolation
3. validate owner-thread message flow
4. validate sleep entry and wake traces

Prefer trace markers over frequent breakpoint halts.

---

## Runtime Debug Workflow

1. capture mode/runtime state markers
2. capture subsystem state transition markers
3. correlate with evidence in brought-up tracker
4. use breakpoints only for targeted confirmation

---

## Common Playbooks

No boot:
- connect under reset
- validate early init markers
- inspect fault record

Unexpected fault/reset:
- inspect HardFault data
- inspect last transition markers
- inspect owner health flags

Sleep not waking:
- verify wake-source classification
- inspect sleep/awake transition markers
- verify quiesce/resume acknowledgements

Installer corruption:
- verify storage ownership mode
- verify mount/export transitions
- verify single-writer behavior

---

## Evidence Logging Requirements

Each debug session must record:
- date/time
- board revision
- firmware commit
- knobs stamp/hash
- test focus
- artifact paths

Log summary should be copied into the brought-up tracker evidence section.

