# Agent Working Rules

This repository uses `docs/` as an Obsidian vault and architectural source of truth.

The repository root is not the vault.

## Architecture Boundary

Use this model:

```text
Platform -> Engine -> Reference Game
```

The Platform owns hardware behavior and must remain game-agnostic.

The Engine owns reusable game-development APIs and must remain reusable beyond the Reference Game.

The Reference Game is the PeepShow proof-of-capability game. It validates the Platform, Engine, and tools, but it does not define hardware behavior.

## Documentation Authority

Before firmware implementation, check:

1. `docs/01 Platform/Authority_and_Invariants.md`
2. relevant Platform contract
3. relevant Hardware contract
4. relevant Engine API contract
5. Bring-up tracker or runbook evidence

Game notes may express intent and capability needs. They may not redefine Platform policy, CubeMX configuration, RTOS ownership, sleep behavior, storage ownership, or DMA policy.

## Bring-up Rule

Bring-up completion requires evidence in `docs/02 Bring-up/Brought_Up_Tracker.md`.

Do not mark hardware behavior as known-good without measured evidence.

