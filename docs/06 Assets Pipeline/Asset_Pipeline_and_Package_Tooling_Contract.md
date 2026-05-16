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
- asset metadata tables
- image/audio source assets
- optional script/config data allowed by host contract

---

## Package Build Outputs

Tooling outputs must include:
- normalized manifest
- packaged asset blobs/chunks
- integrity metadata
- version identifiers

Output format must be deterministic from identical inputs.

---

## Schema Governance

- all package schemas are versioned
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
5. scene/state graph validation where present
6. internal forbidden hardware, RTOS, filesystem, and Platform-internal API scan
7. deterministic build checks
8. integrity/checksum generation
9. final package compatibility report

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
