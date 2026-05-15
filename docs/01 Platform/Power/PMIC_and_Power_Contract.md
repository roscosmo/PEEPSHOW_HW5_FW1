# PMIC and Power Contract

This document defines the HW5 Platform contract for the ADP5360 PMIC, battery policy, VBUS detection, shipping-mode handling, and system power state ownership.

Related:

- [[Power_Architecture_Index]]
- [[Power_and_Sleep_Policy]]
- [[Button_Input_Contract]]
- [[HW5_Pin_Ownership_Matrix]]
- [[HW5_Power_Rails]]
- [[HW5_Wake_Sources]]

---

## Hardware

PMIC: `ADP5360`.

Confirmed connections:

- ADP5360 is connected to MCU I2C at address `0x46`.
- ADP5360 interrupt is `PMIC_INT` on `PB15` / `EXTI15`.
- `BTN_START` is connected to the ADP5360 `MR` path.
- VBUS can be detected through ADP5360 status and through the MCU USB VBUS sense path.
- `USB_OTG_FS_VBUS` is on `PA9`.

Per [[Platform_Hardware_Abstraction_Contract]], the PMIC driver uses the public 7-bit address `0x46`; STM32 HAL shifted-address handling is hidden inside the `ps_hw_i2c3` layer.

---

## Ownership

- `thPower` owns PMIC configuration, battery policy, charger state, VBUS classification, sleep entry, wake classification, and power fault escalation.
- `thInput` owns START button edge/hold classification and publishes shipping-intent events to `thPower`.
- Other owners publish activity blockers or quiesce acknowledgements; they do not directly enter STOP or change power policy.
- The Reference Game and Engine express power intent only.

---

## Power Policy Decisions

- Low battery threshold causes forced sleep.
- Critical battery threshold disconnects the ISOFET.
- Critical battery must not enter shipping mode as the battery response, because START can wake the PMIC/device and create a shipping/wake loop.
- The device may run normally while charging.
- Flashing/install mode is the exception: charging does not imply normal runtime use while the device is in flashing mode.
- All main power rails may remain active; power savings come primarily from MCU sleep, peripheral low-power modes, local enables, and device-specific shutdown/deep-power-down.

---

## START / Shipping-Mode Rule

START is both a normal system button and the ADP5360 `MR` shipping-mode path.

Rules:

- firmware may detect START hold intent early
- firmware should save state and quiesce before the hardware shipping threshold
- firmware may display warning/countdown UI
- firmware must not assume it can prevent shipping mode once the ADP5360 threshold is reached
- low-battery and critical-battery policy must not use shipping mode as the automatic response

---

## VBUS Detection

VBUS may be classified from:

- ADP5360 charger/input status
- MCU `USB_OTG_FS_VBUS` on `PA9`

Both paths should be reconciled by `thPower`.

Disagreement between the PMIC VBUS view and MCU VBUS view is a diagnostic event until explained. It must not silently change installer/storage ownership.

---

## System Power FSM

This state machine describes what the whole device is doing from a power-management perspective.

| State | Meaning |
|---|---|
| `PWR_BOOTING` | MCU has started, but clocks, rails, PMIC state, reset reason, and owner threads are not trusted yet. |
| `PWR_RAIL_VALIDATE` | Firmware validates power conditions, reset cause, required rails, and PMIC status before normal operation. |
| `PWR_ACTIVE_LP` | Awake low-power operation. Display/UI may update slowly, sensors are mostly off, and STOP residency is preferred. |
| `PWR_ACTIVE_RT` | Awake realtime operation. Used for gameplay, realtime display, audio, sensor streaming, install activity, or other high-duty work. |
| `PWR_SLEEP_PREP` | Owners quiesce DMA/peripherals, save required state, and arm approved wake sources. |
| `PWR_STOP_RESIDENT` | MCU is in STOP/low-power sleep. Display hold, RTC, and armed wake sources remain valid. |
| `PWR_WAKE_RESUME` | MCU woke and is restoring clocks, classifying wake reason, and resuming owners. |
| `PWR_FORCED_SLEEP` | Battery or policy requires sleep regardless of normal runtime intent. |
| `PWR_SHIP_PREP` | START hold indicates shipping mode may be reached soon; firmware saves and warns before hardware cutoff. |
| `PWR_FAULT` | Power state is unsafe or incoherent; boot/fault supervisor takes over. |

Rules:

- Only `thPower` transitions this FSM.
- STOP entry requires owner quiesce acknowledgements or explicit timeout/fault policy.
- Wake resume must classify wake source before handing control back to runtime policy.
- Forced sleep is the low-battery response.
- ISOFET disconnect is the critical-battery response.

---

## ADP5360 / Battery FSM

This state machine describes what firmware knows about the PMIC, charger, and battery.

| State | Meaning |
|---|---|
| `PMIC_OFFLINE` | PMIC interface is not initialized or PMIC status is unavailable. |
| `PMIC_PROBE` | Firmware checks that the ADP5360 responds at I2C address `0x46`. |
| `PMIC_CONFIG` | Firmware applies required PMIC configuration and verifies it. |
| `PMIC_MONITOR` | Normal monitoring state for battery, charger, VBUS, interrupt, and fault status. |
| `PMIC_CHARGING` | Charger/input is present and battery is charging. Runtime may continue unless another mode blocks it. |
| `PMIC_CHARGE_DONE` | PMIC reports full or charge termination state. |
| `PMIC_LOW_BATT` | Battery crossed the low threshold; firmware warns where possible and forces sleep. |
| `PMIC_CRITICAL_BATT` | Battery crossed the critical threshold; firmware saves what it can and disconnects the ISOFET. |
| `PMIC_SHIP_PENDING` | START hold indicates ADP5360 shipping-mode threshold may be reached soon. |
| `PMIC_RECOVERING` | Firmware is retrying or revalidating after a transient PMIC/status fault. |
| `PMIC_ERROR` | PMIC or battery state cannot be trusted. System must degrade or fault depending on severity. |

Rules:

- PMIC register access must be serialized by `thPower`.
- `PMIC_INT` may wake or notify, but handling occurs in `thPower` context.
- Charging state does not grant storage/installer ownership.
- Low battery and START shipping intent are different paths and must stay separate.
- Critical battery must prefer ISOFET disconnect over shipping mode.

---

## Threshold Policy

Threshold values are tuning constants, not game policy.

Required thresholds:

- low battery warning threshold
- low battery forced-sleep threshold
- critical battery ISOFET-disconnect threshold
- charger-present debounce/filter timing
- VBUS disagreement timeout
- PMIC read retry limit

Thresholds must be logged with firmware version during bring-up tests.

---

## Failure Policy

PMIC/power faults are potentially fatal.

Fault handling depends on severity:

- transient I2C/status read failure: retry in `PMIC_RECOVERING`
- repeated PMIC read failure: degrade and report power-monitor fault
- low battery: force sleep
- critical battery: save where possible, then disconnect ISOFET
- incoherent VBUS/charger/install state: block installer ownership until resolved
- unsafe rail or reset condition: enter `PWR_FAULT`

---

## Validation Cases

1. ADP5360 probe at I2C address `0x46`
2. PMIC interrupt path from `PMIC_INT` on `PB15`
3. VBUS detected through ADP5360
4. VBUS detected through `USB_OTG_FS_VBUS` on `PA9`
5. VBUS path disagreement handling
6. charging while normal runtime is active
7. charging while flashing/install mode is active
8. low-battery forced sleep
9. critical-battery ISOFET disconnect
10. START hold shipping-prep handoff from input to power
