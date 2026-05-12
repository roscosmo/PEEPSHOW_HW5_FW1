# TMAG3001 Joystick Bring-up Runbook

This runbook records the measured HW5 procedure for bringing up the `TMAG3001A1YBGR` hall-effect joystick.

Related:

- [[Joystick_Hall_Input_Contract]]
- [[Brought_Up_Tracker]]
- [[Debug_Workflows]]

---

## Scope

This runbook covers:

- I2C probe at address `0x34`
- identity/status readback
- threshold interrupt configuration
- `JOY_INT` validation on `PC11` / `EXTI11`
- cardinal direction sampling
- calibration capture and validation
- safe-mode calibration fallback

---

## Preconditions

- `I2C3` bus on `PC0` SCL and `PC1` SDA validated
- `JOY_INT` on `PC11` / `EXTI11` mapped and interrupt path validated
- `thInput` owns joystick policy
- fallback controls are available for safe-mode calibration

---

## Baseline Datasheet Sequence

Initial bring-up uses absolute magnetic threshold detection, not wake-on-change.

Reason: TMAG3001 wake-on-change magnetic mode monitors only the first enabled magnetic axis according to `MAG_CH_EN`. A joystick must wake reliably from X or Y movement, so the baseline uses B-field thresholds across the required axes and then classifies direction from a bounded sample read.

Known datasheet values:

- I2C address: `0x34`
- `Device_ID`: `0x0D`
- `Manufacturer_ID_LSB`: `0x0E`, expected `0x49`
- `Manufacturer_ID_MSB`: `0x0F`, expected `0x54`
- `Device_Config_1`: `0x00`
- `Device_Config_2`: `0x01`
- `Sensor_Config_1`: `0x02`
- `Sensor_Config_2`: `0x03`
- `THR_Config_1`: `0x04`
- `THR_Config_2`: `0x05`
- `THR_Config_3`: `0x06`
- `Sensor_Config_3`: `0x07`
- `INT_Config_1`: `0x08`
- `Sensor_Config_4`: `0x09`
- `Sensor_Config_5`: `0x0A`
- `Sensor_Config_6`: `0x0B`
- X result: `0x12:0x13`
- Y result: `0x14:0x15`
- Z result: `0x16:0x17`
- `Conv_Status`: `0x18`
- `Device_Status`: `0x1C`

Baseline configuration policy:

- use `THR_SEL = 2h` for B-field thresholds
- keep `WOC_SEL = 0h`
- keep angle mode disabled for threshold wake
- use TMAG `INT` pin, not SCL interrupt mode
- use latched interrupt for bring-up so wake events are not missed
- leave exact threshold values, hysteresis, range, sleep interval, and axis mapping to measured calibration

---

## Command Sequence Ledger

Record exact transactions here once measured.

| Step | Operation | Register / Address | Value | Delay | Expected Readback | Notes |
|---|---|---|---|---|---|---|
| 1 | probe | `0x34` | I2C address transaction | TBD | ACK | use 7-bit/shifted HAL representation consistently |
| 2 | identity read | `Device_ID` / `0x0D` | read | TBD | version bits consistent with A1 device | record exact value |
| 3 | manufacturer ID read | `0x0E`, `0x0F` | read | TBD | `0x49`, `0x54` | confirms TI device family |
| 4 | status read/clear | `Conv_Status` / `0x18`, `Device_Status` / `0x1C` | read, clear W1C bits as needed | TBD | no unexpected faults | record POR, INT error, OTP CRC error |
| 5 | configure averaging/read mode | `Device_Config_1` / `0x00` | TBD | TBD | TBD | select bring-up averaging and read mode |
| 6 | configure range/polarity | `Sensor_Config_2` / `0x03` | TBD | TBD | TBD | range, `INTB_POL`, threshold direction |
| 7 | configure threshold type | `Sensor_Config_3` / `0x07` | `THR_SEL = 2h`, `WOC_SEL = 0h` | TBD | TBD | B-field thresholds, wake-on-change disabled |
| 8 | configure low thresholds | `THR_Config_1..3` / `0x04..0x06` | measured X/Y/Z low thresholds | TBD | TBD | values come from calibration/bring-up |
| 9 | configure high thresholds | `Sensor_Config_4..6` / `0x09..0x0B` | measured X/Y/Z high thresholds | TBD | TBD | valid when `THR_SEL = 2h` and angle mode disabled |
| 10 | configure INT output | `INT_Config_1` / `0x08` | `Threshold_INT = 1`, `INT_Mode = 1h`, latched | TBD | TBD | use TMAG `INT`; do not use SCL interrupt |
| 11 | enter threshold-armed mode | `Device_Config_2` / `0x01` and `Sensor_Config_1` / `0x02` | `Operating_Mode = 3h`, `MAG_CH_EN` and `SLEEPTIME` TBD | TBD | TBD | wake-up/sleep mode |
| 12 | bounded sample read | `0x12..0x18` | read X/Y/Z and status | TBD | `Result_Status = 1` when fresh | classify direction from calibration |
| 13 | interrupt clear validation | TMAG addressed by valid I2C access | read or write | TBD | `JOY_INT` deasserts | exact clear behavior must be measured |

---

## Validation Procedure

1. Probe `0x34` and record ACK/failure.
2. Read identity/status registers and record values.
3. Configure threshold interrupt behavior.
4. Confirm `JOY_INT` fires on joystick movement.
5. Read the device after interrupt and classify cardinal direction bits.
6. Confirm wake-on-change is not required for normal joystick wake.
7. Run center/range calibration and store calibration data.
8. Reboot with valid calibration and confirm normal input starts.
9. Reboot with missing/invalid calibration and confirm safe-mode calibration starts.
10. Validate encoder and L/R navigation in joystick safe mode.
11. Force or simulate an I2C/config fault and validate recovery or safe mode.

---

## Bring-Up Decisions To Measure

The following values must be measured on HW5 hardware and recorded before the joystick contract is considered implementation-ready:

- physical X/Y/Z sign mapping for the mounted sensor and magnet
- whether Z is needed for wake/classification or diagnostics only
- chosen `MAG_CH_EN` value
- selected X/Y and Z magnetic ranges
- neutral center and usable travel ranges
- threshold counts for each direction
- hysteresis setting that avoids chatter near center
- wake/sleep interval that balances response and current
- final `JOY_INT` polarity and CubeMX EXTI edge selection
- whether latched interrupt remains final policy or only bring-up policy

---

## Evidence

Every successful validation must link evidence from [[Brought_Up_Tracker]].

Do not mark joystick threshold wake, direction classification, or calibration known-good without measured HW5 evidence.
