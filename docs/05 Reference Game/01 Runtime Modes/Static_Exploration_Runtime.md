# Static Exploration Runtime

Static exploration runtime is the main turn-based Reference Game mode.

It handles exploration, towns, overworld, dungeons, NPC interaction, inventory, dialogue, tactical positioning, and world simulation.

## Turn Shape

```text
Player acts
-> slime acts or follows
-> enemies and world update
-> display redraws changed regions
```

## Display Philosophy

The display should remain mostly static, with changed regions updated as needed.

Small idle effects may run at low cadence:

- blinking
- one or two frame environmental motion
- subtle NPC or slime posture changes
- 1-2 Hz local effects where justified

The game requests presentation work; [[Display_and_Rendering_Contract]] owns the hardware transfer policy.

## Responsibilities

Static exploration owns game-specific behavior such as:

- grid movement
- map traversal
- NPC dialogue
- inventory and item rules
- environmental state
- dungeon interactions
- tactical combat positioning
- form-based traversal checks
- observational mystery flags

## Low-Power Intent

Static runtime should generally prefer:

- partial invalidation
- input-driven updates
- bounded animation windows
- no unnecessary realtime polling
- clear idle fallback to [[Ambient_Pet_Runtime]]

## Related Systems

- [[Turn_Based_Exploration]]
- [[Temporal_World_and_NPC_Routines]]
- [[Combat_and_Split_Slime]]
- [[Transformation_and_Form_System]]
- [[Runtime_Host_Contract]]