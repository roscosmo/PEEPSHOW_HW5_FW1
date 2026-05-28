# Brought-Up Tracker

Use this as the live progress tracker for HW5 hardware and firmware bring-up.

---

## Metadata

- Last updated: `2026-05-28`
- Board revision: `TBD`
- Firmware commit: `TBD`
- Maintainer: `TBD`

---

## Bring-Up Phase Status

| Phase | Status | Primary Runbooks | Notes |
|---|---|---|---|
| 0 - Arrival, power, and clock stability | In progress | [[HW5_Arrival_Phase0_Checklist]], [[ADP5360_Power_Bring-up_Runbook]], [[HW5_Clock_Tree_Contract]], [[FW0_Phased_CubeMX_Bring-up_Plan]], [[Bounded_Build_Flash_Debug_Runbook]] | `fw0` Phase 0A baseline built/flashed/attached; Phase 0B ADP5360 I2C probe ACK/readback succeeded; PMIC rail status and user DMM readings confirm `1.80 V` and `3.30 V` rails; keep phase open for measured current trace, reset-cause, and clock evidence |
| 1 - Display validation | In progress | [[LS013B7DH05_Display_Bring-up_Runbook]] | `fw0` display-minimum CubeMX baseline generated and no-display safe-state probe passed; user measured 1 Hz EXTCOMIN on `PC13`; user confirmed TXU0104 OE is active-high and `VLT_LCD` low had disabled the display translator during failed full-frame attempts; firmware and docs corrected to drive/record `VLT_LCD` high as enabled; full-frame visible pattern/pixel polarity, row order, and SRAM4 DMA buffer path remain pending |
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
| 2026-05-28 | PH0A-FW0-GEN-BUILD | `fw0` / bounded build | PASS | `firmware/peepshow_hw5_fw0/__fw0_build_stdout_2.txt`; `firmware/peepshow_hw5_fw0/__fw0_build_stderr_2.txt` | Active bring-up target `firmware/peepshow_hw5_fw0/peepshow_hw5_fw0.ioc`; bounded Debug build produced `build/Debug/peepshow_hw5_fw0.elf`; final no-op build returned `ninja: no work to do`; RAM `1584 B`, ROM `8344 B`, SRAM4 `0`; no leftover build processes. |
| 2026-05-28 | PH0A-FW0-FLASH | `fw0` / STLINK-V3MINIE | PASS | `firmware/peepshow_hw5_fw0/__fw0_flash_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_flash_stderr.txt` | Flashed `build/Debug/peepshow_hw5_fw0.elf` with `STM32_Programmer_CLI.exe -c port=SWD freq=4000 -w ... -v -rst`; verify and software reset reported; target voltage `1.80 V`; user reported PPK2/thermal/visual state looked good after reset, qualitative only, no numeric current trace recorded. |
| 2026-05-28 | PH0A-FW0-DEBUG-STATUS | `fw0` / bounded GDB attach | PASS | `firmware/peepshow_hw5_fw0/__fw0_gdb_stdout_2.txt`; `firmware/peepshow_hw5_fw0/__fw0_gdbserver_stdout_2.txt` | Read-only attach, register read, detach; no `continue`, stepping, reset, or resume. PC `0x08001fb8 <Reset_Handler>`, SP `0x200c0000`, XPSR `0x01000000`; target voltage `1.81 V`; no breakpoints/watchpoints; no leftover GDB/ST-Link processes. |
| 2026-05-28 | PH0B-ADP5360-I2C-PROBE | `fw0` / I2C3 + bounded GDB readback | PASS | `firmware/peepshow_hw5_fw0/__fw0_phase0b_build_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase0b_flash_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase0b_gdb_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase0b_gdbserver_stdout.txt` | I2C3 on `PC0`/`PC1`; probe struct `g_ps_phase0b_pmic_probe` showed public 7-bit address `0x46` ACKs when passed to STM32 HAL as shifted `0x8C`; unshifted `0x46` failed as expected; registers `0x00..0x0F` read successfully: `10 08 81 7a 29 2b 07 2c 00 00 00 56 8f 71 b4 00`; flash verify/reset passed; target voltage `1.79-1.80 V`; no leftover Programmer/GDB/ST-Link processes. |
| 2026-05-28 | PH0B-ADP5360-RAIL-STATUS-SNAPSHOT | `fw0` / I2C3 + bounded GDB readback + DMM | PASS | `firmware/peepshow_hw5_fw0/__fw0_phase0b_rail_probe_build_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase0b_rail_probe_flash_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase0b_rail_probe_gdb_stdout_3.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase0b_rail_probe_gdbserver_stdout_3.txt` | Read-only ADP5360 snapshot expanded to registers `0x00..0x36` except read-to-clear interrupt flags `0x34/0x35`; flash verify/reset passed; target voltage `1.79-1.81 V`; `PGOOD_STATUS=0x07` reported `VOUT1OK`, `VOUT2OK`, and `BATOK`; buck target decoded `1800 mV`, buck-boost target decoded `3300 mV`; user measured rails by multimeter at `1.80 V` and `3.30 V`, matching PMIC decoded targets; `BAT_SOC=0`, `VBAT_READ_H/L=00 00`, so PMIC fuel/ADC battery voltage is not usable evidence yet; first GDB command attempt failed due path quoting before target attach, rerun succeeded; no leftover Programmer/GDB/ST-Link processes. |
| 2026-05-28 | PH0-RESET-CLOCK-SNAPSHOT | `fw0` / bounded flash + GDB readback | PASS_WITH_LIMITATION | `firmware/peepshow_hw5_fw0/__fw0_phase0_reset_clock_build_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase0_reset_clock_flash_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase0_reset_clock_gdb_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase0_reset_clock_gdbserver_stdout.txt` | Added read-only `g_ps_phase0_reset_clock_probe`; bounded build passed with RAM `2120 B`, ROM `17344 B`, SRAM4 `0`; flash verify/reset passed, target voltage `1.81 V`; GDB readback showed `SystemCoreClock`, `SYSCLK`, `HCLK`, `PCLK1`, `PCLK2`, and `PCLK3` all `4000000 Hz`, matching intentional `fw0` Phase 0 4 MHz MSI bring-up baseline; reset flags showed `PINRST`, `BORRST`, and `SFTRST` set with watchdog/low-power flags clear, but flags were not cleared before a controlled reset-cause test, so this is accumulated reset evidence only; no breakpoints/watchpoints; no leftover Programmer/GDB/ST-Link processes. |
| 2026-05-28 | PH0B-ADP5360-DRIVER-INTEGRATION | `fw0` / imported driver + bounded flash/GDB | PASS | `firmware/peepshow_hw5_fw0/__fw0_adp5360_harness_build2_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_adp5360_driver_flash_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_adp5360_driver_gdb_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_adp5360_driver_gdbserver_stdout.txt` | Added imported `Core/Inc/ADP5360.h` and `Core/Src/ADP5360.c` to `fw0`; `ADP5360_init()` was constrained to read-only identity/revision reads by removing its `ADP5360_power_init(&ADP_cfg)` call; bounded build passed with RAM `2120 B`, ROM `18328 B`, SRAM4 `0`; flash verify/reset passed, target voltage `1.81 V`; GDB readback showed `driver_init_status=0`, public address `0x46` / HAL shifted `0x8C`, same register snapshot as raw Phase 0B probe, `fault=0x00`, `PGOOD_STATUS=0x07`, buck target `1800 mV`, buck-boost target `3300 mV`; no breakpoints/watchpoints; no leftover Programmer/GDB/ST-Link processes. |
| 2026-05-28 | PH1-DISPLAY-NO-PANEL-BASELINE | `fw0` / display-minimum CubeMX + bounded flash/GDB | PASS_WITH_LIMITATION | `firmware/peepshow_hw5_fw0/__fw0_phase1_display_baseline_probe_build_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase1_display_baseline_probe_flash_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase1_display_baseline_probe_gdb_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase1_display_baseline_probe_gdbserver_stdout.txt` | Display panel was not physically connected. `fw0` generated with SPI3 blocking baseline (`PC10` SCK, `PC12` MOSI, `PA15` hardware NSS active high, 1-line TX, LSB first, NSS pulse disabled, 4 MHz MSIK kernel / ~2 Mbit/s), RTC/LSE bypass `PC13` 1 Hz calibration output path, and `PD2` `VLT_LCD` push-pull output default high. Later datasheet check confirmed `VLT_LCD`/TXU0104 OE is active-high, so pin state `1` means translator enabled, not disabled. Bounded build passed with RAM `2352 B`, ROM `28328 B`, SRAM4 `0`; flash verify/reset passed, target voltage `1.79 V`; GDB readback showed ADP5360 still ACK/readback OK after I2C timing/clock change, `VLT_LCD` ODR/IDR `0x4` and pin state `1`, SPI3 kernel `4000000 Hz`, `spi3_nssp_disabled=1`, `spi3_lsb_first=1`, `spi3_nss_active_high=1`; no display transfer was attempted; no leftover Programmer/GDB/ST-Link processes. |
| 2026-05-28 | PH1-DISPLAY-EXTCOMIN-CONNECTOR | `fw0` / DMM at display connector | PASS | `firmware/peepshow_hw5_fw0/__fw0_phase1_display_baseline_probe_gdb_stdout.txt` | With display-minimum baseline firmware running, user measured the RTC 1 Hz EXTCOMIN/LCD_1HZ signal reaching the display connector by multimeter. This confirms the 1 Hz path through the enabled measurement setup at connector level; scope waveform quality, panel-side behavior with display physically connected, and image hold behavior remain pending. |
| 2026-05-28 | PH1-DISPLAY-CONNECTED-INVERT-ATTEMPT | `fw0` / connected panel + blocking SPI3 fill-frame loop | FAIL_ROOT_CAUSED | `firmware/peepshow_hw5_fw0/__fw0_phase1_display_invert_build_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase1_display_invert_flash_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase1_display_invert_status_gdb_stdout_2.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase1_display_invert_status_gdbserver_stdout_2.txt` | User connected the display panel and reported no visible display response and nothing noteworthy on PPK2. `fw0` blocking invert test drove `VLT_LCD` low and alternated full-screen `0x00`/`0xFF` payloads once per second using SPI3 blocking transmit only, no DMA/renderer/SRAM4/LPBAM. Later TXU0104 datasheet check and user scope/DMM observations confirmed `VLT_LCD`/OE is active-high, so this firmware disabled the level translator during the attempted update. Bounded build passed with RAM `5760 B`, ROM `30304 B`, SRAM4 `0`; flash verify/reset passed, target voltage `1.80 V`; bounded GDB readback showed `g_ps_phase1_display_invert_probe.frame_counter=31`, `tx_ok_count=31`, `tx_error_count=0`, `last_tx_status=0`, `last_hal_error=0`, payload length `0x0d22` (`3362`) and `vlt_lcd_pin_state=0`. SPI/NSS/MOSI/SCK panel-side behavior with active-high OE remains pending. |
| 2026-05-28 | PH1-DISPLAY-CLEAR-COMMAND | `fw0` / imported LS013B7DH05 driver adapted for SPI3 hardware NSS | PASS_WITH_LIMITATION | `firmware/peepshow_hw5_fw0/__fw0_phase1_display_driver_hwnss_build_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase1_display_driver_hwnss_flash_stdout.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase1_display_driver_hwnss_status_gdb_stdout_2.txt`; `firmware/peepshow_hw5_fw0/__fw0_phase1_display_driver_hwnss_status_gdbserver_stdout_2.txt` | User reported the connected display cleared from static noise after flashing the hardware-NSS driver build. This is retained as observed panel evidence, but later TXU0104 polarity validation showed the follow-on loop drove `VLT_LCD` low, which disables panel-side signals. Bounded flash verify/reset passed at target voltage `1.80 V`; bounded GDB readback showed the driver loop executing with `frame_counter=43`, `tx_ok_count=43`, `tx_error_count=0`, `last_tx_status=0`, `last_hal_error=0`, and `VLT_LCD` pin state `0` after initialization. Full-frame visible pattern with `VLT_LCD` high/enabled, pixel polarity, row order, and static hold remain pending. |
| 2026-05-28 | PH1-DISPLAY-VLT-OE-POLARITY | `fw0` / TXU0104 datasheet check + user probing | PASS | `docs/07 Hardware/HW5_Power_Rails.md`; `docs/01 Platform/Rendering/Display_and_Rendering_Contract.md`; `firmware/peepshow_hw5_fw0/__fw0_phase1_display_vlt_active_high_build_stdout.txt` | User checked the TXU0104 datasheet and measured `PD2` low while `PC13` still toggled 1 Hz on the MCU side. Datasheet confirms OE is active-high: `VLT_LCD` high enables level translation and low places outputs high-Z. Firmware display loop was corrected to drive `VLT_LCD` high before `LCD_Init()`/`LCD_FlushAll()`. Relevant display hardware contracts/runbooks were corrected from active-low to active-high. Bounded build passed with RAM `8792 B`, ROM `33962 B`, SRAM4 `0`; flash and visible pattern retest remain pending. |

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
