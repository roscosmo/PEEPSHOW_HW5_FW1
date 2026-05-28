/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ADP5360.h"
#include "LS013B7DH05.h"
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  uint32_t magic;
  uint32_t phase;
  uint32_t tick_start;
  uint32_t tick_end;
  uint8_t public_addr_7bit;
  uint8_t hal_addr_shifted;
  uint8_t ready_shifted_status;
  uint8_t ready_unshifted_status;
  uint8_t driver_init_status;
  uint32_t ready_shifted_error;
  uint32_t ready_unshifted_error;
  uint8_t raw_reg[55];
  uint8_t raw_reg_status[55];
  uint32_t raw_reg_error[55];
  uint16_t vbat_mv;
  uint16_t buck_target_mv;
  uint16_t buckboost_target_mv;
  uint8_t bat_soc_percent;
  uint8_t buck_config;
  uint8_t buckboost_config;
  uint8_t charger_status1;
  uint8_t charger_status2;
  uint8_t fault;
  uint8_t pgood_status;
  uint8_t read_count;
  uint8_t complete;
} PsPhase0bPmicProbe;

typedef struct
{
  uint32_t magic;
  uint32_t phase;
  uint32_t tick;
  uint32_t system_core_clock;
  uint32_t sysclk_hz;
  uint32_t hclk_hz;
  uint32_t pclk1_hz;
  uint32_t pclk2_hz;
  uint32_t pclk3_hz;
  uint32_t rcc_cr;
  uint32_t rcc_csr;
  uint32_t rcc_cfgr1;
  uint32_t rcc_cfgr2;
  uint32_t rcc_cfgr3;
  uint32_t rcc_icscr1;
  uint32_t pwr_vosr;
  uint8_t flag_oblrst;
  uint8_t flag_pinrst;
  uint8_t flag_borrst;
  uint8_t flag_sftrst;
  uint8_t flag_iwdgrst;
  uint8_t flag_wwdgrst;
  uint8_t flag_lpwrst;
  uint8_t complete;
} PsPhase0ResetClockProbe;

typedef struct
{
  uint32_t magic;
  uint32_t phase;
  uint32_t tick;
  uint32_t spi3_state;
  uint32_t rtc_state;
  uint32_t spi3_kernel_hz;
  uint32_t gpiod_odr;
  uint32_t gpiod_idr;
  uint8_t vlt_lcd_pin_state;
  uint8_t spi3_nssp_disabled;
  uint8_t spi3_lsb_first;
  uint8_t spi3_nss_active_high;
  uint8_t complete;
} PsPhase1DisplayBaselineProbe;

typedef struct
{
  uint32_t magic;
  uint32_t phase;
  uint32_t tick_start;
  uint32_t tick_last_tx;
  uint32_t frame_counter;
  uint32_t tx_ok_count;
  uint32_t tx_error_count;
  uint32_t last_hal_error;
  uint32_t payload_len;
  uint8_t last_fill_value;
  uint8_t vlt_lcd_pin_state;
  uint8_t last_tx_status;
  uint8_t complete;
} PsPhase1DisplayInvertProbe;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PS_PHASE0B_PROBE_MAGIC        (0x50304231UL)
#define PS_PHASE0B_PROBE_PHASE        (0x00000B00UL)
#define PS_ADP5360_PUBLIC_ADDR_7BIT   (0x46U)
#define PS_ADP5360_HAL_ADDR_SHIFTED   (PS_ADP5360_PUBLIC_ADDR_7BIT << 1)
#define PS_I2C_TIMEOUT_MS             (25U)
#define PS_ADP5360_REG_COUNT          (0x37U)
#define PS_ADP5360_REG_INT_FLAG1      (0x34U)
#define PS_ADP5360_REG_INT_FLAG2      (0x35U)
#define PS_PHASE0_CLOCK_MAGIC         (0x5030434CUL)
#define PS_PHASE0_CLOCK_PHASE         (0x00000C10UL)
#define PS_PHASE1_DISPLAY_MAGIC       (0x50314453UL)
#define PS_PHASE1_DISPLAY_PHASE       (0x00001000UL)
#define PS_PHASE1_DISPLAY_INVERT_MAGIC (0x50314449UL)
#define PS_PHASE1_DISPLAY_INVERT_PHASE (0x00001010UL)
#define PS_DISPLAY_SETTLE_MS          (5U)
#define PS_DISPLAY_FRAME_PERIOD_MS    (1000U)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c3;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi3;

/* USER CODE BEGIN PV */
volatile PsPhase0bPmicProbe g_ps_phase0b_pmic_probe;
volatile PsPhase0ResetClockProbe g_ps_phase0_reset_clock_probe;
volatile PsPhase1DisplayBaselineProbe g_ps_phase1_display_baseline_probe;
volatile PsPhase1DisplayInvertProbe g_ps_phase1_display_invert_probe;
static LS013B7DH05 g_ps_phase1_lcd;
static uint8_t g_ps_phase1_display_framebuffer[BUFFER_LENGTH];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_I2C3_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI3_Init(void);
/* USER CODE BEGIN PFP */
static void PS_Phase0B_RunPmicProbe(void);
static uint16_t PS_Phase0B_DecodeBuckTargetMv(uint8_t raw);
static uint16_t PS_Phase0B_DecodeBuckBoostTargetMv(uint8_t raw);
static void PS_Phase0_RecordResetClockProbe(void);
static void PS_Phase1_RecordDisplayBaselineProbe(void);
static void PS_Phase1_DisplayInvertStep(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_I2C3_Init();
  MX_RTC_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  PS_Phase0B_RunPmicProbe();
  PS_Phase0_RecordResetClockProbe();
  PS_Phase1_RecordDisplayBaselineProbe();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    PS_Phase1_DisplayInvertStep();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI
                              |RCC_OSCILLATORTYPE_MSIK;
  RCC_OscInitStruct.LSEState = RCC_LSE_BYPASS;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_1;
  RCC_OscInitStruct.MSIKClockRange = RCC_MSIKRANGE_4;
  RCC_OscInitStruct.MSIKState = RCC_MSIK_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x00506682;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_PrivilegeStateTypeDef privilegeState = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  privilegeState.rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO;
  privilegeState.backupRegisterPrivZone = RTC_PRIVILEGE_BKUP_ZONE_NONE;
  privilegeState.backupRegisterStartZone2 = RTC_BKP_DR0;
  privilegeState.backupRegisterStartZone3 = RTC_BKP_DR0;
  if (HAL_RTCEx_PrivilegeModeSet(&hrtc, &privilegeState) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable Calibration
  */
  if (HAL_RTCEx_SetCalibrationOutPut(&hrtc, RTC_CALIBOUTPUT_1HZ) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct = {0};

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_1LINE;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_LSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 0x7;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi3.Init.NSSPolarity = SPI_NSS_POLARITY_HIGH;
  hspi3.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi3.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi3.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi3.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi3.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi3.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi3.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi3.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP2_LPDMA_CH0_TCF_TRG;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if (HAL_SPIEx_SetConfigAutonomousMode(&hspi3, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(VLT_LCD_GPIO_Port, VLT_LCD_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : VLT_LCD_Pin */
  GPIO_InitStruct.Pin = VLT_LCD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(VLT_LCD_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
static void PS_Phase0B_RunPmicProbe(void)
{
  volatile PsPhase0bPmicProbe *probe = &g_ps_phase0b_pmic_probe;

  probe->magic = PS_PHASE0B_PROBE_MAGIC;
  probe->phase = PS_PHASE0B_PROBE_PHASE;
  probe->public_addr_7bit = PS_ADP5360_PUBLIC_ADDR_7BIT;
  probe->hal_addr_shifted = PS_ADP5360_HAL_ADDR_SHIFTED;
  probe->tick_start = HAL_GetTick();

  probe->ready_shifted_status = (uint8_t)HAL_I2C_IsDeviceReady(&hi2c3,
                                                               PS_ADP5360_HAL_ADDR_SHIFTED,
                                                               2,
                                                               PS_I2C_TIMEOUT_MS);
  probe->ready_shifted_error = HAL_I2C_GetError(&hi2c3);

  probe->ready_unshifted_status = (uint8_t)HAL_I2C_IsDeviceReady(&hi2c3,
                                                                 PS_ADP5360_PUBLIC_ADDR_7BIT,
                                                                 1,
                                                                 PS_I2C_TIMEOUT_MS);
  probe->ready_unshifted_error = HAL_I2C_GetError(&hi2c3);

  if (probe->ready_shifted_status == (uint8_t)HAL_OK)
  {
    probe->driver_init_status = (uint8_t)ADP5360_init();

    for (uint8_t reg = 0; reg < PS_ADP5360_REG_COUNT; reg++)
    {
      uint8_t value = 0U;
      HAL_StatusTypeDef status;

      if ((reg == PS_ADP5360_REG_INT_FLAG1) || (reg == PS_ADP5360_REG_INT_FLAG2))
      {
        continue;
      }

      status = ADP5360_read(reg, &value, 1U);
      probe->raw_reg[reg] = value;
      probe->raw_reg_status[reg] = (uint8_t)status;
      probe->raw_reg_error[reg] = HAL_I2C_GetError(&hi2c3);
      if (status == HAL_OK)
      {
        probe->read_count++;
      }
    }
  }

  probe->charger_status1 = probe->raw_reg[0x08U];
  probe->charger_status2 = probe->raw_reg[0x09U];
  probe->bat_soc_percent = probe->raw_reg[0x21U] & 0x7FU;
  uint16_t vbat_mv = 0U;
  (void)ADP5360_get_vbat(&vbat_mv, NULL);
  probe->vbat_mv = vbat_mv;
  probe->buck_config = probe->raw_reg[0x29U];
  uint16_t buck_target_mv = 0U;
  if (ADP5360_get_buck_vout(&buck_target_mv, NULL) == HAL_OK)
  {
    probe->buck_target_mv = buck_target_mv;
  }
  else
  {
    probe->buck_target_mv = PS_Phase0B_DecodeBuckTargetMv(probe->raw_reg[0x2AU]);
  }
  probe->buckboost_config = probe->raw_reg[0x2BU];
  uint16_t buckboost_target_mv = 0U;
  if (ADP5360_get_buckboost_vout(&buckboost_target_mv, NULL) == HAL_OK)
  {
    probe->buckboost_target_mv = buckboost_target_mv;
  }
  else
  {
    probe->buckboost_target_mv = PS_Phase0B_DecodeBuckBoostTargetMv(probe->raw_reg[0x2CU]);
  }
  uint8_t fault = 0U;
  (void)ADP5360_get_fault(&fault);
  probe->fault = fault;
  uint8_t pgood_status = 0U;
  (void)ADP5360_read_u8(ADP5360_REG_PGOOD_STATUS, &pgood_status);
  probe->pgood_status = pgood_status;
  probe->tick_end = HAL_GetTick();
  probe->complete = 1U;
}

static uint16_t PS_Phase0B_DecodeBuckTargetMv(uint8_t raw)
{
  return (uint16_t)(600U + ((uint16_t)(raw & 0x3FU) * 50U));
}

static uint16_t PS_Phase0B_DecodeBuckBoostTargetMv(uint8_t raw)
{
  uint8_t code = raw & 0x3FU;

  if (code <= 11U)
  {
    return (uint16_t)(1800U + ((uint16_t)code * 100U));
  }

  return (uint16_t)(2950U + ((uint16_t)(code - 12U) * 50U));
}

static void PS_Phase0_RecordResetClockProbe(void)
{
  volatile PsPhase0ResetClockProbe *probe = &g_ps_phase0_reset_clock_probe;

  probe->magic = PS_PHASE0_CLOCK_MAGIC;
  probe->phase = PS_PHASE0_CLOCK_PHASE;
  probe->tick = HAL_GetTick();
  probe->system_core_clock = SystemCoreClock;
  probe->sysclk_hz = HAL_RCC_GetSysClockFreq();
  probe->hclk_hz = HAL_RCC_GetHCLKFreq();
  probe->pclk1_hz = HAL_RCC_GetPCLK1Freq();
  probe->pclk2_hz = HAL_RCC_GetPCLK2Freq();
  probe->pclk3_hz = HAL_RCC_GetPCLK3Freq();
  probe->rcc_cr = RCC->CR;
  probe->rcc_csr = RCC->CSR;
  probe->rcc_cfgr1 = RCC->CFGR1;
  probe->rcc_cfgr2 = RCC->CFGR2;
  probe->rcc_cfgr3 = RCC->CFGR3;
  probe->rcc_icscr1 = RCC->ICSCR1;
  probe->pwr_vosr = PWR->VOSR;
  probe->flag_oblrst = (uint8_t)__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST);
  probe->flag_pinrst = (uint8_t)__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST);
  probe->flag_borrst = (uint8_t)__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST);
  probe->flag_sftrst = (uint8_t)__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST);
  probe->flag_iwdgrst = (uint8_t)__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST);
  probe->flag_wwdgrst = (uint8_t)__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST);
  probe->flag_lpwrst = (uint8_t)__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST);
  probe->complete = 1U;
}

static void PS_Phase1_RecordDisplayBaselineProbe(void)
{
  volatile PsPhase1DisplayBaselineProbe *probe = &g_ps_phase1_display_baseline_probe;

  probe->magic = PS_PHASE1_DISPLAY_MAGIC;
  probe->phase = PS_PHASE1_DISPLAY_PHASE;
  probe->tick = HAL_GetTick();
  probe->spi3_state = (uint32_t)HAL_SPI_GetState(&hspi3);
  probe->rtc_state = (uint32_t)HAL_RTC_GetState(&hrtc);
  probe->spi3_kernel_hz = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3);
  probe->gpiod_odr = GPIOD->ODR;
  probe->gpiod_idr = GPIOD->IDR;
  probe->vlt_lcd_pin_state = (uint8_t)HAL_GPIO_ReadPin(VLT_LCD_GPIO_Port, VLT_LCD_Pin);
  probe->spi3_nssp_disabled = (hspi3.Init.NSSPMode == SPI_NSS_PULSE_DISABLE) ? 1U : 0U;
  probe->spi3_lsb_first = (hspi3.Init.FirstBit == SPI_FIRSTBIT_LSB) ? 1U : 0U;
  probe->spi3_nss_active_high = (hspi3.Init.NSSPolarity == SPI_NSS_POLARITY_HIGH) ? 1U : 0U;
  probe->complete = 1U;
}

static void PS_Phase1_DisplayInvertStep(void)
{
  static uint8_t initialized = 0U;
  static uint8_t row_block_index = 0U;
  uint16_t rows[8];
  volatile PsPhase1DisplayInvertProbe *probe = &g_ps_phase1_display_invert_probe;
  HAL_StatusTypeDef status;

  if (initialized == 0U)
  {
    probe->magic = PS_PHASE1_DISPLAY_INVERT_MAGIC;
    probe->phase = PS_PHASE1_DISPLAY_INVERT_PHASE;
    probe->tick_start = HAL_GetTick();
    probe->payload_len = (uint32_t)(1U + (DISPLAY_HEIGHT * (1U + LINE_WIDTH + 1U)) + 1U);

    HAL_GPIO_WritePin(VLT_LCD_GPIO_Port, VLT_LCD_Pin, GPIO_PIN_SET);
    HAL_Delay(PS_DISPLAY_SETTLE_MS);
    status = LCD_Init(&g_ps_phase1_lcd, &hspi3);
    probe->last_tx_status = (uint8_t)status;
    probe->last_hal_error = HAL_SPI_GetError(&hspi3);
    if (status != HAL_OK)
    {
      probe->tx_error_count++;
      probe->complete = 1U;
      HAL_Delay(PS_DISPLAY_FRAME_PERIOD_MS);
      return;
    }
    initialized = 1U;
  }

  memset(g_ps_phase1_display_framebuffer, 0xFF, sizeof(g_ps_phase1_display_framebuffer));
  uint16_t first_row = (uint16_t)(1U + ((uint16_t)row_block_index * 8U));
  for (uint16_t i = 0U; i < 8U; i++)
  {
    rows[i] = (uint16_t)(first_row + i);
  }

  status = LCD_Clean(&g_ps_phase1_lcd);
  if (status == HAL_OK)
  {
    HAL_Delay(100U);
    status = LCD_FlushRows(&g_ps_phase1_lcd, g_ps_phase1_display_framebuffer, rows, 8U);
  }
  probe->tick_last_tx = HAL_GetTick();
  probe->last_fill_value = (uint8_t)first_row;
  probe->vlt_lcd_pin_state = (uint8_t)HAL_GPIO_ReadPin(VLT_LCD_GPIO_Port, VLT_LCD_Pin);
  probe->last_tx_status = (uint8_t)status;
  probe->last_hal_error = HAL_SPI_GetError(&hspi3);
  probe->frame_counter++;
  if (status == HAL_OK)
  {
    probe->tx_ok_count++;
  }
  else
  {
    probe->tx_error_count++;
  }
  probe->complete = 1U;

  row_block_index++;
  if (row_block_index >= 4U)
  {
    row_block_index = 0U;
  }
  HAL_Delay(PS_DISPLAY_FRAME_PERIOD_MS);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
