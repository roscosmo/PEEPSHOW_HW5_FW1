# Package Contract

This document defines the package-facing contract independent of hardware implementation details.

---

## Package Model

Packages provide:
- metadata
- assets
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
- `runtime_class` (`LP_GRAPH`, `LP_TEMPLATE`, `RT_SCENE`)
- `required_capabilities`
- `wake_intents`
- `cadence_hints`
- `asset_table`
- `save_schema_version`

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

At install time:
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

