# Sleep Wake Integration Bring-up Runbook

This runbook records measured HW5 sleep and wake behavior across Platform subsystems.

Related:

- [[Power_and_Sleep_Policy]]
- [[PMIC_and_Power_Contract]]
- [[HW5_Wake_Sources]]
- [[HW5_Power_Rails]]
- [[Display_and_Rendering_Contract]]
- [[Brought_Up_Tracker]]

---

## Scope

This runbook covers:

- RTC cadence wake
- button wake
- PMIC wake/notification
- joystick threshold wake
- IMU motion/step/gesture wake where enabled
- encoder wake only if explicitly armed and proven
- USB attach wake/installer transition
- display static hold across sleep
- owner quiesce/resume sequence
- unknown wake reason classification

---

## Integration Baseline

Sleep/wake bring-up is not a single peripheral test. It proves that all Platform owners can quiesce, enter the selected sleep class, wake for an allowed reason, restore clocks/timebases, and resume ownership without stale hardware state.

The first validated sleep class should be conservative. Deeper sleep classes are added only after wake reason classification and owner resume are reliable.

---

## Owner Quiesce Requirements

| Owner | Before sleep | After wake/resume |
| --- | --- | --- |
| `thPower` | choose sleep class, arm wake sources, record expected wake mask | restore clocks, classify wake, verify timebases |
| `thDisplay` | drain/abort active SPI/LPDMA flush; leave panel hold policy valid | revalidate display path before next flush |
| `thStorage` | block new operations; finish/abort read/program/erase; no USB export surprise | revalidate flash liveness if needed |
| `thAudio` | drain/stop SAI/DMA/LPTIM; `SD_MODE` low unless policy says otherwise | revalidate clocks, SAI, DMA, LPTIM before playback |
| `thInput` | arm only approved button/joystick/encoder wake sources | classify raw wake input and debounce post-wake |
| `thSensor` | place ALS off; configure IMU/TMAG modes according to policy | sample/revalidate sensors that caused wake |
| `thComm` | keep NINA off unless communication mode owns it | do not classify BLE as a wake source unless future policy changes |

Any owner timeout during quiesce is a failed sleep-entry test.

---

## Wake Source Matrix

| Wake source | Intended use | Required evidence |
| --- | --- | --- |
| RTC cadence | ambient display/runtime cadence | wake timestamp, timebase recovery, expected cadence |
| `BTN_START` | primary wake/sleep button | active-low wake classification and debounce |
| A/B/L/R | optional/contextual wake | only wakes when explicitly armed |
| `BTN_BOOT` | maintenance after app boot only | not normal runtime/game input |
| `PMIC_INT` | charger/battery/power events | EXTI15 event and PMIC status readback |
| USB VBUS attach | installer/transport policy | VBUS classification and storage policy handoff |
| TMAG3001 `JOY_INT` | threshold joystick wake | threshold-based wake and normalized post-wake sample |
| LIS2DUX12 `MPU_INT` | motion/gesture/event where enabled | event classification; step counter is normally polled |
| IMU step counter mode | background activity count | deepest sleep class that preserves embedded function state |
| rotary encoder | optional armed interaction wake | only when `ENC_WAKE_ARMED`; measured current impact |
| BLE/NINA | not a HW5 wake source by default | remains off/no wake unless future mode explicitly changes policy |

Unknown wake reasons are defects until explained.

---

## Baseline State Sequence

1. Boot to an awake low-power runtime with display initialized and storage/audio idle.
2. Record expected wake mask and selected sleep class.
3. Request quiesce from every Platform owner and record acknowledgements.
4. Enter the selected sleep class.
5. Wake by RTC and confirm wake reason, RTC continuity, HAL tick/RTOS timebase recovery, and owner resume.
6. Repeat with `BTN_START`.
7. Repeat with A/B/L/R only for policies where those buttons are armed.
8. Repeat with `PMIC_INT` using a safe charger/input event.
9. Repeat with USB VBUS attach and confirm installer/storage policy is not entered until storage ownership rules allow it.
10. Repeat with TMAG3001 threshold joystick wake once joystick threshold bring-up is complete.
11. Repeat with LIS2DUX12 event wake only for modes that intentionally arm IMU interrupt wake.
12. Validate IMU step-counter polling mode separately: no step interrupt wake required, but the sleep class must preserve embedded step counting.
13. Repeat with encoder wake only if `ENC_WAKE_ARMED` is selected and current draw is acceptable.
14. Force one unknown or disabled wake path, if practical, and confirm it logs as a defect rather than silently routing to normal wake.

---

## Runtime Class Measurement Matrix

Populate one row per tested sleep/runtime class.

| Runtime class | Sleep class | Armed wake sources | Average current | Wake latency | Resume latency | Failed wakes | Status |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `SHELL` | TBD | TBD | TBD | TBD | TBD | TBD | open |
| `LP_GRAPH` | TBD | TBD | TBD | TBD | TBD | TBD | open |
| `LP_MODULE` | TBD | TBD | TBD | TBD | TBD | TBD | open |
| `RT_SCENE` idle fallback | TBD | TBD | TBD | TBD | TBD | TBD | open |
| `INSTALLER` idle | TBD | USB/VBUS, B exit policy | TBD | TBD | TBD | TBD | open |
| IMU step counter active | TBD | RTC/poll cadence plus optional IMU policy | TBD | TBD | TBD | TBD | open |
| LPBAM display experiment | TBD | RTC/input/power/fault exit | TBD | TBD | TBD | TBD | open |

---

## Failure Injection

At least these failure cases should be tested before sleep/wake policy is considered stable:

| Failure | Expected behavior |
| --- | --- |
| display flush active during sleep request | sleep waits for flush completion or times out/faults |
| audio DMA active during sleep request | audio drains/stops before sleep or blocks sleep |
| flash erase/program active during sleep request | storage blocks sleep until operation completes or faults |
| unknown wake source | logged as defect with raw wake evidence |
| PMIC read failure after wake | bounded recovery; no stale battery state silently trusted |
| clock/timebase restore failure | boot/fault supervisor path, not normal runtime |

---

## Validation Procedure

1. Validate owner quiesce acknowledgements before sleep entry.
2. Validate RTC wake and timebase recovery.
3. Validate Start wake.
4. Validate optional A/B/L/R wake if enabled by policy.
5. Validate PMIC interrupt wake/notification.
6. Validate joystick threshold wake if configured.
7. Validate IMU wake and step-counter sleep floor separately.
8. Validate encoder wake only when `ENC_WAKE_ARMED` and selected sleep class supports it.
9. Validate USB attach enters installer/transport policy.
10. Validate display remains visible and EXTCOMIN/VCOM policy remains correct.
11. Validate unknown wake reasons are logged as defects.
12. Measure current for each sleep class/runtime class where practical.
13. Validate active audio, storage, display, and sensor operations correctly block or delay sleep entry.
14. Validate IMU step-counter mode chooses the deepest sleep class that preserves embedded step counting, not the absolute deepest sleep class.

---

## Evidence Requirements

Record in [[Brought_Up_Tracker]]:

- sleep class tested
- wake source tested
- wake latency
- resume latency
- current measurement if available
- wake reason classification
- failed wake/resume count
- display hold observation

Do not claim a wake source is supported until measured on HW5 hardware.
