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

This is the authoritative compile-time path.

Live tuning, where allowed, is a developer-only extension defined by [[Live_Tuning_And_Knobs_Contract]] and exposed through [[USB_Development_Mode_Contract]] or the digital twin. It must use generated metadata and owner-routed apply requests, not raw memory pokes.

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

## Tuning Classes

Detailed live tuning behavior is defined in [[Live_Tuning_And_Knobs_Contract]].

Every knob should declare a tuning class.

| Class | Meaning | Live Editable |
|---|---|---|
| `compile_time` | memory layout, stack/queue depth, object counts, static hardware configuration | no |
| `boot_applied` | defaults loaded or applied during boot/init | no, or reboot-required |
| `runtime_live_safe` | value can be changed at a safe runtime boundary through owner validation | yes in developer mode |
| `package_tunable` | package/reference-game balancing or content tuning through Engine/package tooling | yes where package policy allows |
| `protected_policy` | power, storage, PMIC, safety, or architecture policy with high risk | no, except explicit bring-up firmware policy |

Rules:

- only `runtime_live_safe` and approved `package_tunable` knobs may be changed through developer CDC live tuning.
- live-editable knobs must declare owner subsystem, type, bounds, apply timing, persistence behavior, and reset/reboot requirement.
- owner subsystem validates, clamps, rejects, or applies live changes.
- compile-time and protected-policy knobs must not be exposed as normal live controls.

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
