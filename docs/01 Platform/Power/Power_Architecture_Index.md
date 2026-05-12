# Power Architecture Index

This section defines sleep classes, clock policy, wake classification, and runtime intent mapping.

## Core Notes

- [[PMIC_and_Power_Contract]]
- [[Power_and_Sleep_Policy]]
- [[Authority_and_Invariants]]
- [[Subsystem_State_Machines]]
- [[HW5_Wake_Sources]]
- [[HW5_Power_Rails]]

## Core Rule

Runtime and game layers express intent.

The Platform decides sleep depth, clock profile, wake-source arming, and resume policy.

HW5 PMIC, battery, VBUS, and shipping-mode behavior is defined in [[PMIC_and_Power_Contract]].

