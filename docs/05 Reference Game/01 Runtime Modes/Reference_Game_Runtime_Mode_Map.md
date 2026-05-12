# Reference Game Runtime Mode Map

The Reference Game uses three game-facing mode labels:

- STOP Mode: ambient pet existence
- STATIC Mode: main turn-based play
- REALTIME Mode: short high-intensity bursts

These labels describe game behavior.

They do not grant the Reference Game authority over MCU sleep modes, clocks, DMA, or peripheral ownership.

## Mapping To Engine Hosts

| Game-Facing Mode | Primary Purpose | Likely Engine Host | Platform Notes |
|---|---|---|---|
| STOP / Ambient | low-power pet presence, RTC/event updates, idle animation | `LP_GRAPH` | Platform chooses actual sleep class through [[Power_and_Sleep_Policy]] |
| STATIC | turn-based world, towns, dungeons, dialogue, inventory | `LP_TEMPLATE` or low-cadence hosted runtime | display uses dirty/partial updates through [[Display_and_Rendering_Contract]] |
| REALTIME | 1-5 second action, ritual, combat, sensor scenes | `RT_SCENE` | explicit power intent, bounded duration, then return to low-power policy |

## Mode Transition Shape

```text
Ambient pet presence
-> player interaction or scheduled event
-> static exploration or shell-like interaction
-> realtime burst when triggered
-> return to static or ambient
```

## Rules

- Runtime mode requests express intent only.
- Power, sleep depth, wake sources, clocks, DMA, and peripheral setup remain Platform-owned.
- Engine APIs must be reusable for non-Reference-Game content.
- Every realtime burst must have a bounded entry, duration, exit, and fallback.

## Related Notes

- [[Ambient_Pet_Runtime]]
- [[Static_Exploration_Runtime]]
- [[Realtime_Microgame_Runtime]]
- [[Runtime_Host_Contract]]
- [[Package_Contract]]
- [[Power_and_Sleep_Policy]]