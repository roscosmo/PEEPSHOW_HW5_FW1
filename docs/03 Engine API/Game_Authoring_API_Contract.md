# Game Authoring API Contract

This document defines the Engine-facing contract for game-authoring tools, package content, and runtime-safe game behavior.

Game-authoring tools target this contract and the package contracts. They must not target Platform hardware abstractions, RTOS internals, CubeMX output, STM32 HAL/LL, middleware internals, or Reference Game-specific implementation details.

Editable project/source files are governed by [[Authoring_Project_Schema_Contract]]. Compiled package/runtime output remains governed by this contract, [[Runtime_Logic_State_API_Contract]], and the package/tooling contracts.

Related:

- [[Engine_API_Index]]
- [[Runtime_Host_Contract]]
- [[Runtime_Host_Internal_State_Machines]]
- [[Runtime_Logic_State_API_Contract]]
- [[Authoring_Project_Schema_Contract]]
- [[Digital_Twin_Host_Runtime_Contract]]
- [[PeepOS_Capability_Registry]]
- [[Target_Profile_Schema_Contract]]
- [[Content_Parameter_Schema_Contract]]
- [[Package_Contract]]
- [[Package_Manager_State_Machine]]
- [[Asset_Pipeline_and_Package_Tooling_Contract]]
- [[Authority_and_Invariants]]
- [[Architecture_and_Boundaries]]
- [[Game_Documentation_Boundary]]

---

## Scope

Defines:

- the game/tool-facing authoring surface
- the runtime-safe Engine request surface available to hosted games
- package validation requirements before compilation/export
- required capability declaration model
- safe hooks for rendering, input, audio, assets, saves, time, sensors, communication, power intent, and diagnostics
- safe hooks for runtime logic, state graphs, action tables, and realtime scene logic

Does not define:

- Platform hardware policy
- HAL, LL, CubeMX, DMA, pin, clock, storage-owner, filesystem, or RTOS details
- Reference Game mechanics or content
- low-level asset file formats beyond Engine-visible requirements
- Platform owner-thread state machines

---

## Layer Position

```text
Game-authoring tools
        |
validated authoring data and package sources
        |
package compiler / asset pipeline
        |
validated package
        |
Engine runtime hosts
        |
Platform capability APIs
        |
Platform owner threads and hardware policy
```

The authoring layer expresses content and intent.

The Engine validates and runs reusable game abstractions.

The Platform decides hardware behavior.

---

## Core Rule

Game-authoring tools must validate all content before package compilation or export.

No tool may emit a compiled package, generated runtime module, asset blob, state table, or installable artifact unless the validation gate passes.

Install-time validation remains mandatory, but it is a second gate. It is not a substitute for tool-side validation.

Game developers work inside the PeepOS sandbox. Low-level concepts such as HAL, GPIO, DMA, RTOS objects, buses, registers, filesystems, and Platform internals should be unrepresentable in normal game tools.

---

## General-Purpose Runtime Principle

PeepOS authoring APIs are genre-agnostic.

The API exposes reusable low-power handheld runtime primitives, not Reference Game mechanics and not a pet-game-specific API.

Valid PeepOS content may include, but is not limited to:

- virtual pets
- clocks and ambient toys
- Game & Watch style games
- puzzle games
- turn-based RPGs
- text adventures
- visual novels
- card or board games
- step/light/motion-reactive games
- multiplayer experiments
- music and BBB toys
- utility or diagnostic-style packages where allowed by package policy

Runtime classes describe execution and power behavior, not genre.

The Reference Game is a proof-of-capability package and showcase. It must be built from the same public PeepOS primitives available to other packages.

Reference Game needs may request new primitives. Accepted primitives must be reusable beyond the Reference Game and documented as Engine/API capabilities before game implementation depends on them.

No Reference Game-only hidden API path is allowed.

Tool UX may include templates for pets, maps, microgames, clocks, dialogue, or other patterns. Templates must compile down to the same general PeepOS primitives and package schemas available to all packages.

---

## Tooling Boundary

PeepOS game tools may expose and produce:

- manifests
- asset tables
- scene graphs
- state graphs
- animation tables
- input action maps
- audio cue tables
- BBB pattern tables
- save schemas
- content parameter schemas
- capability declarations
- wake-intent declarations
- cadence hints
- localized text tables
- bounded script or action tables where a runtime host explicitly allows them

PeepOS game tools must not expose:

- hardware pins, ports, buses, registers, interrupts, DMA, clocks, or sleep modes
- STM32 HAL/LL, CubeMX, ThreadX, FileX, LevelX, USBX, or Platform-internal names
- Platform knobs or `platform.knobs.*` paths as editable package controls
- raw filesystem paths for runtime use
- direct storage-region addresses
- raw memory pointers or function pointers
- host-specific private struct layouts
- unbounded script constructs

The package compiler and internal verifier must also ensure generated artifacts do not contain:

- direct HAL, LL, RTOS, middleware, or Platform-internal API references
- raw filesystem paths for runtime use
- direct storage-region addresses
- raw peripheral register writes
- Platform knob writes or generated Platform knob references
- CubeMX pin, DMA, or clock assumptions
- unbounded scripts
- host-specific private struct layouts unless defined by a versioned package schema

---

## Validation Gate

The toolchain validation gate runs before package compilation/export.

Validation has four layers.

### Authoring Validation

Authoring validation is user-facing and must speak in PeepOS concepts only.

It checks:

- scene entry point exists
- scene/state graph is structurally valid
- transition targets exist
- action lists and expressions are bounded
- asset references resolve or have approved placeholders
- selected runtime class supports the authored features
- declared capabilities match authored feature use
- input action map is valid
- audio cue and BBB pattern bounds are valid
- save fields have a schema and version
- cadence, wake intent, sensor use, and communication use are within declared limits
- required fallbacks exist for unavailable optional capabilities

Validation failures block compilation/export.

Warnings that affect runtime safety, determinism, storage integrity, power policy, or capability availability must become errors for normal packages.

### Compiler Validation

Compiler validation checks deterministic package generation:

- manifest schema validity
- asset format and bounds
- stable generated table ordering
- checksums and integrity metadata
- package format version
- runtime class compatibility
- compatibility report generation

Compiler validation failures block compiled output.

### Internal Safety Verification

Internal safety verification is a hidden guardrail for toolchain bugs, corrupted artifacts, malicious packages, and future advanced tooling.

It checks generated output for forbidden internals such as HAL, LL, RTOS, filesystem, Platform hardware abstraction, raw register, and storage-region references.

Normal PeepOS game developers should not see these names in routine authoring errors. If a normal authoring flow produces an internal-forbidden-token error, treat that as a toolchain defect or corrupted artifact.

### Firmware Install Validation

Firmware install validation runs on-device before install/activation.

It verifies:

- package integrity
- schema compatibility
- runtime class compatibility
- required capability compatibility
- asset table integrity
- save schema declaration
- install transaction safety

Authoring validation output must include exact rejection reasons that can be surfaced to tool users in PeepOS terms.

Internal verifier output may include low-level detail for toolchain developers, but it should not be the normal author-facing explanation.

---

## Validation Severity

Validation results must use severity levels.

| Severity | Meaning | Blocks Authoring Preview | Blocks Dev Package | Blocks Shipping Package |
|---|---|---:|---:|---:|
| `fatal` | package/source cannot be parsed or is structurally incoherent | Yes | Yes | Yes |
| `error` | runtime safety, schema, bounds, or required capability violation | Yes unless mocked by profile | Yes | Yes |
| `warning` | incomplete or suspicious but runtime-safe | No | No unless profile escalates | Yes unless waived |
| `advisory` | polish, optimization, style, or unused content | No | No | No |
| `waived` | known warning accepted under an explicit waiver | No | No | No if release policy allows the waiver |

Developer-facing errors must use PeepOS authoring language.

Examples:

| Condition | User-Facing Severity |
|---|---|
| missing entry scene | `fatal` or `error` |
| unresolved required sprite without placeholder policy | `error` |
| placeholder art | `advisory` |
| missing optional SFX with fallback | `warning` or `advisory` |
| save variable without schema | `error` |
| action graph may loop without a bounded exit or tick budget | `error` |
| unused asset | `advisory` |
| optional sensor feature has no content fallback | `error` |

---

## Build Profiles

Validation policy is profile-aware.

| Profile | Purpose | Validation Behavior |
|---|---|---|
| `authoring_preview` | editor preview, simulator, and quick iteration | allows placeholders, mocks, and warnings; blocks incoherent graphs and unbounded behavior |
| `dev_package` | test package on device or runtime host | allows warnings and explicit waivers; blocks runtime safety errors |
| `hardware_bringup` | Platform-owned hardware testing | outside normal game package path; must not be treated as game content |
| `release_candidate` | final compatibility and polish pass | warnings must be resolved or explicitly waived |
| `shipping` | normal user-installable package | no unresolved errors, no unresolved safety warnings, no unknown schema issues |

Dev and preview profiles must be productive. They may run unfinished but safe content.

No profile may allow unbounded runtime behavior, missing required entry points, invalid save schema, unknown runtime class, or package integrity failure.

Simulator and digital-twin preview modes use the same PeepOS authoring hooks. The host digital twin is implemented only after HW5 Platform behavior is validated and documented; before that point, simulator mocks are development conveniences and must not be treated as hardware evidence.

---

## Waivers

Waivers are allowed only for issues that are runtime-safe.

Rules:

- waivers must record issue ID or stable validation code
- waivers must record reason, author, date, and intended removal condition
- waivers may not suppress `fatal` findings
- waivers may not suppress hard runtime safety errors
- shipping waiver policy must be explicit per package profile
- waiver state is included in compatibility reports

Valid waiver examples:

- placeholder art accepted for a dev package
- optional SFX missing while silent cue fallback is declared
- temporary text warning accepted for a release candidate

Invalid waiver examples:

- no entry scene
- unbounded action graph
- save write without schema
- package integrity failure
- required capability unavailable with no fallback

---

## Placeholder And Mock Policy

Authoring tools may support placeholders and mock capabilities for preview and dev workflows.

Allowed:

- placeholder sprites
- placeholder text
- silent replacement for missing optional SFX
- simulator-only mock light, step, motion, or communication events
- dummy save records in simulator preview

Rules:

- placeholders must be explicit in the validation report
- required runtime assets need either real content or an approved placeholder policy
- mock capabilities are not proof of hardware support
- mock capability use must not be exported as measured bring-up evidence
- shipping packages must not depend on simulator-only mocks

---

## Runtime Safety Rules

All game-facing runtime behavior must be:

- bounded
- deterministic
- rejectable
- suspendable
- resumable or cleanly stoppable
- versioned where persisted
- independent of direct hardware access

Rules:

- no game-created threads
- no direct RTOS object ownership
- no direct HAL/LL access
- no direct Platform hardware abstraction access
- no raw filesystem access during active runtime
- no runtime heap dependency unless a specific host contract later approves a bounded allocator
- no unbounded loops, recursion, retries, waits, or queues
- no transient pointer ownership across API boundaries
- no function pointers in package data
- no blocking calls without an explicit timeout
- all dynamic capability contexts must have declared lifecycle and fallback policy

The Engine or Platform may reject invalid package output during validation when it exceeds declared bounds, current mode policy, or available resources.

---

## Runtime Classes

Authoring tools define runtime units. Each runtime unit targets one runtime class:

| Runtime Class | Authoring Use | Safety Model |
|---|---|---|
| `LP_GRAPH` | low-power event/state driven experiences | event and RTC driven, minimal active work |
| `LP_MODULE` | Engine-hosted low-power modules with predefined bounded loop shapes | host-defined update and action limits |
| `RT_SCENE` | frame-paced realtime scenes | deterministic frame budget and explicit idle fallback |

`SHELL` and `INSTALLER` are Platform-owned classes, not normal game package targets.

Tools must validate that authored content uses only the features allowed by each runtime unit's declared runtime class.

Packages may contain multiple runtime units.

Example:

```text
package:
  default_runtime_unit: ambient_pet

  runtime_units:
    ambient_pet:      LP_GRAPH
    dialogue_flow:    LP_MODULE
    map_explore:      LP_MODULE
    battle_microgame: RT_SCENE
```

The default unit is the package's normal entry point.

Transitions between units are Engine-managed and must be declared and validated.

This keeps high-power or realtime behavior scoped to the unit that needs it. A package may spend most of its life in `LP_GRAPH`, briefly enter `RT_SCENE`, then return to a low-power unit.

Runtime unit transition forms:

- replace current unit with a declared target
- push a declared unit onto a bounded return stack
- pop back to the previous unit
- exit to PeepOS shell through an approved system route

Tools must not expose arbitrary jumps to undeclared unit IDs.

---

## PeepOS Power Compliance

PeepOS content must obey the device power model.

The sandbox is creatively broad, but it is not allowed to ignore the low-power behavior of the device.

Rules:

- every runtime unit must declare a runtime class and idle behavior
- every package must declare a default runtime unit
- every package must tolerate Platform cadence clamping
- every package must have a valid low-power route unless it is a Platform-owned installer/diagnostic flow
- realtime work must be declared, bounded, and able to fall back to idle/static behavior
- realtime units have no fixed maximum active duration at this contract level, but user inactivity timeout always applies
- interactive communication may request only bounded target-profile peer-wait grace through the communication contract
- packages do not keep hardware awake directly
- packages express activity, latency, wake, and cadence intent only
- Platform may force low-power behavior after the idle timeout

Contract-visible display/power behavior uses these authoring-level concepts:

| Concept | Meaning | Game Logic Runs |
|---|---|---|
| `HOLD` | last display frame remains visible; wake sources/cadence policy are armed | No |
| `ULP_ANIM` | optional autonomous prevalidated frame sequence, only when target profile grants it | No arbitrary game logic |
| `STATIC` | bounded event/state-machine behavior with low display cadence | Yes, event/cadence bounded |
| `REALTIME` | frame-paced active behavior | Yes, within frame budget |

Default authoring path should favor `STATIC -> HOLD` or `STATIC -> ULP_ANIM` patterns. This is the preferred path for long-running, low-power packages.

Power compliance validation must check:

- idle timeout behavior is declared
- static/low-power update cadence is within the selected target profile
- realtime scene has an idle fallback
- realtime scene declares idle detection, suspend behavior, and resume behavior
- transitions from realtime units back to low-power units exist where required
- package does not request background realtime updates after inactivity
- input-triggered updates are bounded and return to static/hold policy
- wake intents are declared and supported by target profile
- optional sensor wake behavior has fallback paths
- HW5 target profiles reject communication wake behavior

LPBAM-dependent `ULP_ANIM` behavior is profile-gated. It remains unavailable for shipping packages unless measured HW5 evidence grants `display.autonomous_sequence` through [[PeepOS_Capability_Registry]].

Power-facing authoring primitives:

| Primitive | Purpose | Notes |
|---|---|---|
| `idle_behavior` | declares what the package should do when inactive | `HOLD`, `ULP_ANIM`, `STATIC`, shell exit, or declared low-power unit |
| `activity_hint` | reports meaningful active work | may be expired or ignored by Platform policy |
| `cadence_request` | requests display/update cadence | Platform grants, clamps, coalesces, delays, or rejects |
| `wake_intent` | declares desired wake/event sources | must be supported by target profile |
| `latency_tolerance` | declares acceptable response delay | used by Platform when choosing sleep and cadence |
| `fallback_unit` | declares route from realtime/high-duty work back to low power | required for `RT_SCENE` |
| `capability_context` | declares bounded temporary high-duty behavior | not a settings write or hardware command |

Authoring tools should make `STATIC -> HOLD` and `STATIC -> ULP_ANIM` flows the easiest path for long-running packages.

Tools must not expose STOP level, clocks, LPBAM setup, RTC programming, DMA, display transfer internals, peripheral power state, or wake-pin configuration.

Profile-dependent behavior:

| Target Profile | Tool Behavior |
|---|---|
| `HW5_PENDING_VALIDATION` | allow design-time modeling but report hardware-dependent limits as provisional |
| `HW5_VALIDATED_BASELINE` | reject autonomous display sequence requirements; model low-power display updates as wake/update/return |
| `HW5_VALIDATED_LPBAM` | allow prevalidated autonomous display sequence output where sequence caps are met |
| `HOST_AUTHORING_PREVIEW` | allow mocks/placeholders with explicit compatibility warnings |
| `HOST_DIGITAL_TWIN_HW5` | mirror measured HW5 profile after hardware validation |

---

## Package Manifest Requirements

Every package manifest must declare:

- package ID
- name
- package version
- package format version
- default runtime unit
- runtime units
- required capabilities
- optional capabilities
- wake intents
- cadence hints
- latency tolerance
- asset table
- save schema version
- storage write budget
- compatibility constraints

The manifest is authoritative for runtime admission. Runtime code may request less than the manifest declares, but it must not request capabilities that were not declared and validated at package or runtime-unit scope.

---

## Capability Declaration Model

Capabilities are abstract Engine-visible requirements.

Canonical capability names live in [[PeepOS_Capability_Registry]].

Examples:

| Capability | Meaning |
|---|---|
| `display.mono_canvas` | can render to the logical monochrome display surface |
| `input.buttons` | can receive logical button actions |
| `input.encoder` | can receive logical encoder deltas |
| `input.joystick_vector` | can receive normalized joystick vector/action data |
| `audio.music` | can use symbolic music cues |
| `audio.sfx` | can use symbolic SFX cues |
| `audio.bbb` | can use bounded BBB tone/pattern cues |
| `audio.timeline` | can use symbolic audio timeline events for diagnostics, replay, or package logic where supported |
| `sensor.light` | can consume resolved ambient-light value and band |
| `sensor.light_stream` | can use bounded active light sampling contexts where supported |
| `sensor.imu_steps` | can consume step session totals and deltas |
| `sensor.imu_events` | can consume motion, tap, shake, tilt, or orientation events where supported |
| `sensor.imu_motion_snapshot` | can consume normalized motion/orientation snapshots |
| `sensor.imu_motion_stream` | can use bounded higher-rate motion contexts for realtime gameplay |
| `comm.multiplayer` | can use generic multiplayer sessions and bounded messages |
| `comm.companion` | can use companion-app sessions and bounded messages |
| `comm.session_required` | can declare runtime units that require an active communication session |
| `comm.message_schema` | can declare bounded versioned message schemas |
| `save.records` | can read/write package save records through Engine APIs |
| `time.calendar` | can read valid PeepOS local date/time where the target profile grants it |
| `time.delayed_event` | can schedule bounded package events after a duration |
| `time.calendar_schedule` | can schedule bounded package events against local calendar rules |
| `time.frame_delta` | can consume runtime host frame delta in realtime units |

Capability names are not hardware names. They must not include pin numbers, DMA channels, HAL handles, or device register names.

---

## Target Profiles

Tools validate packages against a target profile.

Target profiles define which capabilities and limits are available for a given target.

Required profile families:

- `HW5_PENDING_VALIDATION`
- `HW5_VALIDATED_BASELINE`
- `HW5_VALIDATED_LPBAM`
- `HOST_AUTHORING_PREVIEW`
- `HOST_DIGITAL_TWIN_HW5`

Rules:

- `HW5_PENDING_VALIDATION` is useful for tool design but not shipping authority
- hardware-derived profiles require evidence in [[Brought_Up_Tracker]]
- `HOST_DIGITAL_TWIN_HW5` is derived from measured HW5 behavior after Platform validation
- package compatibility reports must list the target profile used for validation
- packages may declare profile-specific fallbacks

Target profile fields are defined in [[PeepOS_Capability_Registry]] and [[Digital_Twin_Host_Runtime_Contract]].

---

## Authoring Tool Families

Game tools should expose high-level creation workflows that compile to general PeepOS primitives.

Recommended tool families:

| Tool Family | Compiles To | Notes |
|---|---|---|
| low-power FSM editor | state graph, timers, action tables, wake/cadence intent | preferred default for ultra-low-power packages |
| scene editor | scene graph, draw commands, asset references, input maps | used by static and realtime content |
| tile/map importer | bounded tilemap assets, viewport metadata, collision/data tables | may import from tools such as Tiled, but runtime output must be bounded |
| animation editor | sprite/frame animation tables and optional ULP sequence candidates | LPBAM/autonomous output is profile-gated |
| dialogue/text editor | text tables and state graph actions | no filesystem paths at runtime |
| audio cue editor | music/SFX/BBB cue tables | validates duration, format, and voice bounds |
| save schema editor | save records, defaults, migrations | required before save writes |
| input map editor | actions, focus scopes, bindings | maps to logical input only |

External formats are import sources, not runtime APIs.

For example, Tiled maps may be imported, but the compiler must produce bounded PeepOS tilemap/data-table assets. A package must not stream or parse arbitrary Tiled files at runtime.

---

## Scene And State Graph Hooks

Detailed runtime logic behavior is defined in [[Runtime_Logic_State_API_Contract]].

Tools may author:

- scenes
- scene entry and exit actions
- state graph nodes
- transitions
- guarded transitions
- timers
- event reactions
- bounded action lists
- local variables with declared type and bounds

Runtime rules:

- every graph has one declared entry point
- every transition target must exist
- transition evaluation must be bounded
- action list length must be bounded
- timers must declare their timebase and maximum duration
- graph-local variables must have fixed size and declared reset/persist behavior
- invalid transition or missing asset routes to host fault handling

State graphs may express game logic. They may not express hardware policy.

`RT_SCENE` logic may be more expressive than low-power graph logic, but it must declare frame budget, idle detection, suspend/resume behavior, and fallback routing before validation can accept it.

---

## Script And Action Table Rules

Script and action table behavior is governed by [[Runtime_Logic_State_API_Contract]].

If a runtime host supports scripted logic, the script system must be explicitly bounded.

Allowed forms:

- declarative action tables
- finite state graph actions
- bounded expression evaluation
- host-approved bytecode with instruction and stack limits

Required limits:

- maximum instruction count per tick/event
- maximum stack depth
- maximum local variable storage
- maximum action table length
- no recursion unless statically bounded and validated
- no dynamic code loading after package validation
- no direct calls to Platform hardware APIs

Script validation must run before package compilation/export.

---

## Rendering Hooks

Game-facing rendering is through Engine drawing abstractions.

The detailed rendering contract lives in [[Rendering_API_Contract]].

Tools may author:

- masked 1bpp sprites
- tone5 masked sprites
- tilemaps
- tilesets
- frame animations
- text labels
- fonts
- UI panels
- simple shape primitives where supported
- integer scale factors
- precomposed low-power sequence candidates
- scene-local draw ordering

Runtime may request:

- draw masked 1bpp sprite or frame
- draw tone5 sprite or frame
- draw integer-scaled sprite
- draw tile region
- draw text from validated text table
- play animation by ID
- present frame or scene update

Rules:

- assets are referenced by ID, not filesystem path
- draw command count is bounded per frame/event
- target canvas profile must be declared by capability, not hardware peripheral name
- Engine and Platform detect changed display regions internally where useful
- no package may control SPI, DMA, EXTCOMIN, display voltage translation, or display sleep policy
- `tone5` is a semantic coverage model, not native display color
- integer scaling is the v1 scaling model for package-facing scaled sprites
- visual layer order is `UI -> GAME -> BG`
- authoring tools may expose more logical layers only if the compiler can flatten them into the bounded runtime compositor model
- system UI is reserved and may use baked crisp 1bpp assets outside the package surface
- `ULP_ANIM` uses precomposed low-power sequence assets only; it does not run arbitrary rendering logic while low-power playback is active

For the HW5 target profile, the expected primary display capability is a logical monochrome canvas matching the Platform display contract.

---

## Input Hooks

Game-facing input is logical and focus-routed.

The detailed input/focus contract lives in [[Input_Focus_API_Contract]].

Tools may author:

- action names
- action maps
- focus scopes
- button bindings
- chord bindings
- hold bindings
- repeat behavior requests
- encoder delta bindings
- joystick vector or direction bindings
- low-power wake input intents
- fallback bindings for unavailable optional inputs

Runtime may consume:

- action pressed/released
- action repeated
- action held
- chord action
- encoder delta action
- normalized joystick vector/action
- input focus gained/lost

Rules:

- Platform does not assign universal accept/back/action meanings.
- Engine focus maps logical Platform input into package actions.
- Tools may declare preferred bindings, but shell/platform policy may reserve or override system-critical inputs.
- `BTN_BOOT` is never normal game input.
- Start shipping intent is power policy, not game input.
- raw GPIO, EXTI, timer counter, or ADC input is forbidden.
- raw joystick magnetic readings and encoder hardware counters are forbidden.
- input focus must be released or transferred during runtime-unit transitions.
- wake input is delivered through normal resume/lifecycle flow before package actions.

---

## Audio Hooks

Detailed audio API behavior is defined in [[Audio_API_Contract]].

Game-facing audio is symbolic and creatively open. PeepOS does not require packages to remain semantically complete when muted.

Tools may author:

- music cues
- SFX cues
- BBB patterns
- BBB tones
- BBB sweeps
- cue priorities
- cue groups
- volume defaults
- loop flags
- fade hints
- audio contexts
- symbolic timeline markers

Runtime may request:

- play/stop/pause/resume music cue
- play SFX cue
- stop SFX group
- play BBB pattern
- play bounded BBB tone
- play bounded BBB sweep
- set bus volume intent
- set mute intent
- consume symbolic cue timeline events where supported

Rules:

- audio assets must be prevalidated and package-contained
- SFX and music formats must match Engine/Platform accepted formats
- BBB sequence duration, step count, frequency range, and repeat count are bounded
- physical output may be muted, suppressed, faded, ducked, stopped, or quarantined by PeepOS policy
- audio-centric gameplay is allowed
- no game code may control SAI, DMA, LPTIM, `SD_MODE`, or amplifier state
- no FileX/FAT streaming in active runtime loops
- package logic must not depend on DMA callbacks, buffer refill timing, SAI completion, or LPTIM interrupts

---

## Asset Hooks

Game runtime accesses assets by ID through Engine/package APIs.

Asset classes may include:

- image/sprite assets
- tilemap assets
- animation tables
- audio assets
- BBB pattern assets
- text/localization assets
- graph/state table assets
- data tables

Rules:

- every asset has a type, ID, size, checksum, and compatibility metadata
- asset references must resolve at validation time
- asset bounds must match the declared runtime class
- missing, corrupt, or incompatible assets must reject install or route to runtime fault handling
- runtime code must not access host-visible staging paths directly
- active runtime reads use package-safe asset APIs only

---

## Save Data Hooks

Save data is schema-driven and package-owned through Engine save APIs.

The detailed save/settings contract lives in [[Package_Save_Settings_API_Contract]].

Tools may author:

- save schema version
- record types
- default values
- migration declarations
- maximum record sizes
- write frequency assumptions
- reset/erase behavior

Runtime may request:

- read save record
- write save record
- enumerate package-owned save keys where allowed
- migrate save record through approved migration path
- reset package-owned save data through explicit user/system flow

Rules:

- saves are not direct filesystem files
- saves are not host-writable staging content
- writes are bounded and power-safe through Platform storage
- schema changes require versioning
- failed write must preserve the previous valid record where possible
- high-frequency writes may be clamped or rejected
- package settings use the same package-owned schema discipline
- package code must handle save/settings API failure
- no save/settings API may mutate Platform settings, calibration, BLE bonding, install metadata, or fault logs

---

## Time, Cadence, And Wake Intent Hooks

Detailed time and power intent API behavior is defined in [[Time_And_Power_Intent_API_Contract]].

Games express timing intent and consume PeepOS calendar/logical time. Platform owns physical timing policy.

Tools may author:

- calendar-time requirements
- desired tick cadence
- delayed event requests
- local calendar schedule requests
- wake intent
- idle cadence hints
- latency tolerance
- deadline class
- catch-up policy

Runtime may request:

- schedule event after bounded delay
- schedule event against local calendar rule
- request next low-power tick
- request realtime frame pacing
- publish idle/active hint
- declare temporary wake intent
- read PeepOS local calendar time
- consume elapsed suspend/resume time

Rules:

- Platform may clamp, coalesce, delay, or reject cadence requests
- no package may directly program RTC, SysTick, timers, STOP mode, or clocks
- no package may set, correct, or resync RTC/calendar time
- timing knobs must use documented timebase domains
- realtime scenes must have explicit frame budget and idle fallback
- low-power graph/module content must tolerate missed or delayed wake where policy requires it
- calendar-dependent packages may assume valid PeepOS time after system setup/admission
- missed schedule catch-up must be bounded

---

## Sensor Hooks

Detailed sensor API behavior is defined in [[Sensor_API_Contract]].

Game-facing sensor data is normalized, resolved, and capability-gated.

Tools may author:

- sensor capability requirements
- sensor context declarations
- sample cadence hints
- event interests
- calibration dependency declarations
- optional content fallback behavior for target-profile portability

Runtime may consume:

- resolved ambient-light value
- ambient-light band
- IMU step session delta or total snapshot
- motion/tap/shake/tilt/orientation events where supported
- normalized motion/orientation snapshots or streams where supported
- normalized joystick vector through input APIs

Rules:

- raw ADC, raw I2C registers, raw magnetic diagnostic values, and raw IMU configuration are not normal game APIs
- Platform may internally clamp sensor rate and duration while preserving the package-facing contract
- required sensor primitive failure is handled by Platform/Engine fault logging and lifecycle policy
- optional sensor features require declared content fallback behavior
- sensor streaming contexts are bounded
- sensor contexts must not change Platform sleep policy directly

---

## Communication Hooks

Detailed communication API behavior is defined in [[Communication_API_Contract]].

Communication is generic, transport-agnostic, and capability-gated.

Tools may author:

- multiplayer capability requirement
- companion-app capability requirement
- session role intent
- message schema
- maximum message size
- rate limits
- `optional` or `session_required` context behavior
- timeout and session-end route behavior

Runtime may request:

- advertise session
- join session
- leave session
- send bounded message
- receive bounded message/event
- query communication capability state

Rules:

- no package may control BLE hardware, NINA pins, UART, bonding storage, or BLE command protocol
- messages must have fixed maximum size and schema version
- each communication runtime unit must declare either fallback/route behavior or session-required admission behavior
- peer disconnects, session closes, and message timeouts are package-visible session events
- BLE/NINA/UART faults are Platform/Engine diagnostics, not normal gameplay branches
- pairing/bonding is Platform-owned
- HW5 communication cannot wake the device
- Platform may reject invalid communication use during validation or admission where it would violate power, storage, or realtime policy

---

## Power Intent Hooks

Detailed time and power intent API behavior is defined in [[Time_And_Power_Intent_API_Contract]].

Games and tools express power intent only.

Tools may author:

- runtime class
- cadence hints
- latency tolerance
- required wake intents
- expected audio/display/sensor/communication activity
- idle behavior

Runtime may publish:

- active hint
- idle hint
- realtime work pending
- low-power work acceptable
- temporary capability need

Runtime units may declare temporary capability contexts through Engine APIs, such as high-rate sensor sampling, step-counter session behavior, audio activity, communication session activity, or realtime activity. These are validated PeepOS contexts, not direct hardware control.

Rules:

- Platform chooses sleep class and clock profile
- Platform arms wake sources
- Platform owns quiesce/resume sequencing
- invalid context use is caught before package compilation/export where possible
- if a required context cannot be maintained at runtime, Platform/Engine handles fault logging and lifecycle policy
- packages must tolerate delayed wake within declared policy
- no package may enter sleep, change clocks, or keep hardware awake directly

---

## Content Parameter Hooks

Packages may define content parameters through package schemas.

Content parameters are package-authored values used for balancing or authored behavior. They are not Platform knobs.

Allowed content parameter examples:

- pet hunger or energy rates
- encounter weights
- animation timing
- dialogue timing
- puzzle constants
- package-local difficulty defaults

Rules:

- content parameters live in package source data, package schemas, or generated package content.
- content parameters may be edited by normal game-authoring tools.
- content parameters may be previewed in the digital twin.
- content parameters must compile into package data or package-owned settings.
- content parameters must not mutate Platform knobs, Platform settings, hardware policy, sleep policy, storage policy, sensor policy, PMIC policy, or communication policy.
- any hardware-facing intent must be expressed through target profile validation and bounded capability requests.

---

## Package Settings Hooks

Packages may define package-owned settings through schemas.

The detailed save/settings contract lives in [[Package_Save_Settings_API_Contract]].

Allowed package settings examples:

- difficulty
- text speed
- package-local sound preference
- package-local input preference
- accessibility preference local to the package

Rules:

- package settings are not Platform settings
- PeepOS may render and edit package settings through system UI
- package settings are stored through package save/settings APIs
- packages may not directly mutate Platform settings, hardware policy, sleep policy, storage policy, sensor policy, PMIC policy, or communication policy
- hardware-affecting behavior is expressed through bounded capability contexts, not direct settings mutation

---

## Diagnostics Hooks

Detailed diagnostics API behavior is defined in [[Diagnostics_API_Contract]].

Game-facing diagnostics are bounded and optional.

Runtime may emit:

- package event markers
- package fault codes
- counters
- lightweight timing markers
- compatibility report references
- bounded trace values in dev/twin profiles

Rules:

- diagnostic output is rate-limited
- diagnostics do not own SWD, SWO, USB, UART, storage, or BLE
- package diagnostics must not expose protected Platform storage
- package diagnostics explain package behavior, not Platform hardware behavior
- shipping packages may retain minimal package fault codes and selected bounded counters
- production builds may compile out verbose package diagnostics while preserving fault classification

---

## Forbidden APIs

The following are never part of the game-authoring or game-runtime surface and should be unrepresentable in normal PeepOS tools:

- STM32 HAL or LL APIs
- CubeMX generated handles
- Platform `ps_hw_*` APIs
- GPIO, EXTI, DMA, ADC, SPI, I2C, SAI, UART, OCTOSPI, RTC, or timer control
- ThreadX object creation or direct queue/event ownership
- FileX, LevelX, USBX, or raw flash access
- direct mount, unmount, or host-export operations
- direct sleep, clock, reset, watchdog, or PMIC control
- raw BLE/NINA commands or bonding storage
- raw device register read/write
- arbitrary host filesystem paths

Any package or generated output containing forbidden tokens or imports must fail internal safety verification before compilation/export.

Normal authoring tools should report problems in PeepOS terms, such as missing scene entry, asset too large for profile, save field missing schema, or capability fallback missing. They should not expose low-level names to game developers.

---

## Versioning And Compatibility

Versioned contracts are required for:

- package format
- manifest schema
- asset table schema
- scene/state graph schema
- input map schema
- save schema
- optional script/bytecode format
- message schema

Rules:

- major version mismatch rejects install or compilation
- minor version mismatch is allowed only when backward-compatible
- tool output records schema versions and tool version
- runtime fault reports include package ID and relevant schema versions

---

## Required Toolchain Checks

Every package build must produce:

- validation report
- compatibility report
- `PeepPkg` package blob
- selected build profile
- selected target profile
- waiver list
- schema version list
- required and optional capability list
- power compliance summary
- asset inventory and checksums
- save schema summary
- runtime class summary
- generated package checksum

User-facing reports must use PeepOS authoring terms.

Internal verifier reports may include low-level forbidden-token diagnostics for toolchain developers, but these should normally be treated as tool/compiler defects rather than ordinary game-author mistakes.

The package compiler must refuse to emit installable output when:

- required capability is unknown or unavailable for the target profile
- graph validation fails
- asset bounds are exceeded
- save schema is invalid
- power compliance validation fails
- forbidden API access is detected
- runtime class rules are violated
- deterministic build checks fail
- runtime safety warnings remain unresolved

---

## Validation Cases

1. valid `LP_GRAPH` package validates, compiles, installs, and runs without hardware assumptions.
2. normal PeepOS authoring UI provides no path to hardware, RTOS, filesystem, or Platform-internal concepts.
3. generated artifact containing HAL, RTOS, GPIO, DMA, filesystem, or `ps_hw_*` tokens fails internal safety verification.
4. package requesting undeclared capability at runtime is rejected by Engine.
5. package with unresolved required asset ID fails authoring validation unless an approved placeholder policy applies.
6. package with placeholder art can run in `authoring_preview` or `dev_package` profile.
7. package with unbounded graph loop or action list fails validation in every profile.
8. package with oversized audio, BBB, save, communication, or render command data fails validation.
9. runtime class mismatch fails validation before package compilation/export.
10. install-time validation rejects a package whose manifest/checksum was corrupted after tool validation.
11. optional sensor or communication feature validates only when declared fallback behavior exists.
12. suspend/resume during active package behavior preserves host and package state consistency.
13. save write failure preserves previous valid save record where possible.
14. sensor streaming context above the target profile limit fails validation before package compilation/export.
15. waived warning appears in the compatibility report with reason and removal condition.
16. `RT_SCENE` package without idle fallback fails power compliance validation.
17. package requiring `display.autonomous_sequence` fails shipping validation unless the selected target profile grants it.
18. package that attempts to keep realtime cadence after inactivity timeout fails power compliance validation.
19. package that requires `ULP_ANIM` validates only against a target profile that grants autonomous display sequence support.
20. package that exposes or requires display transfer/internal update-region control fails internal safety verification.

---

## Rule

Game-authoring tools create validated content and intent.

Engine runtime hosts execute reusable bounded abstractions.

Platform owns hardware policy.
