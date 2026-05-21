# Target Profile Schema Contract

This document defines the schema for PeepOS target profiles.

A target profile is the read-only bridge between measured Platform behavior and package/game tooling. It tells tools what capabilities, limits, cadence rules, and compatibility constraints exist for a specific target.

Target profiles are not Platform knobs. Package tools may read them for validation, but may not edit them.

Related:

- [[PeepOS_Capability_Registry]]
- [[Game_Authoring_API_Contract]]
- [[Package_Contract]]
- [[Digital_Twin_Host_Runtime_Contract]]
- [[Brought_Up_Tracker]]
- [[Validation_Plan]]
- [[Power_and_Sleep_Policy]]
- [[Display_and_Rendering_Contract]]
- [[Sensor_API_Contract]]
- [[Audio_API_Contract]]
- [[Communication_API_Contract]]

---

## Purpose

Target profiles allow tools to answer:

- which runtime classes are available
- which Engine-visible capabilities are granted
- which capabilities are optional or blocked
- what display, cadence, memory, audio, input, sensor, communication, save, and diagnostics limits apply
- whether a package is compatible with a target
- whether a feature is provisional, measured, simulated, or unavailable

They must not expose pins, registers, DMA channels, clocks, RTOS objects, HAL names, filesystem paths, flash offsets, or Platform knobs.

---

## Required Initial Profiles

| Profile | Authority | Shipping Use | Purpose |
|---|---|---|---|
| `HW5_PENDING_VALIDATION` | docs and provisional assumptions | no | design-time profile before measured HW5 evidence |
| `HW5_VALIDATED_BASELINE` | measured HW5 evidence | yes after freeze | normal HW5 profile without autonomous LPBAM display sequence support |
| `HW5_VALIDATED_LPBAM` | measured HW5 evidence | yes after freeze | HW5 profile with proven autonomous display sequence support |
| `HOST_AUTHORING_PREVIEW` | tooling model | no | editor preview with mocks/placeholders and compatibility warnings |
| `HOST_DIGITAL_TWIN_HW5` | measured HW5 profile mirrored by twin | no hardware authority | digital twin profile derived from validated HW5 behavior |

Rules:

- `HW5_PENDING_VALIDATION` may unblock tool design but must not be shipping authority.
- hardware-derived profiles require evidence links in [[Brought_Up_Tracker]].
- `HOST_DIGITAL_TWIN_HW5` must be derived from a measured HW5 profile, not invented by host tooling.
- if LPBAM/autonomous display evidence is missing, `display.autonomous_sequence` remains unavailable for shipping packages.

---

## Schema Shape

Conceptual schema:

```text
target_profile:
  profile_id
  profile_version
  profile_status
  profile_family
  source_authority
  platform_contract_revision
  engine_contract_revision
  hardware_revision
  firmware_commit
  knobs_hash
  evidence_refs[]
  generated_at

  capabilities[]
  runtime
  logic
  display
  rendering
  power
  input
  sensors
  audio
  communication
  time
  save_storage
  diagnostics
  package_limits
  compatibility_rules[]
```

Required profile statuses:

| Status | Meaning |
|---|---|
| `pending_validation` | intended behavior, no HW5 evidence yet |
| `hw_validated` | measured on HW5 and linked to evidence |
| `host_preview` | editor/simulator preview profile |
| `host_twin` | host profile derived from measured hardware behavior |
| `deprecated` | profile exists for compatibility reports only |

---

## Capability Grants

Each capability grant uses canonical names from [[PeepOS_Capability_Registry]].

```text
capability_grant:
  name
  grant_status
  may_be_required_by_package
  fallback_required_if_used
  runtime_classes[]
  constraints_ref
  evidence_ref
  notes
```

Allowed grant statuses:

| Status | Meaning |
|---|---|
| `granted` | package may require the capability in this profile |
| `optional` | package may use it only with validated fallback behavior |
| `blocked` | package must not require or use it in this profile |
| `experimental` | dev/preview only; not shipping-authoritative |
| `pending_validation` | tools may model it but must warn and prevent shipping reliance |

Rules:

- required package capabilities must resolve to `granted`.
- optional capabilities require package fallback behavior.
- `pending_validation` capabilities must fail shipping package export.
- profile validation must report every blocked or degraded capability used by a package.

---

## Runtime And Logic Fields

Required runtime fields:

```text
runtime:
  classes[]              # LP_GRAPH, LP_MODULE, RT_SCENE, plus Platform-owned SHELL/INSTALLER where relevant
  default_package_class
  allowed_transitions[]
  forced_idle_route_required
  runtime_units_max
  runtime_unit_nesting_depth_max
  runtime_transition_chain_max
```

Required logic fields:

```text
logic:
  state_count_max
  transition_count_max
  guard_count_max
  action_count_max
  action_steps_per_event_max
  local_event_queue_depth_max
  calendar_schedule_count_max
  catch_up_events_per_resume_max
  unbounded_loops_allowed = false
```

Rules:

- `RT_SCENE` units must declare frame budget, idle behavior, suspend behavior, resume behavior, and fallback route.
- `LP_GRAPH` must not request high-frequency polling.
- `LP_MODULE` must declare an approved module type.
- `forced_idle_route_required` means every package runtime unit must resolve to a concrete low-power route when PeepOS enforces inactivity policy.
- `runtime_unit_nesting_depth_max` limits how many package runtime units may be nested or suspended behind each other.
- `runtime_transition_chain_max` limits how many state/runtime transitions may execute from one event before the runtime must yield or report validation failure.
- neither field describes ThreadX stack memory.

---

## Display And Rendering Fields

Required display fields:

```text
display:
  logical_surface:
    width
    height
    logical_pixel_model    # mono_1bpp
    orientation
  native_panel_diagnostics:
    visible_to_package_tools = false
    width
    height
    native_pixel_model      # panel_native_1bpp
  static_hold_supported
  dirty_tracking_internal = true
  autonomous_sequence:
    grant_status           # blocked, pending_validation, granted, experimental
    frame_count_max
    cadence_hz_max
    payload_bytes_max
    duration_ms_max
    precomposed_only = true
    evidence_ref
```

Required rendering fields:

```text
rendering:
  layer_order_top_to_bottom[] = [UI, GAME, BG]
  masked_1bpp_supported
  tone5:
    supported
    source_model = 5_color_indexed_png
    runtime_asset_model = tone5_masked
    output_model = deterministic_1bpp_coverage
    coverage_model
    scale_integer_only = true
    integer_scale_max
    deterministic_phase_required = true
  tilemap_viewport_supported
  low_power_sequence_assets:
    supported
    pixel_model = precomposed_1bpp
    precompose_required = true
    autonomous_playback_requires_display_grant = true
  system_ui_reserved = true
```

Rules:

- package tools must not expose dirty-row controls.
- package tools author against `display.logical_surface`, not `display.native_panel_diagnostics`.
- native panel diagnostics are Platform reporting metadata only. They are hidden from normal package tools and must not expose panel command bytes, row packing, SRAM4 buffers, or transfer policy.
- tone5 is a semantic coverage model, not native display color.
- tone5 source art may be authored as a five-color indexed PNG; tooling converts it into validated `tone5_masked` package assets containing the logical tone data and masks used by the renderer.
- tone5 output must be deterministic 1bpp coverage on every backend, including the digital twin.
- autonomous display sequences must use precomposed final 1bpp frames.
- precomposed low-power sequence assets may exist even when autonomous playback is blocked; autonomous playback still requires `display.autonomous_sequence.grant_status = granted`.
- system UI is reserved PeepOS behavior for setup, calibration, package management, diagnostics, errors, shipping mode, and related system flows. It is not a package-authored game layer.

---

## Power And Time Fields

Required fields:

```text
power:
  enforced_inactivity_timeout_ms
  inactivity_low_power_route_required = true
  idle_to_low_power_forced = true
  realtime_requires_activity = true
  communication_wake_supported
  interactive_session_wait:
    supported
    awake_grace_ms_max
    remote_activity_refresh_supported
  sleep_classes[]
  wake_intents_supported[]
  lifecycle_wake_reasons[]
  latency_classes[]
  cadence:
    static_periodic_update_hz_max
    static_input_response_latency_ms_max
    low_power_periodic_update_hz_max
    low_power_update_requires_mcu_wake
    realtime_target_fps
    realtime_frame_budget_ms

time:
  calendar_read_supported
  calendar_set_by_package_allowed = false
  delayed_event_supported
  delayed_events_max
  calendar_schedule_supported
  calendar_schedules_max
  rtc_wake_intent_supported
  missed_event_policy
  missed_event_catchup_max
```

Rules:

- packages may read valid PeepOS calendar time where granted.
- packages may not set RTC/calendar time.
- user inactivity timeout always applies, regardless of package cadence requests.
- target profiles may grant only bounded interactive session peer-wait grace before the forced low-power route.
- packages decide the declared low-power route for forced inactivity, but PeepOS owns the timeout and enforcement.
- packages may request supported peer-wait policy through communication contexts, but may not author inactivity timeout or peer-wait grace values.
- remote activity refresh support must not turn keepalive traffic or arbitrary chatter into a general stay-awake path.
- static periodic updates and static input-response latency are separate profile limits.
- baseline low-power display updates that wake the MCU must be modeled separately from autonomous display sequences.
- package wake behavior uses `wake_intents_supported[]`; hardware wake source details remain Platform/HW documentation.
- `lifecycle_wake_reasons[]` are normalized package-visible reasons, not EXTI or peripheral names.
- delayed/calendar event limits are package schedule limits, not direct RTC alarm ownership.
- communication wake is blocked for HW5 profiles unless a future measured profile grants it.

---

## Input, Sensor, Audio, And Communication Fields

Required input fields:

```text
input:
  package_inputs[]
  dev_only_inputs[]
  system_override_actions[]
  encoder_supported
  joystick_vector_supported
  joystick_direction_supported
  chords_supported
  hold_repeat_supported
  low_power_wake_intents[]
```

Required sensor fields:

```text
sensors:
  light_supported
  imu_supported
  sensor_contexts[]:
    context_id
    capability
    runtime_classes[]
    power_class
    grant_status
    may_be_required_by_package
    fallback_required_if_used
    sample_rate_hz_min
    sample_rate_hz_max
    event_rate_hz_max
    wake_capable
    continuous_in_sleep
    mcu_wake_required
    duration_ms_max
    evidence_ref
```

Required audio fields:

```text
audio:
  music_supported
  sfx_supported
  bbb_supported
  timeline_supported
  voice_count_max
  sample_rate_hz
  muted_output_valid = true
```

Required communication fields:

```text
communication:
  multiplayer_supported
  companion_supported
  local_loopback_supported
  session_required_units_supported
  message_payload_bytes_max
  message_rate_max
  wake_supported = false
```

Rules:

- `package_inputs[]` lists normal logical package inputs, including Start where the target profile grants it.
- `package_inputs[]` entries are logical names, not GPIO, EXTI, pin, or board-signal names.
- `dev_only_inputs[]` lists abstract inputs available only on development hardware/profiles. Package behavior using these inputs must be stripped, nulled, or rejected on non-dev profiles.
- a physical development-only boot/debug signal may map to an abstract dev-only input, but the target profile must not expose the physical pin or boot-mode detail to normal package tools.
- `system_override_actions[]` lists Platform-owned override paths that can preempt normal input routing, such as shipping-mode entry, recovery, installer entry, or dev-mode entry.
- shipping-mode intent is a Platform override path, not the same thing as a development boot/debug input.
- Start is a normal package input unless Platform has already entered a system override path.
- package-visible sensor behavior is normalized; hardware faults are Platform/Engine diagnostics.
- sensor limits are exposed as measured PeepOS sensor contexts, not flat sensor-wide rate limits.
- sensor context IDs are abstract PeepOS names, not hardware part, register, interrupt, ADC, I2C, or pin names.
- `sample_rate_hz_*` describes Platform sampling cadence; `event_rate_hz_max` describes package-visible event delivery.
- `wake_capable`, `continuous_in_sleep`, and `mcu_wake_required` must be measured or explicitly marked pending before shipping use.
- high-duty sensor contexts must declare runtime classes and bounded duration.
- audio-centric packages are allowed; mute is user/platform policy, not package validation failure.
- communication sessions are abstract and must not expose BLE/NINA/UART terms to packages.

---

## Save, Diagnostics, And Package Limits

Required fields:

```text
save_storage:
  save_records_supported
  package_settings_supported
  record_bytes_max
  package_save_bytes_max
  package_settings_bytes_max
  writes_per_period_max
  write_period_ms
  write_on_suspend_supported
  transactional_write_supported
  preserve_previous_valid_record = true
  schema_migration_supported
  write_failure_result_required = true
  package_visible_results[]       # success, deferred, rejected_budget, rejected_schema, unavailable, failed_preserved

diagnostics:
  package_markers_supported
  package_counters_supported
  timing_scopes_supported
  trace_values_supported
  package_fault_codes_supported
  shipping_minimal_faults_supported
  event_rate_max
  payload_bytes_max
  marker_count_max
  counter_count_max
  timing_scope_count_max
  trace_value_count_max

package_limits:
  package_bytes_max
  asset_bytes_max
  runtime_ram_bytes_max
  runtime_unit_count_max
  save_settings_bytes_max
  diagnostics_table_bytes_max
  content_parameter_count_max
  content_parameter_blob_bytes_max
  string_table_bytes_max
  low_power_sequence_asset_bytes_max
```

Rules:

- save/settings limits describe Engine APIs, not filesystem access, flash offsets, erase pages, raw storage regions, or Platform settings.
- package save records and package-owned settings are schema-versioned records, not files.
- package-owned settings may influence package logic only. They must not mutate PeepOS knobs, Platform settings, calibration, BLE bonding, install metadata, power policy, or hardware policy.
- save writes may be deferred, clamped, rejected by budget, rejected by schema, unavailable, or failed while preserving the previous valid record.
- save write failure is a package-visible persistence result that package logic and tools must model. The underlying storage or hardware fault remains Platform diagnostics.
- package tools must validate save schemas, defaults, migration policy, write policy, and fallback behavior before export.
- package diagnostics are bounded records and do not own debug transports, dashboard export, Tracealyzer/SWO, USB CDC, BLE, UART, protected storage, or fault-log storage.
- shipping diagnostics must be minimal and explicitly profile-gated.
- package limits are target-profile abstractions, not memory-map facts. They must not expose SRAM bank names, linker sections, flash offsets, raw heap regions, or DMA buffer addresses.
- profile package limits must be enforced before package compilation/export.

---

## Evidence And Change Control

Hardware-derived target profiles must record:

- board revision
- firmware commit
- Platform contract revision
- knobs hash/version
- evidence artifact IDs
- validation cases covered
- date/time and maintainer

Profile changes require:

1. update profile source data
2. update compatibility validation expectations
3. update digital twin profile import if host behavior changes
4. link measured evidence for hardware-derived changes
5. revalidate packages that depend on changed limits or grants

---

## Validation Cases

1. package requiring a blocked capability fails validation.
2. package using optional capability without fallback fails validation.
3. shipping export fails against `HW5_PENDING_VALIDATION`.
4. `display.autonomous_sequence` is unavailable unless profile evidence grants it.
5. target profile contains no HAL, pin, DMA, register, RTOS object, raw filesystem, flash-offset, or Platform knob names.
6. `HOST_DIGITAL_TWIN_HW5` cannot be generated before its source HW5 profile has evidence.
7. changing profile limits invalidates stale compatibility reports.
8. package tools can read target profiles but cannot edit them.
9. dev-only inputs are stripped, nulled, or rejected on non-dev target profiles.
10. system override actions are not delivered as normal package input after Platform override handling begins.
11. package rendering validation uses `display.logical_surface`; `display.native_panel_diagnostics` is not package-authorable.
12. tone5 source art is valid only after tooling converts it to deterministic `tone5_masked` package assets.
13. `rendering.low_power_sequence_assets.supported` does not imply autonomous playback; that still requires `display.autonomous_sequence.grant_status = granted`.
14. package save/settings validation rejects records that exceed schema, size, migration, or write-budget limits before export.
15. package-visible write failure uses bounded persistence results; underlying storage faults remain Platform diagnostics.
16. package diagnostics cannot request debug transports, protected storage, or dashboard export ownership.
17. package limits expose abstract compatibility budgets, not SRAM banks, linker sections, flash offsets, heap regions, or DMA buffers.
18. interactive communication wait validates against profile peer-wait support and grace limits without granting communication wake.

---

## Rule

Target profiles publish Platform capability and limit facts to Engine/package tools.

They are read-only from game tooling and evidence-backed when hardware-derived.
