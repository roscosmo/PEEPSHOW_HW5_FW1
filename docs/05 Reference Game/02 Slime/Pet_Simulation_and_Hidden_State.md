# Pet Simulation And Hidden State

Pet behavior is psychological, not decorative.

The slime develops habits, reacts emotionally, forms preferences, remembers routines, and changes over time.

## Deterministic Hidden State

Behavior should not be arbitrary RNG.

The player may not know the cause, but the system should have one.

State may include:

- mood
- comfort
- trust
- irritation
- curiosity
- overstimulation
- fatigue
- hunger-like needs if used
- environmental exposure history
- routine memory
- form pressure
- relationship rupture/recovery state

## Creature Literacy

The player gradually learns:

- preferred routines
- comfort behaviors
- emotional triggers
- feeding or care patterns if used
- transformation tendencies
- how the slime reacts to light, motion, time, and touch

This creates practical emotional understanding.

## Hardware-Aware Inputs

Pet simulation may consume normalized or abstracted signals from:

- [[Rotary_Encoder_Input_Contract]] for petting, scratching, overstimulation, and ritual movement
- [[IMU_Contract]] for carrying, bedtime posture, shaking, walking, and step context
- [[Light_Sensor_Contract]] for darkness, exposure, hiding, and form pressure
- [[PMIC_and_Power_Contract]] for charging and low-battery emotional expression
- [[Button_Input_Contract]] and [[Joystick_Hall_Input_Contract]] for direct interaction

## Boundary

The pet simulation consumes Engine/Platform events.

It does not read hardware directly.