# Ambient Pet Runtime

Ambient pet runtime is the Reference Game default presence state.

This is the game-facing STOP Mode described in the structural analysis, but it is not a direct command to enter MCU STOP.

The Platform decides the actual sleep class through [[Power_and_Sleep_Policy]].

## Purpose

Ambient runtime makes the slime feel continuously present.

It should support:

- idle creature simulation
- RTC-driven updates
- mood drift
- environmental adaptation
- pet interaction
- low-frequency animation
- emotional expression
- time-of-day behavior
- environmental sensing
- battery-state behavior

## Typical Behavior

The slime may:

- blink
- sleep
- wobble
- dream
- shift posture
- react to wheel input
- react to light level
- react to charging or low battery
- show small scheduled routines

## Engine Needs

Ambient runtime likely needs:

- retained pet state
- low-cadence tick intent
- event-driven wake intent
- dirty-region display presentation
- simple audio cue requests
- sensor snapshots or normalized events
- save/update hooks for pet state

These needs should be expressed through [[Runtime_Host_Contract]] and [[Package_Contract]].

## Platform Capability Links

- display persistence: [[Display_and_Rendering_Contract]]
- power intent: [[Power_and_Sleep_Policy]]
- battery state: [[PMIC_and_Power_Contract]]
- wheel interaction: [[Rotary_Encoder_Input_Contract]]
- buttons: [[Button_Input_Contract]]
- ambient light: [[Light_Sensor_Contract]]
- orientation and motion: [[IMU_Contract]]
- storage of persistent state: [[Storage_and_Installer_Contract]]

## Design Rule

Ambient runtime is not a screensaver.

It is the creature existing beside the player.