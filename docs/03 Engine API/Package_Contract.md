# Package Contract

This document defines the package-facing contract independent of hardware implementation details.

Related:

- [[Game_Authoring_API_Contract]]
- [[PeepOS_Capability_Registry]]
- [[Target_Profile_Schema_Contract]]
- [[Content_Parameter_Schema_Contract]]
- [[Package_Save_Settings_API_Contract]]
- [[Runtime_Host_Contract]]
- [[Runtime_Logic_State_API_Contract]]
- [[Asset_Pipeline_and_Package_Tooling_Contract]]
- [[Audio_API_Contract]]
- [[Communication_API_Contract]]
- [[Sensor_API_Contract]]
- [[Time_And_Power_Intent_API_Contract]]
- [[Diagnostics_API_Contract]]

---

## Package Model

Packages provide:
- metadata
- assets
- runtime units
- state graphs/tables
- variables and configuration
- optional host-allowed scripted logic

Packages do not provide:
- direct peripheral control
- thread creation
- power mode transitions

---

## Manifest Requirements

Every package must declare:
- `package_id`
- `name`
- `version`
- `build_profile`
- `target_profile`
- `default_runtime_unit`
- `runtime_units`
- `required_capabilities`
- `optional_capabilities`
- `wake_intents`
- `cadence_hints`
- `latency_tolerance`
- `asset_table`
- `audio_profile`
- `sensor_profile`
- `communication_profile`
- `time_power_profile`
- `diagnostics_profile`
- `save_schema_version`
- `storage_write_budget`
- `compatibility_constraints`

Capability names are defined in [[PeepOS_Capability_Registry]].

---

## Manifest Schema Outline

Final serialized schemas live in the package tooling schema files. This outline defines the required conceptual fields.

The installable package container is defined by [[Package_Blob_Format_Contract]].

```text
package_manifest:
  package_id
  name
  publisher_id
  package_version
  package_format_version
  build_profile
  target_profile
  default_runtime_unit
  runtime_units[]
  required_capabilities[]
  optional_capabilities[]
  wake_intents[]
  cadence_hints
  latency_tolerance
  power_policy
  asset_table_ref
  audio_profile_ref
  sensor_profile_ref
  communication_profile_ref
  time_power_profile_ref
  diagnostics_profile_ref
  save_schema_ref
  message_schema_ref
  package_blob_ref
  compatibility_constraints
  package_checksum
```

Rules:

- `build_profile` must be one of the profiles defined in [[Game_Authoring_API_Contract]].
- `target_profile` must name a profile from [[PeepOS_Capability_Registry]].
- `default_runtime_unit` must resolve to one entry in `runtime_units`.
- each runtime unit declares its own `runtime_class`.
- required capabilities must be granted by the target profile.
- optional capabilities must include fallback behavior.
- `package_blob_ref` must resolve to the installable `PeepPkg` container metadata.

---

## Runtime Unit Schema Outline

A package may contain multiple runtime units.

The package is the installable artifact. A runtime unit is a bounded executable mode inside that package.

Examples:

```text
ambient_pet      LP_GRAPH
dialogue_flow    LP_MODULE
map_explore      LP_MODULE or RT_SCENE
microgame        RT_SCENE
```

Conceptual schema:

```text
runtime_unit:
  unit_id
  runtime_class
  entry_point
  module_type
  runtime_logic_refs[]
  required_capabilities[]
  optional_capabilities[]
  asset_refs[]
  audio_context_refs[]
  sensor_context_refs[]
  communication_context_refs[]
  save_refs[]
  cadence_hints
  wake_intents[]
  latency_tolerance
  idle_behavior
  allowed_transitions[]
  unit_stack_policy
  suspend_resume_policy
  failure_fallback
```

Rules:

- `unit_id` must be unique within the package.
- `runtime_class` must be one of `LP_GRAPH`, `LP_MODULE`, or `RT_SCENE`.
- `entry_point` must resolve to a state graph, module config, scene, or host entry accepted by that runtime class.
- `runtime_logic_refs` must resolve to validated state graph, action table, module, or scene logic data accepted by [[Runtime_Logic_State_API_Contract]].
- `allowed_transitions` must target valid runtime units in the same package or an approved system route such as shell return.
- runtime unit transitions must use declared transition edges.
- arbitrary runtime jumps to undeclared unit IDs are invalid.
- unit-specific required capabilities must be granted by the selected target profile.
- unit-specific optional capabilities must include fallback behavior.
- assets, audio contexts, sensor contexts, communication contexts, and save records used by the unit must be declared.
- lifecycle handlers and unit transition actions must be bounded.

Runtime class requirements:

| Runtime Class | Required Unit Fields |
|---|---|
| `LP_GRAPH` | state graph entry, bounded action table, cadence/wake intent |
| `LP_MODULE` | approved `module_type`, module config reference, low-power idle behavior |
| `RT_SCENE` | scene entry, frame budget, target FPS, idle fallback runtime unit |

`RT_SCENE` units must declare an idle fallback to an `LP_GRAPH` or `LP_MODULE` unit unless the selected target profile explicitly allows another route.

`RT_SCENE` units do not have a fixed maximum active duration at the package contract level. The Platform inactivity timeout always applies. A realtime unit may remain active while meaningful user activity or Platform-approved active work continues, but it must declare idle detection, suspend behavior, resume behavior, and fallback routing.

`LP_MODULE` units must name an approved Engine module type. They are not arbitrary low-power code blobs.

---

## Runtime Unit Transition Model

Runtime unit transitions are declared, bounded, and Engine-managed.

Allowed transition forms:

```text
transition_to(unit_id)
push_unit(unit_id)
pop_unit()
exit_to_shell(reason)
```

Rules:

- `transition_to` replaces the current runtime unit with a declared target.
- `push_unit` enters a declared target while preserving a bounded return path.
- `pop_unit` returns to the previous runtime unit if the stack is non-empty.
- stack depth is bounded by target profile and package validation.
- recursive push loops are invalid unless statically bounded and approved by validation.
- transition actions must be bounded.
- transition targets must be declared in `allowed_transitions`.
- `exit_to_shell` is an approved system route, not a package-defined shell implementation.

Typical use:

```text
map -> push dialogue -> pop map
idle -> push modal microgame -> pop idle
realtime microgame -> transition idle fallback
```

---

## Content Parameters, Package Settings, And Capability Contexts

Package settings and Platform settings are separate.

Detailed package save/settings API rules are defined in [[Package_Save_Settings_API_Contract]].

Detailed content parameter schema rules are defined in [[Content_Parameter_Schema_Contract]].

Content parameters:

- are package-authored balancing or behavior values
- are declared by package schemas or package source data
- may be edited by normal package-authoring tools
- compile into package data or package-owned settings
- are not Platform knobs and do not live in `config/knobs.json`

Package settings:

- are declared by package schema
- have defaults
- may be rendered or edited through PeepOS UI
- are stored through package save/settings APIs
- may include package preferences such as difficulty, text speed, package-local sound preference, or package-local input preferences

Platform settings:

- are owned by PeepOS
- are not mutated directly by packages
- include hardware, sleep, storage, power, sensor, communication, and system policy

Packages may declare temporary capability contexts.

Examples:

```text
sensor_context.motion_stream_20hz_for_microgame
sensor_context.step_session_for_walk_goal
sensor_context.light_band_for_scene_logic
communication_context.multiplayer_session
audio_context.active_music_or_sfx
runtime_context.realtime_activity_for_minigame
```

Rules:

- contexts are declarations, not settings writes.
- tool-side validation must prove each context is valid for the selected target profile and runtime unit.
- Platform may internally clamp, coalesce, substitute, or degrade hardware behavior while preserving the package-facing contract.
- package gameplay code does not handle hardware-level grant/reject/revoke paths for required HW5 primitives.
- if a required context cannot be maintained at runtime, Platform/Engine handles fault logging and lifecycle policy.
- contexts must have bounded duration, declared runtime-unit scope, or explicit release behavior.
- packages must not directly write Platform knobs, Platform settings, hardware registers, or storage policy.

---

## Power Policy Schema Outline

Packages express power intent only.

Detailed time and power intent API behavior is defined in [[Time_And_Power_Intent_API_Contract]].

```text
time_power_profile:
  calendar_requirements
  schedule_table_ref
  lifecycle_policy
  wake_intents[]
  catch_up_policy

power_policy:
  idle_behavior
  inactivity_timeout_behavior
  activity_hints[]
  cadence_requests:
    static_periodic_update_hz
    low_power_periodic_update_hz
    realtime_target_fps
  input_update_policy
  low_power_sequence_ref
  realtime_idle_fallback
  wake_intents[]
  latency_tolerance_ms
  capability_contexts[]
```

Rules:

- Platform may clamp all cadence requests.
- `realtime_idle_fallback` is required for `RT_SCENE`.
- low-power and static update rates must fit the target profile.
- packages must tolerate forced low-power behavior after Platform idle timeout.
- `idle_behavior` must resolve to `HOLD`, `ULP_ANIM`, `STATIC`, `exit_to_shell`, or another approved low-power route.
- activity hints are not exemptions from inactivity timeout.
- input-triggered updates must be bounded and return to the declared idle behavior.
- `low_power_sequence_ref` may reference portable precomposed sequence assets for preview, static fallback, or autonomous playback candidates.
- packages requiring autonomous low-power playback must target a profile that grants `display.autonomous_sequence`.
- packages targeting `display.autonomous_sequence` should declare a fallback unless the package explicitly requires that capability.
- packages must not implement polling loops to approximate low-power cadence.
- packages may read PeepOS calendar time where the selected target profile grants `time.calendar`.
- packages must not set, correct, resync, or directly access RTC hardware.
- calendar-dependent package runtime may assume valid PeepOS time after system setup/admission.
- missed scheduled events must use a bounded catch-up policy.

Cadence request semantics:

| Request | Meaning | Platform Response |
|---|---|---|
| `static_periodic_update_hz` | requested maximum periodic/static update cadence | grant or clamp to target profile |
| `low_power_periodic_update_hz` | requested low-power display/event cadence | grant, clamp, coalesce, or reject |
| `realtime_target_fps` | requested frame cadence for realtime unit | grant or clamp while realtime activity is valid |

Target profiles separately define static input-response latency caps. Input-triggered static updates may be serviced promptly, then must return to the declared idle behavior.

Profile behavior:

| Target Profile | Package Implication |
|---|---|
| `HW5_VALIDATED_BASELINE` | low-power display updates are wake/update/return operations; packages must tolerate strict cadence caps |
| `HW5_VALIDATED_LPBAM` | prevalidated autonomous low-power sequences may be used through `display.autonomous_sequence` |

---

## Asset Table Schema Outline

Assets are referenced by ID, not filesystem path.

Asset records resolve to package chunks in [[Package_Blob_Format_Contract]].

```text
asset_table:
  assets[]:
    asset_id
    asset_type
    format_version
    byte_size
    bounds
    chunk_id
    checksum
    runtime_class_limits
    required_capability
```

Allowed asset classes include:

- masked 1bpp sprites/images
- tone5 masked sprites/images
- tilesets
- tilemaps
- animation tables
- fonts
- audio/music/SFX
- BBB patterns
- text/localization tables
- state graph tables
- data tables
- precomposed low-power sequences

Rules:

- asset IDs must be stable within the package.
- all asset references must resolve at validation time.
- external editor files are import sources only and are not runtime assets unless compiled into PeepOS package formats.
- package runtime must not use arbitrary host or FAT paths.
- tone5 assets are semantic coverage assets and must not be described as a color-depth format.
- precomposed low-power sequences are package assets, but autonomous playback requires the target profile to grant `display.autonomous_sequence`.
- runtime assets are loaded by package/asset APIs from installed raw package storage or bounded caches, not by filesystem path.
- package chunks must not contain SRAM4 addresses, SPI payloads, DMA descriptors, LPBAM descriptors, or hardware row formats.

---

## State Graph Schema Outline

State graphs are bounded runtime logic data.

Detailed runtime logic behavior is defined in [[Runtime_Logic_State_API_Contract]].

```text
state_graph:
  graph_id
  entry_node
  nodes[]
  transitions[]
  event_bindings[]
  timers[]
  local_variables[]
  action_tables[]
  persistence_policy
  bounds
```

Rules:

- `entry_node` is required.
- every transition target must exist.
- action table length is bounded.
- expression/instruction cost is bounded.
- timers declare timebase and maximum duration.
- graph-local variables declare type, size, reset behavior, and persistence behavior.
- event bindings must use package-visible Engine event classes.
- action tables must use symbolic Engine requests and must be non-blocking.
- graph validation failures block package compilation/export.

---

## Input Map Schema Outline

Input maps bind logical input to package actions.

Detailed input/focus API behavior is defined in [[Input_Focus_API_Contract]].

```text
input_map:
  focus_scopes[]
  actions[]
  bindings[]
  repeat_policy
  chord_policy
  joystick_policy
  encoder_policy
  wake_intents[]
  fallback_bindings[]
```

Rules:

- bindings use logical PeepOS input concepts only.
- Platform-reserved inputs may be rejected or overridden by shell/system policy.
- `BTN_BOOT` and Start shipping intent are not package inputs.
- bindings must target package-local actions.
- focus scope stack depth must be bounded.
- optional input capabilities require fallback bindings or fallback behavior.
- runtime unit transitions must release or transfer input focus explicitly.

---

## Audio Profile Schema Outline

Audio profiles declare symbolic package audio behavior.

Detailed audio API behavior is defined in [[Audio_API_Contract]].

```text
audio_profile:
  cues[]:
    cue_id
    cue_type
    asset_ref
    bus
    group
    priority
    default_volume
    loop_policy
    fade_policy
    ducking_policy
    max_duration_ms
    preload_policy
  bbb_patterns[]:
    pattern_id
    steps[]
    priority
    max_duration_ms
  audio_contexts[]:
    context_id
    runtime_unit_refs[]
    active_cue_refs[]
    bbb_pattern_refs[]
    volume_defaults
    preload_refs[]
    power_behavior_hint
    diagnostic_label
```

Rules:

- audio cue IDs are symbolic package-local IDs.
- audio profiles must not name SAI, DMA, LPTIM, GPIO, `SD_MODE`, amplifier state, mixer buffers, decoder internals, or filesystem paths.
- sampled audio assets and BBB patterns must resolve to package assets.
- BBB pattern frequency, duration, step count, repeat count, curve, and envelope must be bounded.
- runtime unit audio contexts must be valid for the selected runtime class and target profile.
- PeepOS does not require packages to remain semantically complete when muted.
- audio-centric package behavior is valid when it remains within bounded package/runtime rules.

---

## Sensor Profile Schema Outline

Sensor profiles declare package use of PeepOS sensor primitives.

Detailed sensor API behavior is defined in [[Sensor_API_Contract]].

```text
sensor_profile:
  contexts[]:
    context_id
    runtime_unit_refs[]
    required_capabilities[]
    optional_capabilities[]
    mode
    cadence_hint
    max_duration_ms
    event_interests[]
    wake_intents[]
    fallback_policy
    diagnostic_label
```

Rules:

- sensor contexts use PeepOS capability names only.
- sensor contexts must not name hardware parts, pins, ADC channels, I2C addresses, EXTI lines, registers, or HAL handles.
- each sensor context must be referenced by at least one runtime unit.
- high-rate motion or light streaming must be bounded and valid for the runtime class.
- step sessions use package baselines and must not reset the hardware step counter.
- optional sensor features require declared content fallback behavior.
- required sensor primitive failure at runtime is handled by Platform/Engine lifecycle and diagnostics, not normal gameplay logic.

---

## Save Schema Outline

Save writes require a schema.

Detailed save/settings API behavior is defined in [[Package_Save_Settings_API_Contract]].

```text
save_schema:
  save_schema_id
  save_schema_version
  records[]:
    record_id
    record_type
    fields[]
    max_size_bytes
    default_value
    migration_policy
    reset_policy
    write_policy
    durability_class
  package_settings[]:
    setting_id
    value_type
    default_value
    allowed_values
    ui_metadata
    storage_record_ref
    migration_policy
    reset_policy
  write_budget
  reset_policy
```

Rules:

- all save writes must target a declared record.
- schema changes require versioning.
- migrations must be explicit.
- write frequency assumptions must be declared.
- failed writes should preserve the previous valid record where possible.
- package settings are package-owned schema records, not Platform settings.
- package writes use Engine save/settings APIs only.

---

## Message Schema Outline

Communication messages are bounded and versioned.

Detailed communication API behavior is defined in [[Communication_API_Contract]].

```text
communication_profile:
  contexts[]:
    context_id
    runtime_unit_refs[]
    mode
    role_intent
    session_type
    max_peers
    message_schema_ref
    rate_limits
    timeout_policy
    ordering_policy
    session_end_route
    fallback_route
    diagnostic_label
```

```text
message_schema:
  schema_id
  schema_version
  max_message_bytes
  message_types[]
  rate_limits
  session_behavior
```

Rules:

- communication profiles must not name BLE, NINA, UART, GAP, GATT, module commands, pins, or bonding storage.
- messages must fit communication capability limits.
- message schemas must be versioned and bounded.
- each communication context must declare `none`, `optional`, or `session_required` behavior.
- optional communication contexts require fallback/route behavior.
- session-required runtime units require admission/session routes when no session exists.
- HW5 profiles must reject communication wake behavior.
- peer disconnects and session timeouts are package-visible events.
- hardware/module faults are Platform/Engine diagnostics, not normal gameplay branches.

---

## Diagnostics Profile Schema Outline

Diagnostics profiles declare bounded package observability.

Detailed diagnostics API behavior is defined in [[Diagnostics_API_Contract]].

```text
diagnostics_profile:
  marker_table[]
  counter_table[]
  timing_scope_table[]
  trace_value_table[]
  warning_code_table[]
  package_fault_code_table[]
  profile_gates
  rate_limits
  export_policy
```

Rules:

- package diagnostics explain package behavior, not Platform hardware behavior.
- diagnostic IDs must be package-local and stable.
- diagnostic payload types must be declared and bounded.
- shipping diagnostics must be explicitly marked as shipping-allowed.
- package fault codes must map to Engine lifecycle policy.
- diagnostics profiles must not name SWD, SWO, UART, USB, BLE, protected storage, hardware registers, RTOS objects, or filesystem paths.
- Platform owns debug transport, persistent fault logs, and diagnostic export.

---

## Intent-Driven Policy

Packages may declare intent such as:
- "wake on button and step"
- "no periodic tick needed"
- "update every 1000 ms"
- "requires short audio cues only"

Platform decides exact hardware behavior.

---

## Storage and Save Rules

- Package saves and package-owned settings go through [[Package_Save_Settings_API_Contract]] only.
- Save schema version must support migration handlers.
- Package writes must be bounded and power-safe.
- Package data cannot bypass installer validation path.

---

## Versioning and Compatibility

Use semantic versioning for package-facing schemas and package container compatibility:
- `pkg_format_major`
- `pkg_format_minor`

Rules:
- Major mismatch: reject install.
- Minor mismatch: allow if backward-compatible.
- Validation output must include exact rejection reason.
- The `PeepPkg` container format and individual chunk schemas carry explicit versions.

---

## Validation Checklist

Before package compilation/export, tooling must validate:

1. manifest schema
2. runtime class compatibility
3. required and optional capability declarations
4. asset table bounds
5. save schema declaration
6. authoring validation in PeepOS terms
7. internal forbidden hardware, RTOS, filesystem, and Platform-internal API access

At install time, firmware must validate:

1. validate `PeepPkg` header, chunk table, ranges, and integrity metadata
2. validate manifest schema and signatures/checksums
3. validate runtime class compatibility
4. validate asset table bounds
5. validate save schema declaration
6. stage package before commit

---

## Minimum Package API Surface

Expose package-safe APIs only:
- metadata query
- asset metadata query by ID
- asset open/close by ID
- typed asset views/handles through [[Package_Asset_Loading_API_Contract]]
- bounded asset read windows for approved large assets
- save/settings read/write by declared schema through [[Package_Save_Settings_API_Contract]]
- capability query
- host event submission

No HAL or RTOS internals are exposed to packages.

Packages must not receive raw chunk offsets, storage addresses, filesystem paths, SRAM4 addresses, DMA descriptors, LPBAM descriptors, or display payload pointers.
