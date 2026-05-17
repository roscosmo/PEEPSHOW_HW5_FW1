# USB Development Mode Contract

This document defines the v1 USB personalities used by normal users and developers.

The v1 rule is simple: USB MSC and USB CDC developer control are mutually exclusive personalities.

Related:

- [[Development_Tooling_Index]]
- [[Storage_and_Installer_Contract]]
- [[Debug_and_Observability]]
- [[Debug_Workflows]]
- [[Live_Tuning_And_Knobs_Contract]]
- [[Telemetry_And_Debug_Dashboard_Contract]]
- [[USB_MSC_Bring-up_and_Recovery_Runbook]]
- [[Knobs_and_Tuning_Contract]]
- [[Package_Blob_Format_Contract]]
- [[Package_Contract]]

---

## Scope

Defines:

- normal USB MSC package install/export behavior
- developer USB CDC behavior
- storage ownership rules for both personalities
- package upload and live-safe tuning boundaries
- composite USB policy for v1

Does not define:

- low-level USBX descriptor implementation
- host application UI
- package binary schema details
- Platform debug transport ownership beyond the USB personality boundary

---

## Core Rule

For v1, the device exposes one USB personality at a time.

| Personality | Audience | USB Interface | Storage Owner | Purpose |
|---|---|---|---|---|
| normal installer/export | normal users | MSC only | host while exported | copy packages and exported artifacts with no special tool |
| developer console | developers | CDC only | firmware / `thStorage` | package upload, live-safe tuning, telemetry, capture/export commands |

Composite `MSC + CDC` is explicitly deferred.

---

## Normal USB MSC Personality

Normal USB mode remains mass storage first.

Purpose:

- user copies installable `PeepPkg` packages
- user retrieves exported screenshots, logs, or diagnostic summaries
- no host-side PeepShow tool is required for basic package transfer

Rules:

- USB MSC exposes only the staging/export FAT volume.
- host owns the staging/export volume while MSC is active.
- firmware must not mount or write the staging/export FAT volume while the host owns it.
- settings, calibration, saves, installed packages, installed indexes, bonding records, and persistent fault logs remain protected.
- firmware reclaims and rescans staging/export only after host release/exit policy.
- active gameplay is disabled or policy-limited during installer/export mode.

This personality is governed by [[Storage_and_Installer_Contract]] and [[USB_MSC_Bring-up_and_Recovery_Runbook]].

---

## Developer USB CDC Personality

Developer USB mode is a structured command/control and telemetry personality.

Purpose:

- upload packages without exposing MSC
- inspect device state
- stream or query telemetry
- apply live-safe tuning values
- start and stop captures
- export bounded diagnostic artifacts through firmware-owned storage paths

Rules:

- CDC developer mode is not normal user package-install UX.
- CDC developer mode does not expose a host-writable FAT volume.
- firmware keeps storage ownership.
- all storage writes route through `thStorage`.
- all live tuning writes route through the owning Platform or Engine service.
- CDC commands must be typed, bounded, authenticated/gated by dev-mode policy where required, and rejectable.
- CDC must not expose raw memory, raw flash, HAL handles, RTOS objects, filesystem paths, or Platform private structs.
- CDC must be disableable in release/shipping builds unless a future policy explicitly allows a limited diagnostic subset.

---

## Dev Mode Entry Policy

Developer CDC mode must require an explicit dev-mode entry path.

Allowed entry mechanisms:

- debug build flag
- hidden boot chord
- system developer setting
- debugger-attached policy
- dedicated bring-up firmware image

Rules:

- normal USB attach should prefer MSC installer/export behavior.
- dev-mode entry must be visible in Platform state and diagnostics.
- dev-mode entry must not silently expose protected storage.
- failed dev-mode entry falls back to normal safe behavior or no USB personality.

Exact entry gesture is selected during Platform implementation and must be documented before shipping.

---

## CDC Command Families

The CDC protocol is structured. It is not a shell with arbitrary firmware access.

Expected v1 command families:

```text
device.info
device.state
trace.subscribe
trace.unsubscribe
capture.start
capture.stop
capture.export
knob.list
knob.get
knob.set
knob.apply
package.begin
package.write
package.end
package.validate
package.install
package.abort
```

Rules:

- every command has a schema and maximum payload size.
- binary transfers use explicit length, offset/sequence, checksum, and timeout.
- commands return accepted, rejected, clamped, failed, or completed status.
- long operations report progress through structured events.
- command handling must not block owner threads indefinitely.

---

## Package Upload Over CDC

CDC package upload is a structured package-transfer protocol, not arbitrary filesystem access.

Conceptual flow:

```text
package.begin package_id size checksum
package.write sequence offset bytes
package.end
package.validate
package.install
```

Rules:

- uploaded data must be a valid `PeepPkg` container.
- upload target is a firmware-owned staging area, not a host-mounted FAT volume.
- `thStorage` owns writes to flash.
- package manager validation remains mandatory.
- interrupted upload preserves the last known valid installed package/index.
- no command may write directly to installed package storage without validation.

---

## Live-Safe Tuning Over CDC

Live tuning is governed by [[Live_Tuning_And_Knobs_Contract]].

Live tuning must not be raw memory poking.

Live tuning flow:

```text
generated tuning registry
    |
CDC command schema
    |
owner-routed apply request
    |
validated applied/clamped/rejected result
```

Rules:

- only knobs marked live-safe may be edited at runtime.
- compile-time, memory-layout, queue-depth, stack-size, clock-tree, storage-layout, and protected power-policy knobs are not live-editable.
- live tuning metadata comes from the knobs/schema pipeline.
- each live tunable declares owner subsystem, type, min/max or enum values, apply timing, persistence behavior, and reset/reboot requirement.
- owner subsystem validates and applies the value at a safe boundary.
- changes are reflected in telemetry or explicit command response.
- persistence, if allowed, uses Platform settings/storage APIs and remains power-fail safe.

---

## Observation And Telemetry

Dashboard-facing telemetry is defined in [[Telemetry_And_Debug_Dashboard_Contract]].

Preferred observation channel remains SWO structured events where a debugger is attached.

CDC may provide additional telemetry in developer mode.

Allowed telemetry:

- Platform state vector
- runtime class and lifecycle state
- subsystem state transitions
- wake reason
- frame timing
- dirty row count
- storage owner state
- input event summaries
- sensor summaries
- audio queue/status summaries
- package diagnostics
- fault summaries safe for export

Rules:

- telemetry is bounded and rate-limited.
- protected storage and raw memory are not streamed.
- high-rate telemetry must be disableable.
- telemetry must not materially alter low-power validation unless the test explicitly records that instrumentation was active.

---

## Composite USB Policy

Composite `MSC + CDC` is not a v1 requirement.

It may be reconsidered only after:

- MSC installer/export behavior is validated
- CDC developer protocol is validated
- storage ownership and reclaim/rescan behavior are stable
- the project has a concrete need for simultaneous MSC and CDC
- composite descriptors and host behavior are tested on target host OSes

Until then:

- MSC mode means host owns the staging/export FAT volume.
- CDC dev mode means firmware owns storage and host uses structured commands.
- no workflow may require host and firmware to write the same FAT volume concurrently.

---

## Validation Cases

1. normal USB attach exposes MSC installer/export personality, not CDC dev control, unless dev entry policy is active.
2. MSC mode exports only staging/export storage.
3. firmware does not mount or write the staging/export FAT volume while host owns MSC.
4. dev-mode entry exposes CDC only and no MSC volume.
5. CDC package upload writes only through firmware-owned staging and `thStorage`.
6. corrupted CDC package upload fails validation without changing the active installed package.
7. live-safe knob edit is owner-routed, validated, and reported as applied, clamped, or rejected.
8. non-live-safe knob edit is rejected.
9. CDC telemetry is rate-limited and disableable.
10. release/shipping build disables CDC dev control unless a future policy explicitly allows a limited subset.

---

## Rule

MSC is the universal user transfer path.

CDC is the developer control path.

They are mutually exclusive in v1.
