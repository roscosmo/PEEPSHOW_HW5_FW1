# Bring-up Index

Bring-up documentation records how HW5 becomes known-good.

This section is operational and historical. It tracks validation sequences, failures, fixes, temporary measures, and evidence.

## Core Notes

- [[Dev_Orchestration_CLI_Contract]]
- [[Bootstrap_and_Build]]
- [[FW0_Phased_CubeMX_Bring-up_Plan]]
- [[HW5_Arrival_Phase0_Checklist]]
- [[Bring-up_Spec_vs_Tracker]]
- [[Brought_Up_Tracker]]
- [[Brought_Up_Archive]]
- [[Evidence_Artifact_Convention]]
- [[Pending_Measured_Constants_Register]]
- [[Validation_Plan]]
- [[Debug_Workflows]]
- [[Bounded_Build_Flash_Debug_Runbook]]
- [[Power_Measurement_and_Trace_Correlation_Runbook]]
- [[USB_Development_Mode_Contract]]
- [[Live_Tuning_And_Knobs_Contract]]
- [[Telemetry_And_Debug_Dashboard_Contract]]
- [[Tracealyzer_Snapshot_Evidence_Contract]]
- [[HW5_Hardware_Documentation_Readiness]]

## Subsystem Runbooks

- [[ADP5360_Power_Bring-up_Runbook]]
- [[LS013B7DH05_Display_Bring-up_Runbook]]
- [[AT25SL128A_External_Flash_Bring-up_Runbook]]
- [[USB_MSC_Bring-up_and_Recovery_Runbook]]
- [[Audio_Output_Bring-up_Runbook]]
- [[Button_Input_Bring-up_Runbook]]
- [[Rotary_Encoder_Bring-up_Runbook]]
- [[TEMT6000_Light_Sensor_Bring-up_Runbook]]
- [[LIS2DUX12_IMU_Bring-up_Runbook]]
- [[TMAG3001_Joystick_Bring-up_Runbook]]
- [[NINA_B112_BLE_Bring-up_Runbook]]
- [[Sleep_Wake_Integration_Bring-up_Runbook]]

## Bring-up Rule

Do not mark a phase complete from assumption.

A phase is complete only when the expected behavior is measured on HW5 hardware and evidence is linked from [[Brought_Up_Tracker]].

## Digital Twin Rule

The [[Digital_Twin_Host_Runtime_Contract]] is post-validation work.

The host digital twin is implemented from measured PeepShow Platform behavior after HW5 Platform validation is complete. It can validate Engine, package, game-logic, replay, and contract-parity behavior, but it cannot provide hardware bring-up evidence.
