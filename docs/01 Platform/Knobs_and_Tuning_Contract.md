# Knobs and Compile-Time Tuning Contract

This document defines the knobs system contract for all firmware tunables.

---

## Pipeline (Authoritative)

```
config/knobs.json
  -> tools/gen_knobs.py
  -> Core/Inc/knobs_autogen.h
```

Rules:
- firmware includes only generated header
- firmware never parses JSON at runtime
- knob changes require regenerate plus rebuild

---

## Required Files

- `config/knobs.json` source of truth
- `config/knobs.schema.json` validation and editor metadata
- `tools/gen_knobs.py` generator
- `Core/Inc/knobs_autogen.h` generated output

Generated header is never manually edited.

---

## Timebase Domain Contract

All timing knobs must declare:
- authored domain
- runtime compare domain

Allowed domains:
- `threadx`
- `hal_ms`
- `knob_rtos_tick_hz`

Any conversion must happen once at an explicit boundary.

---

## Adding a New Knob

1. add key to `config/knobs.json`
2. add schema entry with type and constraints
3. regenerate header
4. consume generated macro in firmware
5. document effect and owner subsystem

---

## Knob Hygiene Rules

- no duplicated tunables for same behavior
- no stale/unused knob keys
- names must include subsystem context
- tuning knobs must not bypass ownership boundaries

---

## Determinism Rules

Knobs must not:
- add random timing behavior
- introduce unbounded retries
- silently change architectural contracts

---

## Version and Traceability

Track a knob-set stamp for each bring-up and validation run:
- firmware commit
- knobs hash or version
- board revision

Store stamps in validation evidence records.

