# CubeMX Configuration Checklist

Use this checklist before generating the HW5 firmware base.

During HW5 hardware bring-up, use [[FW0_Phased_CubeMX_Bring-up_Plan]] to decide which features are allowed in the active `fw0` CubeMX project. The full `fw1` CubeMX project is reference/full-intent only until promotion.

## Before Generation

- [[HW5_Pin_Ownership_Matrix]] is filled enough to assign pins.
- [[HW5_Clock_Tree_Contract]] defines required clock profiles and kernel clock assumptions.
- [[HW5_DMA_Map]] defines expected DMA paths.
- [[HW5_Power_Rails]] defines safe boot defaults.
- [[HW5_Wake_Sources]] defines wake-capable EXTI and RTC paths.

## After Generation

Verify the generated `.ioc` and code against:

- pin ownership
- alternate functions
- interrupt priorities
- DMA request mappings
- peripheral kernel clocks
- GPIO initial output states
- RTC and low-power settings
- USB device settings
- ThreadX/Cube middleware settings

## Current HW5 Checks From `fw1` Reference `.ioc`

- `BTN_BOOT` label is fixed on `PH3-BOOT0`; BOOT0 can force ROM bootloader before application firmware runs.
- `PC14` is LSE external clock input from a 32.768 kHz MEMS oscillator, not a crystal.
- `PC12` is `SPI3_MOSI`.
- EXTI NVIC entries are enabled for `EXTI3`, `EXTI4`, `EXTI5`, `EXTI6`, `EXTI7`, `EXTI8`, `EXTI11`, `EXTI14`, and `EXTI15`.
- `TIM2_IRQn` is enabled for the rotary encoder activity path.
- ADC interrupt is intentionally disabled for now.
- LPUART DMA is intentionally deferred. BLE owner implementation should start with interrupt-driven RX/TX static ring buffers before any DMA path is considered.
- USBX MSC, FileX, LevelX, ThreadX, ICACHE, LPBAM, GPDMA1, and LPDMA1 are present in the CubeMX project.
- ADP5360 uses I2C address `0x46`, with `PMIC_INT` on `PB15` / `EXTI15` and VBUS cross-check through `PA9`.
- LIS2DUX12TR uses I2C address `0x18`, with `MPU_INT` on `PB14` / `EXTI14`; step-counter mode requires a power-policy sleep floor.
- TMAG3001A1YBGR uses I2C address `0x34`, with `JOY_INT` on `PC11` / `EXTI11`; threshold mode and calibration safe mode must be validated.
- NINA-B112-04B uses LPUART1 with RTS/CTS; no BLE wake path is assigned; `NINA_SW1`, `NINA_SW2`, `NINA_DTR`, and `NINA_DSR` are now `GPIO_Analog` safe defaults in the `.ioc`.
- SAI1 uses `PLL2P = 4.096 MHz` and reports real audio frequency `16.0 kHz`.
- OCTOSPI1 uses `PLL2Q = 64 MHz` kernel with `OCTOSPI1.ClockPrescaler = 8`.

## Required GPIO Initial-State Checks

- `VLT_LCD` / `PD2`: TXU0104RUTR OE is active low. Generated firmware must default this output high/disabled until `thDisplay` intentionally drives it low/enabled.
- `PHOT_EN` / `PC2`: active high. Generated firmware default must be low/off.
- `ENC_EN` / `PB4`: active high. Generated firmware default must be low/off.
- `NINA_NRST` / `PC6`: active low. BLE-off policy asserts reset low until `thComm` brings the module online.
- `NINA_SW1` / `PC4` and `NINA_SW2` / `PC5`: should be configured as high-Z input or analog/no-pull in CubeMX unless intentionally using a NINA startup/control function.
- `NINA_DTR` / `PC7` and `NINA_DSR` / `PC8`: should be configured as high-Z input or analog/no-pull in CubeMX until their NINA configuration, direction, and polarity are proven.
- `SD_MODE` / `PC9`: low shuts down MAX98357A.

## Current Watch Items

- Confirm `NINA_SW1`, `NINA_SW2`, `NINA_DTR`, and `NINA_DSR` remain `GPIO_Analog` high-Z/no-pull defaults after future CubeMX edits; `thComm` reconfigures them only when a validated NINA mode needs output drive.
- Confirm generated GPIO init order cannot release `NINA_NRST` while `NINA_SW1`/`NINA_SW2` are low or output-driven; `fw1` reference `.ioc` analog defaults avoid this, and `fw0` should preserve the same safe behavior when BLE pins are introduced.
- Confirm `VLT_LCD` active-low OE initialization drives high/disabled before `thDisplay` starts.
- Confirm `BTN_BOOT` / BOOT0 electrical behavior and enclosure access cannot cause accidental ROM bootloader entry.
- OCTOSPI1 performance tuning may be considered later using PLL2R-derived 128/256 MHz options only after conservative bring-up passes.

## Rule

Do not treat CubeMX defaults as architecture.

CubeMX is a generator. The vault is the architectural source of truth.
