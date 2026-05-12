# Realtime Microgame Runtime

Realtime microgame runtime handles short high-intensity sequences.

These are the game-facing REALTIME Mode moments from the structural analysis.

## Purpose

Realtime moments provide:

- intensity spikes
- physical interaction
- emotional punctuation
- combat climaxes
- ritual pressure
- sensor-rich gameplay
- hardware showcase moments

They should usually last 1-5 seconds.

## Triggers

Realtime bursts can trigger from:

- critical hits
- special attacks
- enemy attacks
- dodge windows
- transformation events
- ritual sequences
- environmental hazards
- major emotional scenes

See [[Realtime_Microgame_Catalog]].

## Runtime Contract

Every realtime burst should define:

- entry condition
- required input capabilities
- required sensor capabilities
- display style
- audio needs
- maximum duration
- timeout result
- success result
- failure result
- clean return mode

## Platform Boundary

The Reference Game requests an `RT_SCENE`-like host and declares intent.

The Platform owns clocks, sleep blocking, DMA, audio transfer, input sampling policy, and sensor mode.

Related:

- [[Runtime_Host_Contract]]
- [[Power_and_Sleep_Policy]]
- [[Button_Input_Contract]]
- [[Joystick_Hall_Input_Contract]]
- [[Rotary_Encoder_Input_Contract]]
- [[IMU_Contract]]
- [[Light_Sensor_Contract]]
- [[Audio_Contract]]