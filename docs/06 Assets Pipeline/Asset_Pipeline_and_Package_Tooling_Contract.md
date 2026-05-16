# Asset Pipeline and Package Tooling Contract

This document defines how external tools produce package content for runtime hosts.

---

## Goal

Keep tooling output stable and host-oriented so tools never depend on RTOS or hardware internals.

Tool-side validation is a required pre-compilation gate. Install-time firmware validation remains mandatory, but it is not a substitute for validating content before package compilation or export.

Normal game-authoring validation must use PeepOS concepts. Low-level forbidden-token checks are internal verifier guardrails for toolchain defects, corrupted artifacts, malicious packages, or future advanced tooling.

---

## Package Build Inputs

Tooling inputs may include:
- package manifest JSON
- state graph JSON/GraphML
- input map and focus scope definitions
- audio profile/cue/context definitions
- sensor profile/context definitions
- communication profile/message schema definitions
- asset metadata tables
- image/audio source assets
- Aseprite/PNG sprite and tile sources
- Tiled map sources
- optional script/config data allowed by host contract

---

## Package Build Outputs

Tooling outputs must include:
- normalized manifest
- packaged asset blobs/chunks
- `PeepPkg` package blob following [[Package_Blob_Format_Contract]]
- integrity metadata
- version identifiers

Output format must be deterministic from identical inputs.

The package blob is the installable artifact. Editor-native source files are never runtime assets.

---

## Schema Governance

- all package schemas are versioned
- the package container format is versioned separately from chunk schemas
- breaking schema changes require major version increment
- tooling must validate schema before package compilation or export
- firmware must reject incompatible schema versions cleanly

---

## Runtime Unit Compatibility

Packages must declare one or more runtime units.

Each runtime unit must declare a runtime class.

Tooling must validate declared runtime units against available host capabilities:
- `LP_GRAPH`
- `LP_MODULE`
- `RT_SCENE`

Capability names and target profiles are defined in [[PeepOS_Capability_Registry]].

Tooling must validate package output against the selected target profile.

---

## Input Map And Focus Pipeline

Input map output must target [[Input_Focus_API_Contract]], not Platform button, encoder, joystick, EXTI, or GPIO internals.

Required package-facing input artifacts:

| Artifact | Purpose |
|---|---|
| `action_table` | symbolic package action identifiers |
| `focus_scope_table` | bounded focus scopes, modal behavior, fallbacks, and allowed actions |
| `binding_table` | logical source-to-action bindings |
| `repeat_hold_policy` | bounded hold/repeat behavior where allowed |
| `joystick_policy` | normalized vector/direction use, deadzones, scaling, and fallback behavior |
| `encoder_policy` | logical delta mapping and acceleration/fallback behavior |
| `wake_intent_table` | package requests for approved low-power wake input classes |

Rules:

- package authors bind logical sources to package actions, not hardware pins.
- `BTN_BOOT` is never a valid package input.
- Start shipping and power-intent events are not package actions.
- every focus scope must have bounded stack behavior and a declared fallback or close path.
- optional inputs must have runtime-safe fallback behavior when the selected target profile does not provide them.
- wake input intent must be declared as capability intent; Platform decides whether and how the source is armed.
- host keyboard, mouse, and gamepad bindings in the digital twin are adapters into this contract, not runtime package sources.

Tooling must reject input maps that reference GPIO, EXTI, timer counters, I2C/register values, raw joystick magnetic readings, debounce internals, wake-pin configuration, or Platform maintenance actions.

---

## Sensor Profile Pipeline

Sensor profile output must target [[Sensor_API_Contract]], not Platform sensor drivers.

Required package-facing sensor artifacts:

| Artifact | Purpose |
|---|---|
| `sensor_context_table` | runtime-unit sensor contexts and bounds |
| `sensor_capability_refs` | PeepOS sensor capabilities used by each context |
| `event_interest_table` | motion, tap, shake, tilt, orientation, light-band, or step events |
| `step_session_table` | package step baselines and counters |
| `sensor_wake_intents` | declared low-power sensor wake intent where supported |
| `sensor_diagnostic_labels` | package-facing labels for developer traces |

Rules:

- package authors use PeepOS sensor primitives, not hardware sensors.
- target-profile validation must reject invalid mode/cadence combinations before export.
- high-rate sensor contexts must be bounded and tied to declared runtime units.
- step sessions must use package baselines and must not reset the hardware step counter.
- optional sensor features must declare content fallback behavior.
- required sensor primitive failure at runtime is handled by Platform/Engine lifecycle and diagnostics, not game logic.

Tooling must reject sensor profiles that reference ADC, GPIO, EXTI, I2C addresses, hardware part numbers, registers, HAL handles, calibration storage, sensor power state, or wake-pin configuration.

---

## Audio Profile Pipeline

Audio profile output must target [[Audio_API_Contract]], not Platform audio drivers.

Required package-facing audio artifacts:

| Artifact | Purpose |
|---|---|
| `audio_cue_table` | symbolic music/SFX/BBB cue IDs, groups, priorities, loops, fades, and defaults |
| `audio_context_table` | runtime-unit audio contexts and preload requirements |
| `audio_asset_table` | validated sampled audio references and decode budgets |
| `bbb_pattern_table` | bounded BBB tone/gap/sweep/repeat steps |
| `audio_timeline_table` | optional symbolic markers for replay, diagnostics, or package logic |

Rules:

- package authors use symbolic audio cues, not hardware output paths.
- PeepOS does not require packages to remain semantically complete when muted.
- audio-centric packages are valid when their assets, contexts, and runtime behavior are bounded.
- sampled audio assets must match the accepted target profile format.
- BBB patterns must validate duration, frequency, envelope, curve, step count, and repeat bounds.
- cue priorities, groups, loop policy, fade policy, and ducking policy must be deterministic.
- active runtime audio must not require FAT, host paths, or editor source files.

Tooling must reject audio profiles that reference SAI, DMA, LPTIM, GPIO, `SD_MODE`, amplifier state, mixer buffers, decoder internals, hardware callbacks, FAT paths, host paths, or unbounded playback behavior.

---

## Communication Profile Pipeline

Communication profile output must target [[Communication_API_Contract]], not BLE/NINA transport internals.

Required package-facing communication artifacts:

| Artifact | Purpose |
|---|---|
| `communication_context_table` | runtime-unit communication contexts, modes, roles, and routes |
| `message_schema_table` | bounded versioned message types and payload schemas |
| `session_policy_table` | session-required admission, optional fallback, timeout, and session-end behavior |
| `comm_rate_limit_table` | send/receive rate and queue limits |
| `comm_diagnostic_labels` | package-facing labels for developer traces |

Rules:

- package authors use sessions, peers, and schema messages, not BLE transport behavior.
- session-required multiplayer/companion packages are valid when declared and bounded.
- every communication runtime unit must declare either fallback/route behavior or session-required admission behavior.
- message size, message queue depth, send rate, receive rate, and processing cost must be bounded.
- HW5 target profiles must reject communication wake behavior.
- package-visible disconnect/session events are distinct from Platform BLE/NINA faults.

Tooling must reject communication profiles that reference BLE, NINA, UART, GAP, GATT, SPS, AT commands, pins, bonding storage, flow control, module reset, hardware addresses, or arbitrary byte streams.

---

## Rendering Asset Pipeline

Rendering asset output must target [[Rendering_API_Contract]], not the HW5 display driver.

Source files may include PNG, Aseprite, Tiled, font sources, or other editor-native inputs. Runtime packages must contain compiled PeepOS assets only.

Required rendering asset classes:

| Asset Class | Purpose |
|---|---|
| `masked_1bpp_sprite` | crisp black/white sprite with opacity mask |
| `tone5_masked_sprite` | semantic tone5 sprite with opacity/ownership |
| `tone5_tileset` | tone5 tile graphics for bounded tilemaps |
| `tilemap` | compact bounded map/viewport data |
| `font_1bpp` | crisp monochrome font data |
| `animation_table` | bounded frame/timing table referencing sprite/tile assets |
| `precomposed_low_power_sequence` | baked 1bpp sequence candidate for `ULP_ANIM` |

`tone5` is a coverage model, not a color-depth format.

Tone values:

| Value | Meaning |
|---|---|
| `transparent` | no ownership |
| `white` | 0% black coverage |
| `light` | about 25% black coverage |
| `mid` | about 50% black coverage |
| `dark` | about 75% black coverage |
| `black` | 100% black coverage |

Tooling may store tone5 using compact implementation formats such as color plane plus mask plane, but the schema must define the semantic tone output.

Integer scaling is the v1 package-facing scaling model. Tooling must validate output bounds, collision/placement metadata, animation frame consistency, dither phase stability, and render cost for each declared scale.

Logical authoring layers may exceed the runtime compositor layer count only when tooling can flatten or schedule them into the bounded `UI -> GAME -> BG` runtime compositor model.

System UI assets are reserved Platform/Engine assets and may remain crisp 1bpp outside package control.

Rendering assets are stored in package chunks defined by [[Package_Blob_Format_Contract]].

---

## Precomposed Low-Power Sequence Pipeline

`precomposed_low_power_sequence` assets are built before package compilation/export.

Rules:

- the sequence must resolve to final 1bpp display content before low-power playback.
- source may be sprites, tone5 assets, tilemaps, animations, or direct authored frames.
- no runtime JSON, PNG, Aseprite, Tiled, or font parsing is allowed during playback.
- sequence payload size, frame count, cadence, and wake/exit policy must fit the selected target profile.
- the package-facing asset must not encode SRAM4 addresses, LPBAM descriptors, SPI bytes, or Sharp LCD command details.
- the Platform may convert validated sequence content into full frames, row deltas, repeated payloads, or another display-owner format.
- autonomous playback remains unavailable unless the selected target profile grants `display.autonomous_sequence`.
- package chunks remain portable PeepOS data; hardware playback payloads are Platform/display-owner internals.

---

## Package Container Rules

The `PeepPkg` container defined in [[Package_Blob_Format_Contract]] is the package compiler output and installer input.

Rules:

- use `PKG1` as the initial package-container magic/version family in examples.
- chunks are addressed by stable package IDs at authoring level and compact chunk indexes/offsets at runtime level.
- every chunk has type, format version, offset, size, alignment, capability metadata, runtime-unit references, and integrity metadata.
- per-chunk CRC plus whole-package checksum are required for v1 integrity.
- cryptographic signatures are a future policy placeholder, not a v1 requirement.
- no runtime path may depend on host/editor source files.

---

## Compression And Packing Rules

V1 runtime paths do not allow general-purpose compression.

Allowed packing must be format-specific and bounded:

- bitplanes
- opacity masks
- tone planes
- fixed-layout tilemaps
- bounded ADPCM audio payloads
- simple RLE only where the chunk format declares maximum expansion size and decode budget

Tooling must reject any asset whose decode time, expanded size, or memory requirement is not statically bounded for the selected target profile.

---

## Deterministic Build Rules

- no hidden timestamps in package payload unless explicitly declared
- stable ordering for generated tables/indexes
- reproducible checksums for identical inputs

---

## Validation Steps (Tool Side)

1. schema validation
2. asset bounds and format validation
3. manifest consistency checks
4. runtime class and capability validation
5. input map and focus scope validation
6. audio profile and context validation
7. sensor profile and context validation
8. communication profile and message schema validation
9. scene/state graph validation where present
10. internal forbidden hardware, RTOS, filesystem, and Platform-internal API scan
11. deterministic build checks
12. integrity/checksum generation
13. final package compatibility report

Validation failures block package compilation or export.

Warnings that affect runtime safety, determinism, storage integrity, power policy, or capability availability must be treated as errors.

Development profiles may allow placeholders, mocks, warnings, and explicit runtime-safe waivers as defined in [[Game_Authoring_API_Contract]]. They must still block incoherent graphs, unbounded behavior, invalid save schemas, package integrity failures, and unknown runtime classes.

---

## Integration Rules

Tooling does not:
- emit hardware register assumptions
- embed RTOS queue assumptions
- assume specific peripheral timing implementation

Tooling does:
- target runtime/package contracts only
- emit intent and structured content

---

## Required Artifacts

For each package build retain:
- input manifest and schema versions
- tool version
- generated package checksum
- compatibility report
