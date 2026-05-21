# Power and Sleep Policy

This document defines how the PeepShow Platform translates Engine and runtime intent into power behavior.

The package-facing time, cadence, lifecycle, wake, and power-intent API is defined in [[Time_And_Power_Intent_API_Contract]].

---

## Core Policy

- STOP-first strategy for low-activity operation.
- Power thread is sole owner of clock and sleep transitions.
- Runtime hosts provide intent, not hardware commands.
- No transition is complete until timebases are verified.
- PMIC, battery, charger, VBUS, and shipping-mode policy is owned by [[PMIC_and_Power_Contract]].

---

## Package Intent Boundary

Packages and runtime hosts may publish:

- runtime unit and runtime class
- activity and idle intent
- requested cadence
- latency tolerance
- required wake intents
- temporary capability context declarations
- declared low-power fallback route

Packages and runtime hosts must not publish or control:

- STOP level
- clock profile
- RTC programming
- LPBAM setup
- DMA/display transfer method
- peripheral power state
- PMIC policy
- wake-source electrical configuration

`thPower` may grant, clamp, delay, reject, shorten, or revoke any runtime request when Platform policy, battery state, sleep policy, or validated target profile requires it.

---

## Wake Reason Classification

All wakes must be classified as one of:
- RTC cadence
- button or joystick input
- sensor event
- PMIC interrupt
- USB attach/detach
- watchdog or fault recovery
- unknown (requires investigation)

Unknown wake reasons are defects until explained.

---

## Intent To Policy Mapping

Inputs to power manager:
- runtime class
- requested cadence
- allowed latency
- required wake sources
- active audio/display flags

Outputs from power manager:
- sleep class
- clock profile
- sensor mode
- input mode
- display transfer mode
- battery/charger policy action

When [[IMU_Contract|IMU]] step counting is active, power policy must select the deepest sleep class that preserves the IMU embedded functions and interrupt path. Absolute deepest sleep is invalid if it stops step counting or loses step-counter state.

---

## Runtime Policy Matrix

`SHELL`
- default low-power interactive policy
- STOP2 eligible when idle

`LP_GRAPH`
- RTC and event-driven
- maximize STOP residency

`LP_MODULE`
- low-power preferred, bounded awake windows

`RT_SCENE`
- frame paced active mode with explicit idle fallback

`INSTALLER`
- USB/staging ownership mode with strict subsystem isolation

---

## Game-Facing Power Concepts

The Engine exposes power behavior as authoring concepts, not hardware states.

| Concept | Platform Meaning |
|---|---|
| `HOLD` | last display frame remains visible; no package logic runs; approved wake sources and cadence policy remain armed |
| `ULP_ANIM` | optional prevalidated autonomous low-power display sequence; available only when the target profile grants it |
| `STATIC` | low-cadence event/state-machine behavior; input-triggered updates may be immediate where policy allows |
| `REALTIME` | frame-paced active behavior with declared idle detection and fallback |

The Platform maps these concepts onto actual clock, sleep, display, input, and sensor policy.

Package-visible concepts must not expose `STOP2`, `STOP3`, PLL settings, LPDMA, LPBAM, EXTCOMIN, PMIC registers, or wake pin configuration.

---

## Inactivity And Cadence Policy

User inactivity timeout is mandatory for game/runtime packages.

After the timeout expires, Platform policy may force the active runtime unit toward its declared low-power route even if the package requested realtime cadence or temporary capability contexts.

An admitted interactive communication context may receive bounded peer-wait grace where the target profile grants it. This exists for cases such as turn-based remote play where the local device is watching for the next meaningful peer action. It delays the forced low-power route only within Platform-owned limits; it is not a package-owned timeout override, unlimited active-session lease, or communication wake source.

Cadence requests are requests only:

- `REALTIME` frame cadence is granted only while realtime activity is valid.
- `STATIC` cadence is low-rate and event driven.
- input-triggered `STATIC` updates may be serviced promptly, then returned to idle policy.
- `HOLD` runs no package logic.
- `ULP_ANIM` runs no arbitrary package logic and is profile-gated.

Target profiles must define:

- enforced inactivity timeout
- required forced-idle low-power route policy
- static periodic update cadence cap
- static input-response latency cap
- low-power periodic update cadence cap
- whether low-power updates require MCU wake/update/return behavior
- realtime frame budget
- realtime target frame rate
- autonomous display sequence availability
- autonomous sequence frame/cadence caps, if available
- interactive session peer-wait support, grace cap, refresh policy, and expiry-route requirement where available
- package-visible wake intents and lifecycle wake reasons

Current design expectations are an inactivity timeout in the 10-15 second range, static updates around 2 Hz, baseline low-power display updates around 1 Hz when each update wakes the MCU, and a 30 fps realtime target. These are design targets only until HW5 evidence freezes target profile values.

Profile-dependent STOP display behavior:

| Target Profile | STOP Display Behavior |
|---|---|
| `HW5_VALIDATED_BASELINE` | display updates from low power require wake/update/return behavior and are capped by measured Platform policy |
| `HW5_VALIDATED_LPBAM` | prevalidated autonomous frame sequences may run without arbitrary package logic, capped by measured Platform policy |

---

## Transition Sequence (Required)

1. collect quiesce acknowledgements from owners
2. apply profile and sleep transition
3. validate HAL and RTOS timebases
4. resume owners and validate liveness
5. publish mode transition complete event

All waits in this sequence must have explicit timeout.

---

## Forbidden Patterns

- package code requesting direct STOP entry
- clock profile writes outside power owner
- polling loops used instead of wake events
- entering installer mode with active non-USB storage users

---

## Required Measurements

For each runtime class measure:
- average current in steady state
- wake latency
- resume latency
- failed wake/resume count

Track these in [[Power_Validation]].
