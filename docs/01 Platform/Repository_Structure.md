# Repository Structure

This is the target layout for PeepShow HW5 after CubeMX generation and firmware scaffolding.

The current phase is documentation and architecture setup before generating the firmware base.

---

## Top-Level Layout

```text
PeepShow_HW5_FW1/
├── firmware/
│   ├── Core/
│   ├── Drivers/
│   ├── Middlewares/
│   ├── platform/
│   ├── engine/
│   ├── games/
│   │   └── reference_game/
│   ├── config/
│   ├── cmake/
│   ├── tools/
│   └── PeepShow_HW5.ioc
│
├── docs/
│   ├── .obsidian/
│   ├── 00 Inbox/
│   ├── 01 Platform/
│   ├── 02 Bring-up/
│   ├── 03 Engine API/
│   ├── 04 Game Design/
│   ├── 05 Reference Game/
│   ├── 06 Assets Pipeline/
│   ├── 07 Hardware/
│   ├── 08 Research/
│   ├── 09 Canvases/
│   ├── 10 References/
│   ├── 11 Development Tools/
│   └── assets/
│
├── assets/
├── tools/
├── README.md
└── AGENTS.md
```

---

## Directory Purpose

`docs/`

- Obsidian vault root
- architecture authority
- bring-up journal
- hardware contracts
- Engine API contracts
- development tooling contracts
- Reference Game design notes

`firmware/Core/`, `firmware/Drivers/`, `firmware/Middlewares/`

- CubeMX-generated and vendor layers
- do not modify generated code outside allowed user blocks

`firmware/platform/`

- Platform owner-thread implementations and Platform services
- game-agnostic hardware behavior only

`firmware/engine/`

- reusable runtime, scene, rendering, input-focus, package, asset, and game SDK code
- no direct peripheral ownership
- no Reference Game-specific logic

`firmware/games/reference_game/`

- the PeepShow proof-of-capability game
- consumes Engine and Platform APIs
- must not own hardware behavior

`firmware/config/`

- compile-time knobs, memory budgets, static resource configs, and schemas

`firmware/tools/`

- firmware-local generators and validators

`assets/`

- project-level source assets that are not part of the Obsidian vault

`tools/`

- repository-level helper tools
- expected home for the project orchestration CLI defined in [[Dev_Orchestration_CLI_Contract]]

---

## Recommended Internal Boundaries

`firmware/platform/include/`

- public Platform service headers

`firmware/platform/src/owners/`

- owner threads for display, audio, input, sensor, storage, power, USB, and debug

`firmware/platform/src/services/`

- non-owner Platform services that route requests and enforce policy

`firmware/engine/include/`

- reusable Engine APIs

`firmware/engine/src/runtime/`

- runtime host lifecycle and scheduling

`firmware/engine/src/rendering/`

- rendering abstractions above Platform display ownership

`firmware/engine/src/input/`

- focus and action-routing abstractions above Platform input events

`firmware/engine/src/assets/`

- package and asset loading/runtime support

`firmware/games/reference_game/`

- game-specific state, content, mechanics, and UI flow

---

## Naming Rules

- Prefix Platform-owned APIs with `ps_`.
- Prefix Engine-owned APIs with `eng_`.
- Prefix package-facing contracts with `pkg_`.
- Prefix Reference Game internals with `rg_` unless a better game-local convention is adopted.
- Use explicit enum names for all state machines.

---

## File-Level Ownership Manifest

Maintain one ownership note:

- `docs/01 Platform/File_Ownership.md`

For each module, document:

- owner
- allowed dependencies
- forbidden dependencies
- required tests
