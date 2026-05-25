# Power Measurement and Trace Correlation Runbook

This runbook defines how PeepShow uses external current measurement, firmware trace markers, Tracealyzer snapshots, and telemetry to diagnose and optimize PeepOS power behavior during HW5 development.

This is PeepOS Platform development tooling.

It is not part of the package/game developer API.

Related:

- [[Brought_Up_Tracker]]
- [[Evidence_Artifact_Convention]]
- [[Pending_Measured_Constants_Register]]
- [[Validation_Plan]]
- [[Debug_Workflows]]
- [[Tracealyzer_Snapshot_Evidence_Contract]]
- [[Telemetry_And_Debug_Dashboard_Contract]]
- [[Power_and_Sleep_Policy]]
- [[PMIC_and_Power_Contract]]
- [[Target_Profile_Schema_Contract]]

---

## Purpose

PPK2 or equivalent current measurement shows what the hardware consumed.

Tracealyzer, SWO, and telemetry show what PeepOS was doing.

The goal is to correlate those sources so power anomalies can be traced to concrete Platform behavior:

- mode transitions
- owner-thread activity
- display flushes
- sensor reads
- BLE/NINA activity
- audio output
- storage writes
- USB attach/enumeration/MSC
- sleep entry and wake paths

The output of this work feeds:

- measured constants
- Platform power policy
- target-profile limits
- future battery/runtime estimates
- regression evidence

---

## Non-Goals

This runbook does not define:

- game-facing power controls
- package-visible hardware telemetry
- user-facing battery UX details
- PPK2 software operation in detail
- Tracealyzer licensing or streaming setup
- final production test procedure

Package authors should see a power-optimized PeepOS API and target-profile limits, not current probes, GPIO sync pins, PMIC internals, or Tracealyzer artifacts.

---

## Measurement Sources

| Source | Proves | Does Not Prove |
|---|---|---|
| PPK2 current trace | current draw, spikes, plateaus, operation energy cost, average current | software cause by itself |
| Tracealyzer snapshot | ThreadX scheduling, owner ordering, queue/timer behavior | electrical current or STOP current |
| SWO/telemetry | Platform state vector, event timeline, firmware timestamps | electrical current by itself |
| optional sync GPIO | alignment cue between current trace and firmware events | power behavior by itself |

Rules:

- PPK2 or equivalent physical measurement is required for current and energy claims.
- Tracealyzer snapshots explain scheduling and ordering only.
- Telemetry can explain state and event context.
- Final low-power current evidence should be confirmed with trace overhead disabled unless the test explicitly accepts instrumentation overhead.

---

## Sync Strategy

### Preferred: Physical Sync Marker

If HW5 has a safe spare pin, reserve it in development builds as a power-trace sync output.

Recommended behavior:

- connect sync pin to a PPK2 digital input or logic capture channel
- emit a short pulse at major Platform markers
- mirror the same marker into Tracealyzer/SWO/telemetry
- document the pin, polarity, pulse width, and firmware build profile in the evidence manifest

Example marker flow:

```text
firmware emits POWER_SYNC_STOP_PREP
  -> Tracealyzer custom event
  -> SWO/telemetry event
  -> optional GPIO pulse visible in PPK2 timeline
```

Rules:

- the sync pin is dev-only instrumentation.
- it must not be required by package logic.
- it must not be used for product behavior.
- if the pin is shared with future hardware function, the evidence must record the temporary routing.

### Fallback: Timed/Cue-Based Correlation

If no safe pin is available, use deliberate timing cues:

- run a focused single-action scenario
- record a firmware timestamped marker stream
- include a clear preamble pattern, such as three display flushes or three short BBB beeps
- align PPK2 trace features to matching firmware markers
- record the expected correlation precision in the evidence manifest

Rules:

- fallback correlation is valid for diagnosis and coarse attribution.
- do not claim sub-millisecond marker alignment without a physical sync or equivalent shared timebase.
- unresolved ambiguity should remain in the evidence notes.

---

## Required Marker Classes

Power correlation builds should emit bounded Platform markers for:

| Marker Class | Examples |
|---|---|
| boot | `POWER_SYNC_BOOT`, `BOOT_PHASE_BEGIN`, `BOOT_PHASE_END` |
| mode | `MODE_SHELL`, `MODE_REALTIME`, `MODE_STATIC`, `MODE_STOP_PREP`, `MODE_STOP_ENTER`, `MODE_WAKE` |
| owner | owner wake, owner idle, owner fault, quiesce begin/ack |
| display | flush requested, full flush begin/end, partial flush begin/end, LPBAM sequence arm/disarm |
| input | input wake, focus delivery, inactivity timeout reset |
| sensor | IMU sample burst, step session update, light sample |
| audio | BBB pattern begin/end, melody begin/end, speaker stream begin/end, amplifier enable/disable |
| communication | NINA wake/sleep, BLE advertise, connected, message TX/RX |
| storage | flash read/write/erase, save commit, package install step |
| USB | VBUS present, USB activity, enumerated, MSC entry, MSC exit |
| sleep/wake | quiesce begin, all owners acked, sleep attempted, wake reason classified, resume complete |

Rules:

- marker names must be stable once used in evidence.
- marker payloads must be bounded.
- high-rate marker classes must be filterable.
- markers must not expose raw pointers, register dumps, protected storage, private package data, or host filesystem paths.

---

## Capture Scenarios

Start with narrow captures. Each run should focus on one behavior.

Baseline scenarios:

| Scenario | Purpose |
|---|---|
| boot to shell idle | baseline startup cost and idle plateau |
| shell idle | steady interactive baseline |
| `STATIC` no activity | low-rate active baseline |
| `STATIC` input-triggered update | input wake plus redraw cost |
| `REALTIME` active loop | frame cadence and CPU/display cost |
| inactivity timeout to low power | forced route back to low power |
| STOP entry and wake | sleep entry cost, STOP current, wake cost |
| display full flush | worst-case display transfer cost |
| display partial flush | normal changed-region transfer cost |
| LPBAM/autonomous display test | autonomous display feasibility, only after LPBAM experiment exists |
| BBB melody | piezo pattern current and timer behavior |
| speaker cue | amplifier and DMA path cost |
| IMU low-power step mode | always-on step counter cost |
| IMU motion stream | high-rate sensor context cost |
| storage save commit | flash write current and duration |
| BLE advertise/connect/message | NINA current behavior and session cost |
| VBUS charger attach | charger/power-bank behavior without MSC prompt |
| USB host enumeration | lightweight data-host detection cost |
| MSC active | heavy USB/storage transport cost |

---

## Interpreting Current Traces

Use operation energy, not just peak current.

For each scenario, record:

- peak current
- steady plateau current
- duration
- average current over the scenario window
- estimated charge cost where practical
- active mode/runtime class
- instrumentation profile

Typical interpretations:

| PPK2 Shape | Likely Cause To Check |
|---|---|
| short sharp spike | display flush, radio packet, wake transition, flash write |
| regular periodic spike | timer wake, sensor poll, BLE interval, RTC wake, display cadence |
| long plateau | peripheral left enabled, owner not idle, NINA awake, audio/display path active |
| current fails to drop after `STOP_PREP` | sleep not entered, pending wake source, unparked peripheral |
| current rises only after input | input path, redraw, audio feedback, sensor burst |
| repeated sawtooth | radio interval, polling loop, charger/fuel gauge behavior |

Every conclusion should identify both:

- the electrical observation from PPK2
- the matching firmware state/event evidence where available

---

## Battery Estimate Pipeline

Game developers should not work with PPK2 traces.

The useful package-facing output is a Platform/Engine estimate derived from measured profiles.

Intended path:

```text
PPK2 current traces
  + Tracealyzer/telemetry correlation
  + measured battery capacity/profile
  + measured runtime-class costs
  -> Platform power model
  -> target-profile budgets and estimates
  -> package validator/runtime compatibility reports
```

Possible future package-facing outputs:

- broad battery state or icon level
- estimated runtime class cost category
- compatibility warning for expensive contexts
- package validation report showing power-relevant declarations

Rules:

- battery estimates are advisory unless a later contract says otherwise.
- estimates must identify the target profile and measured constants used.
- package tools may consume profile limits and estimates.
- package tools must not consume raw PPK2 traces or Platform power internals.

---

## Evidence Requirements

Each power-correlation evidence entry should include:

- evidence ID
- board revision
- firmware commit
- build profile
- target profile if applicable
- Platform knobs hash/version
- active tuning overlay
- instrumentation state
- PPK2 or source-meter model
- PPK2 software version where practical
- sample rate / capture resolution
- source voltage and current limit
- measurement range or calibration notes
- sync strategy: physical pin, logic channel, or timed/cue fallback
- marker list enabled
- Tracealyzer artifact path if used
- telemetry/SWO artifact path if used
- PPK2 raw capture path
- derived plot/export path if used
- conclusion and follow-ups

Use [[Evidence_Artifact_Convention]] for folder and manifest format.

---

## Acceptance Rules

A power measurement is usable bring-up evidence only when:

1. the measurement source and setup are recorded.
2. firmware build/instrumentation profile is recorded.
3. the scenario window is clear.
4. the current trace is preserved or exported.
5. marker/correlation method is described.
6. any promoted constant is updated in [[Pending_Measured_Constants_Register]].
7. the tracker entry distinguishes measured data from interpretation.

Trace-correlated evidence is stronger when:

- the PPK2 trace has a physical sync marker, or
- the firmware event timeline contains a deliberate alignment cue, and
- the trace/telemetry window covers the same scenario.

---

## Rule

Use PPK2 to measure what PeepShow consumed.

Use Tracealyzer and telemetry to explain why.

Promote only measured, reproducible power behavior into Platform policy, target profiles, and package-facing estimates.
