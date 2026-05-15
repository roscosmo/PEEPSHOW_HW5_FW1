# ADP5360 Power Bring-up Runbook

This runbook records the measured HW5 procedure for PMIC, charger, battery, VBUS, ISOFET, and shipping-mode validation.

Related:

- [[PMIC_and_Power_Contract]]
- [[Power_and_Sleep_Policy]]
- [[HW5_Power_Rails]]
- [[HW5_Wake_Sources]]
- [[Brought_Up_Tracker]]

---

## Scope

This runbook covers:

- I2C probe of ADP5360 at address `0x46`
- `PMIC_INT` behavior on `PB15` / `EXTI15`
- VBUS detection through ADP5360 and `USB_OTG_FS_VBUS` on `PA9`
- charge and charge-done state reporting
- low-battery forced sleep threshold behavior
- critical-battery ISOFET disconnect behavior
- START / ADP5360 `MR` shipping-mode path
- first-boot START shipping-intent ignore behavior

---

## CubeMX / Electrical Baseline

| Function | MCU resource | Required baseline |
| --- | --- | --- |
| PMIC bus | `I2C3` on `PC0` SCL, `PC1` SDA | ADP5360 responds at `0x46` |
| PMIC interrupt | `PB15` `PMIC_INT` | `GPXTI15` / `EXTI15_IRQn` |
| USB VBUS sense | `PA9` `USB_OTG_FS_VBUS` | alternate VBUS classification path |
| Start / MR path | `BTN_START` through ADP5360 `MR` path | firmware-visible active-low Start before hardware shipping threshold |

Per [[Platform_Hardware_Abstraction_Contract]], the driver-facing address is the public 7-bit address `0x46`; STM32 HAL shifted-address handling is hidden inside the `ps_hw_i2c3` layer. Bring-up evidence should confirm the convention used by the firmware under test.

---

## Safety Gates

Power bring-up can physically shut the device down or disconnect the battery path. Do not run the hazardous tests until the recovery method is known.

| Test | Hazard | Required gate |
| --- | --- | --- |
| START shipping-mode entry | device may enter shipping mode | prove normal Start warning/prep first; confirm 200 ms wake/recovery path |
| critical-battery ISOFET disconnect | device may lose power | perform only with controlled supply/battery setup and recovery plan |
| charger/VBUS edge tests | storage/USB policy may change | ensure no install/export/write operation is active |
| low-battery forced sleep | runtime may be stopped | ensure save/quiesce behavior is observable |

For early bring-up, prefer register-controlled, threshold-simulated, or bench-supply-controlled tests where the ADP5360 supports them. Record whether each result is measured physically or simulated through safe configuration.

---

## Baseline State Sequence

This sequence proves the PMIC monitor path before full sleep policy depends on it.

1. Boot from a known-good power source with START released.
2. Initialize I2C3 and probe the ADP5360 at `0x46`.
3. Confirm the firmware driver uses public 7-bit I2C address `0x46` through `ps_hw_i2c3`.
4. Read PMIC identity/status, charger/input status, battery/fuel state, interrupt status, and fault status registers.
5. Configure only the minimum required PMIC settings for safe monitor operation.
6. Validate `PMIC_INT` by reading/clearing a known pending condition or producing a safe charger/input event.
7. Compare VBUS classification from ADP5360 status and `PA9` USB VBUS sense.
8. Connect USB and confirm charging/input-present state without changing storage ownership by itself.
9. Disconnect USB and confirm charger/input-absent state.
10. Trigger or simulate low-battery threshold and confirm forced-sleep policy is selected.
11. Trigger or simulate critical-battery threshold and confirm ISOFET-disconnect policy is selected instead of shipping mode.
12. Hold START long enough to generate firmware shipping-prep intent below the ADP5360 hardware threshold.
13. Release START during prep and confirm firmware cancels software-side warning/prep.
14. Validate first-boot/no-settings policy ignores save/backup work for START shipping intent because there is nothing valid to preserve yet.

Do not intentionally cross the ADP5360 shipping-mode threshold until shipping entry and recovery are part of the active test plan.

---

## Threshold / Policy Ledger

Populate this table during bring-up. Values are placeholders until selected and measured.

| Policy item | Initial value | Evidence required | Status |
| --- | --- | --- | --- |
| low battery warning threshold | TBD | warning event and log | open |
| low battery forced-sleep threshold | TBD | forced-sleep transition evidence | open |
| critical battery ISOFET-disconnect threshold | TBD | controlled disconnect or safe simulation evidence | open |
| charger-present debounce/filter | TBD | USB connect/disconnect logs | open |
| VBUS disagreement timeout | TBD | ADP5360 vs `PA9` mismatch handling | open |
| PMIC read retry limit | TBD | transient failure recovery test | open |
| START ship-prep warning threshold | TBD | warning starts early enough for save/quiesce | open |
| START imminent threshold | TBD | final warning before hardware cutoff | open |

Thresholds are Platform tuning constants, not Reference Game policy.

---

## Command / Configuration Ledger

| Step | Configuration | Expected result | Measured result | Status |
| --- | --- | --- | --- | --- |
| I2C probe | address `0x46` | ADP5360 ACKs | TBD | open |
| address representation | public 7-bit `0x46` through `ps_hw_i2c3` | convention confirmed | TBD | open |
| status read | PMIC status registers | charger/battery/fault state readable | TBD | open |
| PMIC_INT | safe event or pending clear | EXTI15 event and owner handling | TBD | open |
| VBUS cross-check | USB attach/detach | ADP5360 and `PA9` agree or log diagnostic | TBD | open |
| charging | USB attached | charging/charge-done state reported | TBD | open |
| low battery | simulated or measured threshold | forced sleep selected | TBD | open |
| critical battery | simulated or controlled threshold | ISOFET disconnect selected, not shipping mode | TBD | open |
| START prep | sustained hold below hardware cutoff | warning/save/quiesce path starts | TBD | open |
| START release | release during prep | software warning/prep cancelled | TBD | open |
| first boot | no settings/calibration | no save/backup dependency during ship prep | TBD | open |

---

## Validation Procedure

1. Confirm board powers safely from battery and USB input.
2. Probe ADP5360 over I2C3 at `0x46`.
3. Read basic PMIC/fuel/charger status registers.
4. Confirm `PMIC_INT` edge/level behavior and EXTI routing.
5. Compare VBUS classification from ADP5360 and `PA9` VBUS divider/path.
6. Validate charging and charge-done reporting.
7. Validate low-battery threshold routes to forced sleep policy.
8. Validate critical-battery threshold disconnects ISOFET, not shipping mode.
9. Validate normal START short/long press remains firmware-observable before hardware shipping threshold.
10. Validate START hold warning/prep path can run before the ADP5360 shipping threshold.
11. Validate first-boot START shipping intent is ignored by firmware policy.
12. Validate PMIC read failure uses bounded recovery and does not silently trust stale power state.
13. Validate VBUS disagreement blocks installer/storage ownership decisions until explained.

---

## Evidence Requirements

Record in [[Brought_Up_Tracker]]:

- I2C probe log
- PMIC status readback
- PMIC interrupt observation
- VBUS cross-check result
- charging state result
- low-battery forced-sleep test result
- critical-battery ISOFET disconnect result if safely testable
- START / shipping-mode timing observations

Do not mark power behavior known-good without measured HW5 evidence.
