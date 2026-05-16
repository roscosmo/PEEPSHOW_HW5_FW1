# Package Contract

This document defines the package-facing contract independent of hardware implementation details.

Related:

- [[Game_Authoring_API_Contract]]
- [[PeepOS_Capability_Registry]]
- [[Runtime_Host_Contract]]
- [[Asset_Pipeline_and_Package_Tooling_Contract]]

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
- `save_schema_version`
- `storage_write_budget`
- `compatibility_constraints`

Capability names are defined in [[PeepOS_Capability_Registry]].

---

## Manifest Schema Outline

Final serialized schemas live in the package tooling schema files. This outline defines the required conceptual fields.

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
  save_schema_ref
  message_schema_ref
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
  required_capabilities[]
  optional_capabilities[]
  asset_refs[]
  save_refs[]
  cadence_hints
  wake_intents[]
  latency_tolerance
  idle_behavior
  allowed_transitions[]
  suspend_resume_policy
  failure_fallback
```

Rules:

- `unit_id` must be unique within the package.
- `runtime_class` must be one of `LP_GRAPH`, `LP_MODULE`, or `RT_SCENE`.
- `entry_point` must resolve to a state graph, module config, scene, or host entry accepted by that runtime class.
- `allowed_transitions` must target valid runtime units in the same package or an approved system route such as shell return.
- unit-specific required capabilities must be granted by the selected target profile.
- unit-specific optional capabilities must include fallback behavior.
- assets and save records used by the unit must be declared.
- lifecycle handlers and unit transition actions must be bounded.

Runtime class requirements:

| Runtime Class | Required Unit Fields |
|---|---|
| `LP_GRAPH` | state graph entry, bounded action table, cadence/wake intent |
| `LP_MODULE` | approved `module_type`, module config reference, low-power idle behavior |
| `RT_SCENE` | scene entry, frame budget, target FPS, idle fallback runtime unit |

`RT_SCENE` units must declare an idle fallback to an `LP_GRAPH` or `LP_MODULE` unit unless the selected target profile explicitly allows another route.

`LP_MODULE` units must name an approved Engine module type. They are not arbitrary low-power code blobs.

---

## Power Policy Schema Outline

Packages express power intent only.

```text
power_policy:
  idle_behavior
  idle_timeout_policy
  static_update_max_hz_request
  low_power_update_max_hz_request
  realtime_frame_rate_request
  realtime_idle_fallback
  wake_intents[]
  latency_tolerance_ms
```

Rules:

- Platform may clamp all cadence requests.
- `realtime_idle_fallback` is required for `RT_SCENE`.
- low-power and static update rates must fit the target profile.
- packages must tolerate forced low-power behavior after Platform idle timeout.

---

## Asset Table Schema Outline

Assets are referenced by ID, not filesystem path.

```text
asset_table:
  assets[]:
    asset_id
    asset_type
    format_version
    byte_size
    bounds
    checksum
    runtime_class_limits
    required_capability
```

Allowed asset classes include:

- sprites/images
- tilemaps
- animation tables
- audio/music/SFX
- BBB patterns
- text/localization tables
- state graph tables
- data tables

Rules:

- asset IDs must be stable within the package.
- all asset references must resolve at validation time.
- external editor files are import sources only and are not runtime assets unless compiled into PeepOS package formats.
- package runtime must not use arbitrary host or FAT paths.

---

## State Graph Schema Outline

State graphs are bounded runtime data.

```text
state_graph:
  graph_id
  entry_node
  nodes[]
  transitions[]
  timers[]
  local_variables[]
  action_tables[]
  bounds
```

Rules:

- `entry_node` is required.
- every transition target must exist.
- action table length is bounded.
- expression/instruction cost is bounded.
- timers declare timebase and maximum duration.
- graph-local variables declare type, size, reset behavior, and persistence behavior.
- graph validation failures block package compilation/export.

---

## Input Map Schema Outline

Input maps bind logical input to package actions.

```text
input_map:
  focus_scopes[]
  actions[]
  bindings[]
  repeat_policy
  chord_policy
  fallback_bindings[]
```

Rules:

- bindings use logical PeepOS input concepts only.
- Platform-reserved inputs may be rejected or overridden by shell/system policy.
- `BTN_BOOT` and Start shipping intent are not package inputs.

---

## Save Schema Outline

Save writes require a schema.

```text
save_schema:
  save_schema_id
  save_schema_version
  records[]:
    record_id
    fields[]
    max_size_bytes
    default_value
    migration_policy
  write_budget
  reset_policy
```

Rules:

- all save writes must target a declared record.
- schema changes require versioning.
- migrations must be explicit.
- write frequency assumptions must be declared.
- failed writes should preserve the previous valid record where possible.

---

## Message Schema Outline

Communication messages are bounded and versioned.

```text
message_schema:
  schema_id
  schema_version
  max_message_bytes
  message_types[]
  rate_limits
  offline_behavior
```

Rules:

- messages must fit communication capability limits.
- offline or unavailable communication behavior must be declared unless communication is a hard package requirement.

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

- Package saves go through package storage APIs only.
- Save schema version must support migration handlers.
- Package writes must be bounded and power-safe.
- Package data cannot bypass installer validation path.

---

## Versioning and Compatibility

Use semantic versioning for package format:
- `pkg_format_major`
- `pkg_format_minor`

Rules:
- Major mismatch: reject install.
- Minor mismatch: allow if backward-compatible.
- Validation output must include exact rejection reason.

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

1. validate manifest schema and signatures/checksums
2. validate runtime class compatibility
3. validate asset table bounds
4. validate save schema declaration
5. stage package before commit

---

## Minimum Package API Surface

Expose package-safe APIs only:
- metadata query
- asset read by ID
- save read/write by key/schema
- capability query
- host event submission

No HAL or RTOS internals are exposed to packages.
