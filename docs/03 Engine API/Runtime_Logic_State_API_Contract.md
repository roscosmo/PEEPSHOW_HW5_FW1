# Runtime Logic and State API Contract

This document defines the package-facing runtime logic model used by PeepOS game-authoring tools, package data, Engine runtime hosts, and the digital twin.

Runtime logic is the layer that turns validated game content into bounded state, event, action, and frame behavior.

Related:

- [[Engine_API_Index]]
- [[Game_Authoring_API_Contract]]
- [[Runtime_Host_Contract]]
- [[Runtime_Host_Internal_State_Machines]]
- [[Package_Contract]]
- [[PeepOS_Capability_Registry]]
- [[Digital_Twin_Host_Runtime_Contract]]
- [[Asset_Pipeline_and_Package_Tooling_Contract]]
- [[Time_And_Power_Intent_API_Contract]]
- [[Input_Focus_API_Contract]]
- [[Rendering_API_Contract]]
- [[Diagnostics_API_Contract]]

---

## Scope

Defines:

- runtime logic primitives exposed to packages and authoring tools
- state graph and action table behavior
- package-visible event model
- runtime unit logic requirements
- validation rules for bounded execution
- digital twin replay expectations

Does not define:

- Platform hardware policy
- RTOS object ownership
- Platform owner-thread internals
- HAL, LL, CubeMX, DMA, interrupt, filesystem, or peripheral behavior
- Reference Game mechanics

---

## Core Principle

Packages do not own threads, RTOS objects, timers, queues, interrupts, or hardware loops.

Packages express:

- states
- events
- guards
- actions
- variables
- bounded ticks
- declared transitions
- lifecycle behavior

The Engine runs those primitives inside runtime hosts.

The Platform owns hardware behavior.

```text
game-dev tools
    |
validated scenes / graphs / modules / scripts
    |
runtime logic tables
    |
Engine runtime host
    |
Platform capability contracts
```

---

## Runtime Logic Model

Runtime logic is organized as:

```text
package
  runtime_units[]
    scenes / modules / graphs
      states / substates
        transitions
          guards
          bounded action lists
```

Authoring tools may present richer editors, hierarchy, visual scripting, dialogue trees, map triggers, pet behavior trees, or scene timelines.

Compiled package output must reduce those forms to bounded PeepOS runtime logic primitives.

Rules:

- every runtime unit has one entry point
- every transition target is declared before package compilation/export
- every action list has a bounded maximum cost
- every event queue, timer table, and variable table has bounded size
- no runtime logic may create threads, tasks, RTOS queues, hardware timers, or direct callbacks
- no runtime logic may call Platform hardware APIs directly

---

## Runtime Classes

Runtime classes define execution and power shape, not game genre.

| Runtime Class | Logic Shape | Expected Use |
|---|---|---|
| `LP_GRAPH` | event, schedule, and state driven | long-running low-power games, clocks, pets, idle toys, Game & Watch logic |
| `LP_MODULE` | Engine-hosted bounded module shape | menus, dialogue, map viewers, inventory, low-power structured modules |
| `RT_SCENE` | frame-paced realtime scene | action scenes, microgames, realtime maps, higher-rate interaction |

`RT_SCENE` is more demanding than graph/module logic. It may request richer per-frame behavior, but it must declare more constraints before validation can accept it.

---

## LP_GRAPH Requirements

`LP_GRAPH` is the preferred primitive for ultra-low-power, long-running packages.

It supports:

- state and substate graphs
- lifecycle events
- input action events
- delayed events
- local-calendar schedule events
- low-rate sensor events
- animation completion events
- audio timeline events where supported
- save/settings completion events
- bounded action tables
- declared transitions to other runtime units

Rules:

- no polling loop
- no high-rate sensor stream
- no active communication receive dependency on HW5 profiles
- no frame-paced realtime update requirement
- all timers and schedules must tolerate cadence clamp, missed wake, and bounded catch-up
- idle behavior must route to `HOLD`, `ULP_ANIM`, `STATIC`, another approved low-power unit, or shell exit

`LP_GRAPH` should be powerful enough for complete games. Its restriction is power and boundedness, not style or genre.

---

## LP_MODULE Requirements

`LP_MODULE` is an Engine-hosted low-power module with a predefined bounded loop shape.

Examples:

- dialogue module
- menu module
- map inspection module
- turn-based encounter module
- inventory/status module
- clock or schedule module

Rules:

- `module_type` must be approved by the Engine contract
- module config must be validated before package compilation/export
- host-defined update cadence and action limits apply
- module must declare idle behavior and suspend/resume behavior
- module must declare allowed runtime-unit transitions
- module may not contain arbitrary unbounded code

`LP_MODULE` exists to make common structured gameplay easier without exposing RTOS or hardware control.

---

## RT_SCENE Requirements

`RT_SCENE` is the package-facing primitive for active frame-paced scenes.

It may use:

- frame tick
- active input focus
- realtime render commands
- bounded animation updates
- bounded high-rate sensor contexts where target profile grants them
- active audio contexts
- communication contexts where target profile grants them
- scene-local transient variables

Required declarations:

- target frame rate
- frame budget
- maximum update cost
- maximum render command count
- maximum event processing cost per frame
- asset preparation/preload requirements
- input focus scope
- sensor/audio/communication contexts
- idle detection rule
- suspend behavior
- resume behavior
- fallback runtime unit
- power/cadence intent

Rules:

- Platform inactivity timeout always applies
- realtime work must stop, suspend, or transition when no meaningful activity remains
- frame logic must not block on storage, communication, save writes, or hardware completion
- overruns must be observable through diagnostics where the active profile allows
- `RT_SCENE` must return to a declared low-power route unless the selected target profile explicitly allows another route

`RT_SCENE` has no fixed maximum active duration at this contract level. It may remain active while meaningful user activity or Platform-approved active work continues.

---

## State And Substate Model

Authoring tools may expose nested states, substates, and grouped graph regions.

Compiled runtime data must provide:

```text
state_graph:
  graph_id
  entry_node
  states[]
  transitions[]
  timers[]
  local_variables[]
  action_tables[]
  bounds
```

Rules:

- every graph has one entry node
- every state ID is stable within the graph
- every transition target exists
- entry/exit actions are bounded
- transition actions are bounded
- hierarchical authoring must compile to deterministic runtime tables
- parallel state regions are allowed only if their scheduling and action cost are statically bounded
- graph-local variables must declare type, size, reset behavior, and persistence behavior

---

## Event Model

Package-visible events are symbolic Engine events.

Allowed event classes:

| Event Class | Source Contract |
|---|---|
| lifecycle | runtime host mount/start/suspend/resume/stop/unmount |
| input action | [[Input_Focus_API_Contract]] |
| delayed timer | [[Time_And_Power_Intent_API_Contract]] |
| local calendar schedule | [[Time_And_Power_Intent_API_Contract]] |
| wake/resume reason | [[Time_And_Power_Intent_API_Contract]] |
| render/animation completion | [[Rendering_API_Contract]] |
| audio timeline marker | [[Audio_API_Contract]] |
| sensor event/snapshot availability | [[Sensor_API_Contract]] |
| communication session/message | [[Communication_API_Contract]] |
| save/settings completion | [[Package_Save_Settings_API_Contract]] |
| package diagnostic/fault routing | [[Diagnostics_API_Contract]] |

Rules:

- event payloads are fixed-schema and bounded
- event queue depth is bounded by runtime class and target profile
- event dispatch order must be deterministic for a fixed input trace
- overflow behavior must be declared and validated
- hardware faults are not ordinary gameplay events
- required Platform primitive failure routes through Engine lifecycle and Platform diagnostics

---

## Guards And Expressions

Guards are bounded expressions used to select transitions and actions.

Allowed inputs:

- graph variables
- package settings
- save-backed values read through schema
- event payload fields
- resolved sensor values
- local calendar/logical time
- capability state exposed by Engine contracts
- deterministic random source where a package seed policy is declared

Rules:

- expression cost is statically bounded
- types are explicit
- numeric ranges are explicit
- no recursion
- no unbounded loops
- no dynamic code loading
- no direct memory access
- no direct calls to Platform, HAL, RTOS, filesystem, or middleware APIs

---

## Actions

Runtime actions are symbolic requests to Engine contracts.

Allowed action categories:

- set or clear graph variable
- transition state
- push, pop, or replace runtime unit through declared edges
- request draw/update through [[Rendering_API_Contract]]
- request input focus change through [[Input_Focus_API_Contract]]
- request audio cue or BBB pattern through [[Audio_API_Contract]]
- request save/settings read or write through [[Package_Save_Settings_API_Contract]]
- request delayed event, calendar schedule, cadence, or power intent through [[Time_And_Power_Intent_API_Contract]]
- request or release sensor context through [[Sensor_API_Contract]]
- request communication session/message behavior through [[Communication_API_Contract]]
- emit package diagnostics through [[Diagnostics_API_Contract]]
- raise package fault code

Rules:

- actions are bounded and non-blocking
- actions return package-visible completion events where the called contract defines them
- action tables must not contain hardware commands
- action tables must not contain host filesystem paths
- action tables must not contain function pointers or raw memory pointers
- action tables must not spin, sleep, busy-wait, retry forever, or block on I/O

---

## Variables And Persistence

Runtime logic may use several variable classes.

| Variable Class | Purpose | Durability |
|---|---|---|
| transient | current state/event calculation | lost on stop/unmount |
| unit-local | runtime unit state | survives within mounted unit lifecycle |
| fast-resume | small STOP-resume state where profile supports it | retained only across supported low-power resume |
| save-backed | durable package state | persisted through save schema |
| package setting | package-owned user preference | persisted through save/settings schema |

Rules:

- all variables have declared type, size, and bounds
- save-backed variables must map to [[Package_Save_Settings_API_Contract]]
- fast-resume state is not durable storage
- retained snapshots must be versioned and integrity checked where used
- packages must tolerate fast-resume loss by restoring from durable save/default state
- variables may not contain raw pointers, host paths, hardware addresses, or private struct layouts

---

## Tick And Scheduling Semantics

Runtime logic receives work through events and approved ticks.

| Runtime Class | Tick Semantics |
|---|---|
| `LP_GRAPH` | no free-running tick; event/schedule/wake driven |
| `LP_MODULE` | host-defined bounded update points |
| `RT_SCENE` | frame tick while realtime activity remains valid |

Rules:

- low-power logic must not emulate a loop with high-frequency delayed events
- repeated timers must declare maximum cadence and catch-up behavior
- `RT_SCENE` frame delta comes from Engine time model, not hardware timer registers
- missed ticks must be handled through bounded catch-up or discard policy
- Platform may clamp, coalesce, delay, or suppress work according to power policy

---

## Runtime Unit Transitions

Runtime unit transitions use the package model defined in [[Package_Contract]].

Allowed transition forms:

```text
transition_to(unit_id)
push_unit(unit_id)
pop_unit()
exit_to_shell(reason)
```

Rules:

- transition targets must be declared
- transition stack depth is bounded
- recursive push loops are invalid unless statically bounded and approved by validation
- transition actions are bounded
- active contexts must be released, suspended, or transferred according to their contracts
- input focus must be released or transferred during transition
- realtime units must declare fallback routing before validation accepts them

---

## Fault And Failure Semantics

Package logic failures are separate from Platform hardware failures.

Package logic failures include:

- invalid transition target reached through corrupted package data
- action budget exceeded
- frame budget exceeded
- missing required asset after validation
- unhandled runtime unit failure
- package-declared fault code

Platform hardware failures include:

- sensor owner fault
- display owner fault
- storage owner fault
- audio owner fault
- communication owner fault
- wake/power fault

Rules:

- package logic faults route through Engine lifecycle policy and package diagnostics
- Platform hardware faults route through Platform diagnostics and lifecycle policy
- required HW5 primitives are assumed available after validation; if they fail, that is not ordinary game logic
- normal package tools should report failures in PeepOS authoring language

---

## Digital Twin Requirements

The digital twin must execute or faithfully mirror the same runtime logic contract.

Required behavior:

- same state graph data
- same action tables
- same runtime unit transitions
- same event ordering for a fixed trace
- same cadence clamp behavior from the selected target profile
- same save/schema behavior
- same diagnostics behavior for a fixed trace
- deterministic replay support

The twin may expose richer inspection tools, but those tools must not become package runtime APIs.

Digital twin evidence can validate runtime logic and package behavior. It cannot validate HW5 electrical, timing, current, peripheral, or storage-media behavior.

---

## Validation Requirements

Tooling must validate runtime logic before package compilation/export.

Required checks:

- entry point exists
- every state/transition/action reference resolves
- runtime unit declarations are valid
- runtime class requirements are satisfied
- action table length and cost are bounded
- expression cost is bounded
- timer cadence and catch-up policy are valid
- variable size and persistence class are valid
- event queue bounds are valid
- capability use is declared
- power compliance is satisfied
- `RT_SCENE` frame budget, idle detection, suspend/resume behavior, and fallback unit are declared
- asset, save, input, sensor, audio, communication, time, power, and diagnostics references resolve
- generated logic contains no hardware, RTOS, filesystem, Platform-internal, or host-path references

Validation failures that affect runtime safety, determinism, storage integrity, or power policy block package compilation/export in every profile.

---

## Forbidden Runtime Logic Constructs

Normal package runtime logic must not contain:

- unbounded loops
- unbounded recursion
- dynamic code loading
- package-created threads
- RTOS object creation
- blocking waits without timeout
- busy-wait loops
- hardware interrupt handlers
- raw filesystem paths
- raw pointers or function pointers
- hardware registers
- HAL, LL, CubeMX, ThreadX, FileX, LevelX, USBX, or Platform-internal API references

Any generated artifact containing these constructs must fail internal safety verification.

---

## Validation Cases

1. valid `LP_GRAPH` state graph validates and runs from entry node.
2. missing entry state fails package validation.
3. transition to undeclared state fails package validation.
4. transition to undeclared runtime unit fails package validation.
5. unbounded action loop fails validation in every build profile.
6. `LP_GRAPH` high-frequency polling timer fails validation.
7. `LP_GRAPH` with bounded calendar schedule and catch-up policy validates.
8. `LP_MODULE` without approved `module_type` fails validation.
9. `RT_SCENE` without frame budget fails validation.
10. `RT_SCENE` without idle fallback fails validation.
11. `RT_SCENE` frame overrun emits diagnostics where profile allows and follows lifecycle policy.
12. suspend/resume during active runtime logic preserves or reconstructs package state according to declared persistence classes.
13. package logic cannot receive hardware owner faults as normal gameplay branches.
14. digital twin replay of a fixed input/time/sensor trace produces identical state and diagnostics output.

---

## Rule

PeepOS runtime logic is event, state, action, and frame behavior.

It is not RTOS ownership, hardware control, or Platform policy.
