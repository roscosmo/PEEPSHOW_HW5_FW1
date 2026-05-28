# LS013B7DH05 Display Bring-up Runbook

This runbook records the measured HW5 procedure for the Sharp Memory LCD path.

Related:

- [[Display_and_Rendering_Contract]]
- [[HW5_Pin_Ownership_Matrix]]
- [[HW5_DMA_Map]]
- [[HW5_Power_Rails]]
- [[Brought_Up_Tracker]]

---

## Scope

This runbook covers:

- `LS013B7DH05` panel validation
- logical landscape `168 x 144` mapping from native portrait `144 x 168`
- `SPI3` TX-only transfer path
- `PA15` display chip select behavior
- `PD2` `VLT_LCD` level-translator OE behavior
- `PC13` `LCD_1HZ` RTC calibration output for EXTCOMIN/VCOM
- full-frame updates
- partial updates
- SRAM4 DMA-safe display buffer placement
- low-power static hold behavior
- LPBAM display experiment evidence after baseline display validation

---

## CubeMX / Electrical Baseline

| Function | MCU resource | Required baseline |
| --- | --- | --- |
| Display bus | `SPI3` TX-only master | `PC10` SCK, `PC12` MOSI, `PA15` NSS |
| Display DMA | `LPDMA1_CH0`, `LPDMA1_REQUEST_SPI3_TX` | memory-to-peripheral display payload transfer |
| Level translator OE | `PD2` `VLT_LCD` | active-high TXU0104RUTR OE; low disables/high-Z, high enables |
| EXTCOMIN / VCOM | `PC13` `LCD_1HZ` | RTC 1 Hz calibration output |

Safe boot default is `VLT_LCD` low / disabled until `thDisplay` owns the display path. After display initialization, the default hold policy is `VLT_LCD` high / enabled so SPI and `LCD_1HZ` can reach the panel.

The display has no readback path. Every bring-up claim must come from waveform evidence, known pattern photos, framebuffer/payload logs, or current measurements.

---

## Baseline State Sequence

This sequence proves the normal renderer path before any LPBAM experiment.

1. Boot with `VLT_LCD` low and SPI3 idle.
2. Start the display owner and drive `VLT_LCD` high.
3. Confirm `LCD_1HZ` is present at the panel side of the level translator.
4. Send a full clear frame at conservative SPI speed.
5. Send full-frame black, white, checkerboard, border, vertical-line, and horizontal-line patterns.
6. Confirm logical landscape `168 x 144` maps correctly onto native portrait `144 x 168`.
7. Confirm pixel polarity, row order, byte order, and line address format.
8. Perform a partial update on a single line or small row range.
9. Perform a dirty-region update from framebuffer change tracking.
10. Confirm full-frame fallback triggers when dirty coverage exceeds the chosen threshold.
11. Confirm display payload DMA reads from the approved SRAM4 display buffer region.
12. Enter low-power/static hold with the image visible and `LCD_1HZ` maintained.
13. Resume and perform another partial update without stale transfer state.

If any of these fail, LPBAM display animation remains out of scope until the normal path is stable.

---

## Pattern / Mapping Ledger

Populate this table during bring-up. The current values are placeholders until measured on HW5 hardware.

| Test pattern | Purpose | Expected evidence | Measured result | Status |
| --- | --- | --- | --- | --- |
| all white | pixel polarity and clear path | uniform cleared display | TBD | open |
| all black | pixel polarity and full-frame fill | uniform filled display | TBD | open |
| checkerboard | byte order and adjacent pixel mapping | alternating pattern with no skew | TBD | open |
| border | logical edge mapping | visible border on all four logical edges | TBD | open |
| single pixel | coordinate transform | expected logical coordinate appears | TBD | open |
| vertical line | x-axis mapping | straight vertical line in landscape space | TBD | open |
| horizontal line | y-axis mapping | straight horizontal line in landscape space | TBD | open |
| small dirty row range | partial update | only target rows change | TBD | open |
| SRAM4 DMA source | buffer placement and DMA reachability | payload read from approved SRAM4 display buffer | TBD | open |
| static hold | low-power hold behavior | image remains visible; EXTCOMIN remains valid | TBD | open |

---

## LPBAM Experiment Gate

LPBAM is an intentional HW5 experiment, not the baseline renderer. Attempt it after the normal display sequence passes and before final sleep/wake integration is closed.

Required LPBAM experiment flow:

1. Build a tiny prevalidated sequence of two or more static idle-animation frames.
2. Convert the sequence into bounded SPI3/LPDMA/LPBAM payloads using the same verified row format as the normal renderer.
3. Place the sequence payloads in the approved SRAM4 display/LPBAM buffer region.
4. Confirm the scenario can run for a fixed window without CPU intervention.
5. Trigger wake/exit through input or RTC policy and prove `thDisplay` reclaims SPI3/LPDMA/LPBAM ownership.
6. Compare current draw against the normal RTC wake/partial-update idle strategy.
7. Record whether `VLT_LCD` must remain enabled continuously during the scenario.

Acceptance requires correct image output, clean ownership reclaim, correct EXTCOMIN behavior, and measured current benefit or a clearly documented reason to keep the path disabled.

---

## Command / Configuration Ledger

| Step | Configuration | Expected result | Measured result | Status |
| --- | --- | --- | --- | --- |
| safe boot | `VLT_LCD` low | translator disabled/high-Z | TBD | open |
| display enable | `VLT_LCD` high | translator enabled | TBD | open |
| EXTCOMIN | RTC 1 Hz calibration output | `LCD_1HZ` reaches panel while enabled | TBD | open |
| SPI baseline | conservative SPI3 TX | valid SCK/MOSI/NSS waveforms | TBD | open |
| full frame | clear/fill patterns | correct full-screen image | TBD | open |
| orientation | mapping patterns | logical `168 x 144` landscape confirmed | TBD | open |
| partial update | row/range payload | only target rows change | TBD | open |
| SRAM4 display buffer | approved SRAM4 DMA source | LPDMA reads correct payload without corruption/fault | TBD | open |
| static hold | low-power hold | image remains visible | TBD | open |
| suspend/resume | quiesce then update | no stale DMA/SPI state | TBD | open |
| LPBAM experiment | prevalidated SRAM4 animation payload | autonomous sequence and clean exit | TBD | open |
| fault injection | transfer/init fault | fatal display fault path entered | TBD | open |

---

## Validation Procedure

1. Confirm `VLT_LCD` active-high OE behavior: low disables TXU0104RUTR outputs, high enables outputs.
2. Confirm RTC 1 Hz calibration output appears on `LCD_1HZ` when display policy enables it.
3. Confirm SPI3 SCK/MOSI/NSS waveforms at bring-up speed.
4. Send known full-frame test pattern.
5. Validate logical landscape orientation and row/column mapping.
6. Validate clear, checkerboard, border, and single-pixel/line patterns.
7. Validate partial update of a small dirty rectangle or line range.
8. Validate display payload transfer from the approved SRAM4 display buffer region.
9. Validate static hold after MCU idle/sleep entry with `VLT_LCD` high/enabled and `LCD_1HZ` maintained.
10. Validate fault behavior when display init/transfer fails.
11. Attempt the LPBAM idle-animation experiment after full/partial/static-hold validation and before final sleep/wake integration closure.
12. Record whether optional `VLT_LCD` duty-cycling saves measurable current without harming image hold or EXTCOMIN behavior.

---

## Evidence Requirements

Record in [[Brought_Up_Tracker]]:

- logic capture or scope notes for SPI3 and EXTCOMIN
- photos of known display patterns
- orientation verification notes
- partial-update verification notes
- SRAM4 display buffer DMA reachability evidence
- low-power static-hold observation
- measured current for display hold if available
- LPBAM evidence: prevalidated SRAM4 sequence correctness, autonomous transfer window, wake/exit behavior, ownership reclaim, image correctness, and current comparison

Do not mark LPBAM display animation supported without measured scenario evidence. If it fails, normal display bring-up may still pass, but LPBAM remains unavailable.
