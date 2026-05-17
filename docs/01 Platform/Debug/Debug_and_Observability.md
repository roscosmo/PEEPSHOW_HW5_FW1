# Debug and Observability Contract

This document defines what must be observable in bring-up and runtime without destabilizing low-power behavior.

Package-facing diagnostics are defined in [[Diagnostics_API_Contract]]. Packages do not own debug transports or persistent fault storage.

Tracealyzer snapshot evidence policy is defined in [[Tracealyzer_Snapshot_Evidence_Contract]].

Dashboard-facing telemetry is defined in [[Telemetry_And_Debug_Dashboard_Contract]].

---

## Allowed Debug Channels

- SWD (required)
- SWO structured events (preferred)
- Tracealyzer snapshot/ring-buffer traces for RTOS and owner-thread bring-up evidence
- USB CDC developer personality optional, rate-limited, and mutually exclusive with MSC in v1

No UART printf console policy by default.

USB CDC developer mode is defined in [[USB_Development_Mode_Contract]]. Live tuning behavior is defined in [[Live_Tuning_And_Knobs_Contract]].

Rules:

- SWO/SWV is primarily observation from device to host.
- CDC developer mode is the structured control path for live-safe tuning, telemetry queries, captures, and package upload.
- CDC commands must route through owner-thread or Engine service requests.
- CDC must not expose raw memory, protected storage, HAL handles, RTOS objects, or arbitrary filesystem paths.
- release/shipping builds must disable CDC developer control unless a future policy explicitly defines a limited diagnostic subset.

---

## Required Event Visibility

Must be observable with low overhead:
- mode transitions
- subsystem state transitions
- runtime lifecycle transitions
- storage ownership transitions
- fault and recovery transitions
- owner-thread scheduling and queue/event wake paths during trace-enabled bring-up builds
- dashboard-facing state vector and bounded telemetry events where a dev/debug profile enables them

---

## HardFault Record Contract

On HardFault capture:
- PC, LR
- stacked registers
- CFSR, HFSR
- MMFAR, BFAR when valid
- current mode/runtime state identifiers

HardFault data must be captured before reset.

---

## Breakpoint Policy

- avoid high-frequency ISRs and DMA callbacks
- cap active breakpoints during runtime debugging
- prefer event markers over stop-heavy breakpoint sessions for STOP validation

---

## STOP2 Debug Policy

- enable debug-in-low-power when investigating STOP behavior
- classify every wake source with evidence
- avoid instrumentation that materially changes sleep behavior

---

## Production Build Guardrails

Release builds must:
- compile out verbose debug paths
- keep structured fault capture
- preserve deterministic timing behavior
