# Brought-Up Tracker

Use this as the live progress tracker for HW5 hardware and firmware bring-up.

---

## Metadata

- Last updated: `YYYY-MM-DD`
- Board revision: `TBD`
- Firmware commit: `TBD`
- Maintainer: `TBD`

---

## Bring-Up Phase Status

| Phase | Status | Primary Runbooks | Notes |
|---|---|---|---|
| 0 - Arrival, power, and clock stability | Not started | [[HW5_Arrival_Phase0_Checklist]], [[ADP5360_Power_Bring-up_Runbook]], [[HW5_Clock_Tree_Contract]] | intake board safely, validate safe power, clocks, PMIC, reset causes |
| 1 - Display validation | Not started | [[LS013B7DH05_Display_Bring-up_Runbook]] | static image, EXTCOMIN, full/partial update, SRAM4 DMA buffer path |
| 2 - Storage validation | Not started | [[AT25SL128A_External_Flash_Bring-up_Runbook]] | ID, erase/program/readback, deep power-down |
| 3 - Audio validation | Not started | [[Audio_Output_Bring-up_Runbook]] | speaker, BBB, shutdown, DMA/mixer when available |
| 4 - Input and sensors | Not started | [[Button_Input_Bring-up_Runbook]], [[Rotary_Encoder_Bring-up_Runbook]], [[TEMT6000_Light_Sensor_Bring-up_Runbook]], [[LIS2DUX12_IMU_Bring-up_Runbook]], [[TMAG3001_Joystick_Bring-up_Runbook]] | validate physical input and normalized events |
| 5 - RTOS owner integration | Not started | [[RTOS_Ownership_and_Queue_Topology]], [[Subsystem_State_Machines]] | owner threads and queues match contracts |
| 6 - Sleep and wake validation | Not started | [[Sleep_Wake_Integration_Bring-up_Runbook]] | wake reason classification and measured current |
| 7 - Installer and transport mode | Not started | [[USB_MSC_Bring-up_and_Recovery_Runbook]], [[USB_Development_Mode_Contract]], [[NINA_B112_BLE_Bring-up_Runbook]] | USB MSC staging first; CDC developer personality is separate and mutually exclusive; BLE after comm owner exists |
| 8 - Runtime host lifecycle | Not started | [[Runtime_Host_Contract]], [[Package_Contract]] | only after hardware owners are stable |
| 9 - Platform freeze checks | Not started | [[Platform_Freeze_Charter]], [[HW5_Hardware_Documentation_Readiness]] | authority docs and measured evidence agree |
| 10 - Digital twin parity | Blocked | [[Digital_Twin_Host_Runtime_Contract]], [[Validation_Plan]] | blocked until HW5 Platform hardware validation and Platform contract behavior are proven |

---

## Digital Twin Sequencing Rule

The digital twin is not a bring-up substitute.

Do not implement or validate the host digital twin as a source of hardware truth before HW5 Platform hardware validation is complete.

Digital twin work may begin only after measured Platform behavior is recorded for the hardware/backend contracts it mirrors.

Digital twin evidence can validate package, Engine, runtime lifecycle, state graph, input, save, cadence, and cross-mode contract behavior.

Digital twin evidence cannot mark hardware behavior known-good. Hardware behavior is known-good only when measured on HW5 hardware and linked from this tracker.

---

## Evidence Ledger

Evidence artifacts should follow [[Evidence_Artifact_Convention]].

Measured constants promoted by an evidence entry should update [[Pending_Measured_Constants_Register]].

For each evidence entry record:

- date/time
- evidence ID
- test case ID
- mode/runtime class
- result
- artifacts
- command line or orchestration CLI invocation
- notes
- trace profile and Tracealyzer artifact path, if a trace snapshot was used
- telemetry capture artifact path and schema version, if dashboard telemetry was used
- current/power capture artifact path, instrument setup, and sync strategy, if PPK2 or equivalent measurement was used
- active tuning overlay and instrumentation state, if applicable

Template row:

| Date | Test Case | Mode/Host | Result | Artifact | Notes |
|---|---|---|---|---|---|
| YYYY-MM-DD | T-XXX | SHELL | PASS | path/to/log | |

---

## Temporary Measures Register

| ID | Introduced | Scope | Exit Criteria | Owner | Status |
|---|---|---|---|---|---|
| TMP-XXX | YYYY-MM-DD | TBD | TBD | TBD | active |

---

## Open Issues Blocking Completion

| ID | Blocking Phase | Summary | Owner | Next Action |
|---|---|---|---|---|
| BUG-XXX | Phase X | TBD | TBD | TBD |
