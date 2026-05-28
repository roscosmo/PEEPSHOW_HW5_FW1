# HW5 Clock Tree Contract

This note records the intended HW5 clock tree and the CubeMX baseline.

## Required Profiles

| Profile | Purpose | SYSCLK | Notes |
|---|---|---|---|
| CubeMX baseline | generated initial clock profile | 24 MHz MSI | Current `.ioc` baseline; not a final performance policy |
| Boot/init | deterministic early startup | 24 MHz target unless changed | must keep early bring-up simple |
| Low-power active | shell and light runtime work | 24 MHz baseline unless a measured lower profile is added | should maximize STOP residency and avoid unnecessary clock switching |
| Real-time active | frame-paced runtime work | 24 MHz baseline until a specific Engine/runtime budget requires more | Platform owns clock changes; higher profiles must be justified by measurement |
| USB installer | host transport mode | 24 MHz SYSCLK baseline with valid 48 MHz USB clock | USB clock must remain valid at 48 MHz |

## PLL2 Baseline

Current `.ioc` PLL2 configuration:

| Output | Frequency | Use |
|---|---|---|
| `PLL2P` | `4.096 MHz` | required SAI1 audio clock for 16 kHz mono playback |
| `PLL2Q` | `64 MHz` | current OCTOSPI1 kernel clock source |
| `PLL2R` | `256 MHz` | possible later OCTOSPI/performance source after validation |

## Kernel Clocks

| Peripheral | Kernel Clock | Profile Constraints | STOP/Resume Notes |
|---|---|---|---|
| SYSCLK / HCLK | MSI 24 MHz baseline | `fw1` reference `.ioc` has SYSCLK/HCLK 24 MHz; `fw0` should start with a conservative equivalent where practical | validate after STOP resume |
| RTC | external 32.768 kHz MEMS oscillator on `PC14` LSE input | must remain low-power safe | wake source authority lives in `thPower` |
| Display SPI3 | HSI 16 MHz kernel, SPI calculated 8 Mbit/s | no clock change during active SPI/LPDMA transfer | display owner must quiesce before STOP unless a validated LPBAM display scenario owns the transfer |
| Display EXTCOMIN | RTC 1 Hz calibration output on `PC13` | must remain valid while display holds image | coordinated by display/power policy |
| Audio SAI1 | `PLL2P = 4.096 MHz`, SAI real audio frequency `16.0 kHz` | required valid clock tree for 16 kHz mono, 16-bit output to MAX98357A | audio owner stops DMA before STOP |
| BBB LPTIM1 | reference CubeMX LPTIM path via `PB2` `BUZZ` | procedural BBB tone/sweep timing must be bounded | audio owner stops BUZZ output before STOP |
| External flash OCTOSPI1 | `PLL2Q = 64 MHz` kernel, `OCTOSPI1.ClockPrescaler = 8` | conservative bring-up baseline; later tuning may evaluate PLL2R-derived 128/256 MHz options only after reliable ID/read/write/erase | storage owner quiesces before STOP |
| Sensor/power bus I2C3 | HSI 16 MHz kernel | timing `0x00303D5B`; validate against bus speed target; hosts TMAG3001A1YBGR at `0x34`, LIS2DUX12TR at `0x18`, and ADP5360 at `0x46` | input/sensor/power owners validate bus after wake; IMU step counter should run in embedded logic without active MCU I2C |
| Light sensor ADC1 | 16 MHz ADC clock | sampled while awake; no ADC IRQ required yet | sensor owner controls `PHOT_EN` settle/sample/off |
| BLE LPUART1 | HSI 16 MHz kernel, 9600 baud baseline | interrupt/DMA policy deferred until BLE owner pass | comm owner quiesces UART before STOP |
| USB OTG FS | 48 MHz USB clock | USB installer mode blocks deep sleep as required | storage/power owners arbitrate attach/detach |

Related:

- [[Power_and_Sleep_Policy]]
- [[HW5_Hardware_Revision_Contract]]
- [[CubeMX_Configuration_Checklist]]
