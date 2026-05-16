# Communication Index

This section defines game-agnostic communication interfaces.

The HW5 communication path currently covers the BLE/NINA module over `LPUART1` with hardware flow control and module control GPIO.

## Core Notes

- [[BLE_Communication_Contract]]
- [[Communication_API_Contract]]
- [[RTOS_Ownership_and_Queue_Topology]]
- [[Power_and_Sleep_Policy]]
- [[HW5_CubeMX_Pin_Map]]
- [[HW5_Pin_Ownership_Matrix]]

## Boundary

The Platform owns the BLE module, UART, reset/mode pins, flow-control behavior, and power transitions.

The Engine may request generic communication capabilities.

The Reference Game may consume approved Engine communication APIs, but it must not configure BLE hardware directly.

The package-facing API boundary is defined in [[Communication_API_Contract]].
