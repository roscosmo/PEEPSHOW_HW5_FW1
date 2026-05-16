# Time And Power Intent API Contract

This document defines the Engine-facing time, cadence, lifecycle, wake, and power-intent API for PeepOS packages and game-development tools.

The API exposes logical time and intent. It does not expose RTC registers, SysTick, hardware timers, clock trees, STOP modes, wake-pin configuration, PMIC policy, LPBAM setup, DMA behavior, or RTOS scheduler internals.

Related:

- [[Game_Authoring_API_Contract]]
- [[PeepOS_Capability_Registry]]
- [[Package_Contract]]
- [[Runtime_Host_Contract]]
- [[Digital_Twin_Host_Runtime_Contract]]
- [[Power_and_Sleep_Policy]]
- [[PMIC_and_Power_Contract]]
- [[Brought_Up_Tracker]]

---

## Purpose

Games own logical time and state progression.

PeepOS owns physical timebases, RTC setup, wake scheduling, sleep entry, clock policy, and inactivity enforcement.

Package-facing time exists so packages can implement:

- virtual pet cycles
- day/night behavior
- real-world daily schedules
- cooldowns
- long-running idle progression
- low-power state machines
- realtime scenes
- deterministic digital twin replay

Packages may read PeepOS calendar time and schedule against it. Packages may not set, correct, resync, or directly access RTC hardware.

---

## Ownership Boundary

The Platform owns:

- RTC hardware
- date/time setup UI
- time validity
- time correction and invalid-time recovery
- drift/resync policy
- local-time policy
- sleep class and clock transitions
- wake-source arming
- inactivity timeout
- quiesce/resume sequencing
- PMIC and shipping-mode policy

The Engine owns:

- package-facing logical time APIs
- runtime lifecycle event delivery
- schedule and delayed-event admission
- runtime-unit cadence validation
- power-intent validation
- digital twin time model contract
- package-visible wake reason normalization

Packages own:

- gameplay state progression
- reactions to local calendar time
- elapsed-time reconciliation after suspend/resume
- scheduled package events
- activity/idle intent hints
- declared low-power fallback routing

Packages do not own physical time or power policy.

---

## Core Rules

- Calendar time is a normal package-facing primitive on target profiles that grant `time.calendar`.
- Packages may read local PeepOS date/time and schedule package events against it.
- Packages may not set or correct system time.
- Packages may not program RTC, SysTick, hardware timers, clocks, STOP mode, or wake pins.
- Packages may request logical schedules and cadence only.
- Platform may clamp, coalesce, delay, or skip low-power timing work according to policy.
- User inactivity timeout always applies to package runtime.
- `HOLD` and `ULP_ANIM` run no arbitrary package logic.
- Realtime execution must declare idle detection and fallback routing.
- Runtime lifecycle events are the normal path for sleep/resume reconciliation.

---

## Calendar Time

PeepOS local calendar time is system-owned and package-readable.

Package-facing values:

```text
time.now_local()
time.date_local()
time.time_of_day_local()
time.weekday_local()
time.day_phase()
time.calendar_valid()
```

Examples of game use:

- morning/day/night behavior
- daily care routines
- birthday or anniversary events
- "come back tomorrow" mechanics
- shop/item rotations
- real-world cooldowns

Rules:

- normal package runtime may assume calendar time is valid on profiles that grant `time.calendar`.
- first setup should establish date/time before normal package runtime where `time.calendar` is part of the baseline profile.
- if calendar time becomes invalid later, PeepOS handles recovery through system lifecycle/setup/diagnostic flow.
- package code must not present its own RTC-setting path except through approved system UI requests.
- package save data that stores timestamps must treat them as PeepOS local calendar values with explicit schema versioning.

`day_phase` is a PeepOS helper derived from local time and system policy. Exact bands should be profile/system configurable, not hardcoded to a specific game.

---

## Elapsed Time

Packages also receive logical elapsed time.

Package-facing values:

```text
time.runtime_elapsed_ms()
time.frame_dt_ms()
time.elapsed_since(timestamp_or_saved_time)
resume_event.elapsed_suspended_ms
resume_event.elapsed_calendar_ms
```

Rules:

- `runtime_elapsed_ms` advances while a runtime unit is active.
- `frame_dt_ms` is supplied by the realtime host, not hardware timers.
- elapsed suspended time is delivered through lifecycle/resume context.
- low-power packages should reconcile state from elapsed time instead of running continuously.
- elapsed time used for deterministic tests must be controllable by the digital twin.

---

## Scheduling

Packages may schedule logical events.

Package-facing operations:

```text
time.schedule_after(duration_ms, event_id)
time.schedule_at_local(local_datetime_or_rule, event_id)
time.cancel_schedule(event_id)
time.next_scheduled_event()
```

Schedule rule examples:

```text
after 10 seconds
after 30 minutes
at 07:00 local time
daily at 18:00 local time
next local date boundary
```

Rules:

- scheduled events are package events, not direct RTC alarms.
- Platform/Engine maps schedules onto safe wake/cadence behavior.
- low-power schedules may be coalesced or delayed according to target profile.
- tools must reject unbounded schedule tables.
- high-frequency schedules must be valid for the runtime class and target profile.
- package schedules must survive suspend/resume through explicit package state or Engine schedule state.
- missed events after long sleep are delivered according to declared catch-up policy.

Catch-up policy examples:

| Policy | Meaning |
|---|---|
| `latest_only` | deliver only the latest due event |
| `count_elapsed` | deliver one event with elapsed occurrence count |
| `bounded_replay` | replay up to a declared maximum |
| `drop_if_stale` | discard stale event and continue |

Unbounded catch-up is invalid.

---

## Cadence Intent

Packages declare cadence intent; Platform chooses physical timing.

Package-facing cadence intents:

| Intent | Meaning |
|---|---|
| `static_update_max_hz` | preferred maximum low-cadence active update rate |
| `low_power_tick_period_ms` | desired low-power package event cadence |
| `realtime_target_fps` | desired frame cadence for realtime scene |
| `deadline_class` | latency class for package-visible work |

Rules:

- cadence intent must be tied to a runtime unit.
- `RT_SCENE` frame pacing is valid only while realtime activity is valid.
- `LP_GRAPH` should be event/schedule driven, not polling driven.
- `STATIC` updates are low-rate and event driven.
- input-triggered `STATIC` updates may be serviced promptly and then return to idle policy.
- Platform inactivity timeout can force fallback regardless of requested cadence.

---

## Lifecycle Events

Packages receive lifecycle events through the runtime host.

Package-facing lifecycle events:

```text
on_mount(context)
on_start(context)
on_suspend(reason)
on_resume(resume_context)
on_stop(reason)
on_unmount()
```

Resume context:

```text
resume_context:
  wake_reason
  elapsed_suspended_ms
  elapsed_calendar_ms
  calendar_valid
  power_context
  missed_schedule_summary
```

Rules:

- lifecycle delivery is ordered and bounded.
- `on_suspend` must not block indefinitely.
- `on_resume` is where low-power packages reconcile elapsed time and wake reason.
- package code must not assume it ran while suspended.
- resume failure routes through Engine lifecycle policy, not partial runtime state.

---

## Wake Reason

Packages receive normalized wake reasons where relevant.

Package-facing wake reasons:

| Wake Reason | Meaning |
|---|---|
| `input` | button, encoder, joystick, or focus-delivered input wake |
| `schedule` | package/system schedule or RTC cadence wake |
| `sensor` | approved sensor event wake |
| `power` | PMIC, charger, battery, or shipping/power event |
| `usb` | USB attach/detach or installer/system event |
| `fault_recovery` | watchdog or recovery path |
| `system` | shell/system transition |
| `unknown` | Platform defect path; not normal gameplay |

Rules:

- raw EXTI, RTC alarm IDs, PMIC registers, and wake pins are not package-visible.
- unknown wake reasons are Platform defects until explained.
- HW5 communication cannot be a wake reason for packages.
- wake input is delivered through lifecycle/resume before normal package action delivery where required by input policy.

---

## Activity And Idle Intent

Packages may publish activity state.

Package-facing operations:

```text
power.mark_active(reason)
power.mark_idle()
power.low_power_ready(route_id)
power.realtime_work_pending(reason)
```

Rules:

- activity hints are not exemptions from user inactivity timeout.
- packages must declare low-power fallback route from high-duty runtime units.
- `RT_SCENE` must declare idle detection, suspend behavior, resume behavior, and fallback routing.
- Platform may ignore, expire, clamp, or override activity hints.
- packages must not keep hardware awake directly.

---

## Runtime Class Rules

| Runtime Class | Time And Power Behavior |
|---|---|
| `LP_GRAPH` | event, schedule, input, and low-power cadence driven; no polling loop |
| `LP_MODULE` | bounded awake windows with declared cadence/context behavior |
| `RT_SCENE` | frame-paced active loop with budget reporting and idle fallback |

`HOLD`:

- no package logic runs.
- last display frame remains visible.
- approved wake/schedule policy remains armed.

`ULP_ANIM`:

- no arbitrary package logic runs.
- only prevalidated low-power sequence behavior may continue where target profile grants it.

---

## Tool-Time Validation

Tooling must validate time and power intent before package compilation/export.

Reject:

- direct RTC, SysTick, hardware timer, STOP, clock, PMIC, or wake-pin references.
- unbounded schedule tables.
- unbounded catch-up.
- polling loops used to approximate low-power cadence.
- high-frequency schedules in `LP_GRAPH`.
- realtime runtime unit without idle fallback.
- runtime unit that requests cadence above target profile caps.
- package wake intent unsupported by selected target profile.
- communication wake intent on HW5 profiles.
- package requiring `time.calendar` on a target profile that does not grant it.
- package timestamp fields without schema type/version where persisted.

Authoring tools should explain failures in PeepOS terms, such as:

```text
This low-power graph requests a 20 Hz timer. Use RT_SCENE or lower the cadence.
```

They should not expose RTC, STOP, clock, timer, or wake-pin implementation details to normal game authors.

---

## Digital Twin Requirements

The digital twin must use the same time and power-intent contract as the hardware runtime.

Required twin time models:

- deterministic fixed-step
- interactive wall-clock

Optional twin time models:

- accelerated low-power idle simulation
- single-step event evaluation
- recorded timeline replay

Rules:

- calendar time must be controllable in deterministic tests.
- scheduled events, lifecycle events, wake reasons, cadence clamps, and inactivity timeout must be replayable.
- twin profiles must derive cadence caps and inactivity timeout from measured/frozen target profiles.
- twin evidence does not prove RTC hardware, wake latency, current draw, or physical sleep behavior.

---

## Validation Cases

1. package can read valid PeepOS local calendar time without RTC access.
2. package cannot set, correct, resync, or directly program RTC.
3. first-setup or recovery flow establishes valid system time before launching calendar-dependent packages.
4. `schedule_after` and `schedule_at_local` produce bounded package events.
5. long sleep resumes package with elapsed suspended/calendar time and bounded missed-event summary.
6. unbounded catch-up policy fails validation.
7. `LP_GRAPH` polling cadence above target profile limits fails validation.
8. `RT_SCENE` without idle fallback fails validation.
9. user inactivity timeout forces declared low-power route despite active cadence request.
10. HW5 communication wake intent fails validation.
11. digital twin deterministic replay produces the same time, schedule, wake, and lifecycle event sequence for a fixed trace.
12. digital twin accelerated sleep simulation is not used as HW5 current, wake-latency, or RTC hardware evidence.
