# Hardware Index

This section is the HW5 board authority.

It records the schematic/PCB/BOM basis, canonical part tokens, pin ownership, clock tree, DMA map, power rails, wake sources, and CubeMX configuration expectations.

## Current Hardware Documentation Track

- [[HW5_Hardware_Documentation_Readiness]]

Reference Game expansion is paused while hardware, Platform contracts, and bring-up coverage are completed.

## Core Notes

- [[HW5_Hardware_Revision_Contract]]
- [[HW5_Part_Tokens]]
- [[HW5_CubeMX_Pin_Map]]
- [[HW5_Pin_Ownership_Matrix]]
- [[HW5_Clock_Tree_Contract]]
- [[HW5_DMA_Map]]
- [[HW5_Power_Rails]]
- [[HW5_Wake_Sources]]
- [[CubeMX_Configuration_Checklist]]

## Visual Maps And Assets

- [[Hardware_Canvas_Index]]
- [[HW5_Hardware_Visual_Map]]
- [[Hardware_Asset_Export_Guide]]

## Bring-Up Coverage

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

## Required Rule

The `.ioc` file must agree with this section.

If CubeMX generation changes pin ownership, clocks, DMA, or wake behavior, update this section before firmware architecture work continues.
