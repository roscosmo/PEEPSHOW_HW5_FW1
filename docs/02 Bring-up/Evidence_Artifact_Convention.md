# Evidence Artifact Convention

This document defines naming, storage, and metadata conventions for bring-up and validation evidence artifacts.

Evidence proves behavior only when it is linked from [[Brought_Up_Tracker]] and includes enough metadata to reproduce the test context.

Related:

- [[Brought_Up_Tracker]]
- [[Validation_Plan]]
- [[Debug_Workflows]]
- [[Power_Measurement_and_Trace_Correlation_Runbook]]
- [[Tracealyzer_Snapshot_Evidence_Contract]]
- [[Telemetry_And_Debug_Dashboard_Contract]]
- [[Dev_Orchestration_CLI_Contract]]
- [[Knobs_and_Tuning_Contract]]

---

## Purpose

The evidence convention prevents bring-up artifacts from becoming untraceable.

It standardizes:

- evidence IDs
- artifact paths
- capture metadata
- trace/dashboard/current-log naming
- link format from the tracker
- distinction between hardware evidence, host evidence, and design-time output

---

## Evidence Classes

| Class | Meaning | Can Prove Hardware? |
|---|---|---|
| `hw_measurement` | measured on HW5 hardware with physical instrument or firmware capture | yes |
| `hw_trace` | Tracealyzer/SWO/telemetry from HW5 firmware | yes for scheduling/state evidence, not electrical behavior |
| `hw_log` | firmware/dev-tool log from HW5 | yes where test case allows |
| `host_twin` | digital twin replay/capture | no |
| `host_tool` | package compiler, validator, asset pipeline, dashboard decode | no |
| `design_doc` | schematic/doc/spec extract | no by itself |
| `photo_video` | physical photo/video evidence | yes only for visible/mechanical/display behavior |

Rules:

- digital twin artifacts must never be recorded as hardware bring-up evidence.
- Tracealyzer snapshots can prove owner scheduling and state sequencing, not current draw.
- current, voltage, wake latency, and sleep behavior need physical or HW firmware measurement artifacts.

---

## Evidence ID Format

Use stable evidence IDs:

```text
EV-YYYYMMDD-PHASE-SUBSYSTEM-NNN
```

Examples:

```text
EV-20260603-P1-DISPLAY-001
EV-20260604-P6-SLEEP-002
EV-20260605-P7-USB-001
```

Rules:

- `PHASE` matches the bring-up phase where practical, for example `P1`.
- `SUBSYSTEM` uses an uppercase short name, for example `POWER`, `DISPLAY`, `STORAGE`, `AUDIO`, `INPUT`, `SENSOR`, `BLE`, `SLEEP`, `USB`, `RTOS`, `TWIN`.
- evidence IDs are not reused.
- failed tests still get evidence IDs when artifacts are useful.

---

## Artifact Path Convention

Preferred path shape:

```text
docs/02 Bring-up/Evidence/YYYY/MM/DD/EV-YYYYMMDD-PHASE-SUBSYSTEM-NNN/
```

Each evidence folder should contain:

```text
manifest.md
artifacts...
```

Example:

```text
docs/02 Bring-up/Evidence/2026/06/03/EV-20260603-P1-DISPLAY-001/
  manifest.md
  display_pattern_photo.jpg
  telemetry.jsonl
  trace.psfs
  notes.md
```

Rules:

- artifact filenames should be descriptive and stable.
- raw exported artifacts should be preserved where practical.
- derived screenshots/plots should identify the source artifact.
- do not store secrets, host usernames, raw private filesystem paths, or protected storage dumps in evidence artifacts.

---

## Manifest Template

Each evidence folder should include `manifest.md`.

```text
# EV-YYYYMMDD-PHASE-SUBSYSTEM-NNN

## Summary

- Test case:
- Result:
- Date/time:
- Maintainer:
- Board revision:
- Firmware commit:
- Build profile:
- Target profile:
- Platform contract revision:
- Knobs hash/version:
- Active tuning overlay:
- Instrumentation:

## Setup

- Hardware:
- Instruments:
- Host OS/tool versions:
- USB personality:
- Trace profile:
- Telemetry schema version:

## Artifacts

| Artifact | Type | Purpose |
|---|---|---|
| file.ext | hw_measurement | |

## Observations

## Conclusion

## Follow-Ups
```

Rules:

- `Result` must be `PASS`, `FAIL`, `PARTIAL`, `BLOCKED`, or `INFO`.
- `Active tuning overlay` must be `none` or list the overlay artifact/path.
- `Instrumentation` must note Tracealyzer, SWO, CDC telemetry, dashboard, current probe, logic analyzer, oscilloscope, or none.
- conclusions must not mark behavior known-good unless the artifact class can prove it.

---

## Tracker Link Format

[[Brought_Up_Tracker]] rows should reference evidence IDs and paths.

Example:

| Date | Test Case | Mode/Host | Result | Artifact | Notes |
|---|---|---|---|---|---|
| 2026-06-03 | P1-DISPLAY-PATTERN | HW5 | PASS | `EV-20260603-P1-DISPLAY-001` | logical/native mapping verified |

Rules:

- tracker rows link to evidence folders, not loose screenshots where practical.
- tracker notes summarize the result, not the full evidence.
- detailed setup and raw artifacts live in the evidence manifest/folder.

---

## Required Artifact Metadata By Type

| Artifact Type | Required Metadata |
|---|---|
| Tracealyzer snapshot | trace profile, buffer size, firmware commit, thread names, capture window |
| SWO/telemetry log | schema version, source, firmware commit, timestamp basis |
| dashboard capture | dashboard version, telemetry schema, source type, profile |
| current/power log | instrument model, sample rate, shunt/range, calibration notes, source voltage/current limit, sync marker strategy |
| logic analyzer/oscilloscope | instrument, channel mapping, sample rate, trigger condition |
| display photo/video | pattern name, expected output, lighting notes where relevant |
| package validation output | package hash, target profile, validator version |
| digital twin replay | package hash, twin profile, input/time/sensor trace, content parameter overrides |

---

## Evidence Boundary Rules

- hardware evidence must identify board revision and firmware commit.
- profile-derived evidence must identify target profile and profile version.
- all evidence using live tuning must record active Platform knob overlay.
- all evidence using content parameter overrides must record package parameter hash/override file.
- failed tests must preserve enough information to reproduce the failure.
- evidence should distinguish measured data, interpretation, and follow-up actions.

---

## Validation Cases

1. evidence folder contains `manifest.md`.
2. manifest records board revision, firmware commit, knobs hash/version, and active instrumentation.
3. tracker row links to evidence ID.
4. digital twin evidence is labeled `host_twin` and not used as hardware proof.
5. Tracealyzer evidence records trace profile and capture window.
6. current measurement evidence records instrument and sample configuration.

---

## Rule

If behavior is important enough to mark known-good, it needs an evidence ID, a manifest, and a tracker link.
