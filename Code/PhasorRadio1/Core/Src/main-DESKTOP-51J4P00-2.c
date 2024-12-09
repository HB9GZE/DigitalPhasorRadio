/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "app_touchgfx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32h735g_discovery_ospi.h"
#include "stm32h7xx_hal_ospi.h"
#include "si5351.h"
#include "semphr.h"
#include "stdio.h"
#include "queue.h"
#include "task.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifndef FALSE     /* in case these macros already exist */
#define FALSE 0   /* values of boolean */
#endif
#ifndef TRUE
#define TRUE  1
#endif
#define ARM_MATH_CM7
#define INPUT_SAMPLES 4096
#define BAND_80M 80
#define BAND_40M 40
#define BAND_20M 20
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

CRC_HandleTypeDef hcrc;

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c4;

LTDC_HandleTypeDef hltdc;

OSPI_HandleTypeDef hospi1;
OSPI_HandleTypeDef hospi2;

TIM_HandleTypeDef htim23;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes =
{ .name = "defaultTask", .stack_size = 128 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for TouchGFXTask */
osThreadId_t TouchGFXTaskHandle;
const osThreadAttr_t TouchGFXTask_attributes =
{ .name = "TouchGFXTask", .stack_size = 1024 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for myTaskToGUI */
osThreadId_t myTaskToGUIHandle;
const osThreadAttr_t myTaskToGUI_attributes =
{ .name = "myTaskToGUI", .stack_size = 1200 * 4, .priority = (osPriority_t) osPriorityLow, };
/* Definitions for myTaskFromGui */
osThreadId_t myTaskFromGuiHandle;
const osThreadAttr_t myTaskFromGui_attributes =
{ .name = "myTaskFromGui", .stack_size = 1200 * 4, .priority = (osPriority_t) osPriorityLow, };
/* Definitions for myTimerI2CWindowA */
osTimerId_t myTimerI2CWindowAHandle;
const osTimerAttr_t myTimerI2CWindowA_attributes =
{ .name = "myTimerI2CWindowA" };
/* Definitions for myTimerI2CWindowB */
osTimerId_t myTimerI2CWindowBHandle;
const osTimerAttr_t myTimerI2CWindowB_attributes =
{ .name = "myTimerI2CWindowB" };
/* Definitions for i2cMutex */
osMutexId_t i2cMutexHandle;
const osMutexAttr_t i2cMutex_attributes =
{ .name = "i2cMutex" };
/* USER CODE BEGIN PV */
//config SI5351
uint8_t phaseOffset;
const int32_t correction = 200;
si5351PLLConfig_t pllConf;
si5351OutputConfig_t outConf;
int32_t fclk = 7074000;
//VFO settings
float vfo_80m = 3573.0;
float vfo_40m = 7074.0;
float vfo_20m = 14074.0;
float vfo_80m_dec = 0; //_dec for handling the fine tuning (with 1/10th decimal fraction)
float vfo_40m_dec = 0; //_dec for handling the fine tuning (with 1/10th decimal fraction)
float vfo_20m_dec = 0; //_dec for handling the fine tuning (with 1/10th decimal fraction)
uint32_t vfo_80m_5351 = 3573000;
uint32_t vfo_40m_5351 = 7074000;
uint32_t vfo_20m_5351 = 14074000;
uint8_t VFOhasChangedforDisplay;
uint8_t VFOhasChangedforSI5351;
uint8_t currentBand = BAND_40M;
uint8_t sMeterTestData;
uint8_t swapIQ = 0;
uint8_t swapIQhasChanged = 0;
uint8_t stateRXTX = 0; //0 = RX
uint8_t stateRXTXhasChanged = 0;
uint8_t stateBand = 40;
uint8_t stateBandhasChanged = 0;
uint8_t stateLSBUSB = 0; // 0 = LSB
uint8_t stateLSBUSBhasChanged = 0;
uint32_t stepSize = 1000;
uint8_t rotaryEncoderCounter;
int8_t rotaryEncoderValue;
uint8_t readyToSendI2C;
uint8_t startTimerA;
//FFT
float hanningWin[INPUT_SAMPLES];
float fftInputBuffer[INPUT_SAMPLES];
float fftOutputBuffer[INPUT_SAMPLES];
float fftOutputMagnitudeBuffer[INPUT_SAMPLES / 2];
int adcInputBuffer[INPUT_SAMPLES];
int dynamicGraphValue[300];
int sMeterAverageValue[10];
struct dataToView_t
{
	int dynGraphData[300];
	int sMeterData;
	float vfoData;
} viewData;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_DMA2D_Init(void);
static void MX_OCTOSPI1_Init(void);
static void MX_OCTOSPI2_Init(void);
static void MX_TIM23_Init(void);
static void MX_I2C4_Init(void);
static void MX_LTDC_Init(void);
void StartDefaultTask(void *argument);
extern void TouchGFX_Task(void *argument);
void StartTaskToGui(void *argument);
void StartTaskFromGui(void *argument);
void myTimerCallbackA(void *argument);
void myTimerCallbackB(void *argument);

/* USER CODE BEGIN PFP */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void sendDatatoSI5351(void);
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_8)
	{
		if (stepSize == 1000)
		{
			stepSize = 100;
			switch (currentBand)
			{
			case BAND_80M:
				vfo_80m_5351 = (vfo_80m_5351 / 100) * 100;
				vfo_80m = (float) (vfo_80m_5351) / 1000.0;
				break;
			case BAND_40M:
				vfo_40m_5351 = (vfo_40m_5351 / 100) * 100;
				vfo_40m = (float) (vfo_40m_5351) / 1000.0;
				break;
			case BAND_20M:
				vfo_20m_5351 = (vfo_20m_5351 / 100) * 100;
				vfo_20m = (float) (vfo_20m_5351) / 1000.0;
				break;
			}
		}
		else if (stepSize == 100)
		{
			stepSize = 10;
			switch (currentBand)
			{
			case BAND_80M:
				vfo_80m_5351 = (vfo_80m_5351 / 10) * 10;
				vfo_80m = (float) (vfo_80m_5351) / 1000.0;
				break;
			case BAND_40M:
				vfo_40m_5351 = (vfo_40m_5351 / 10) * 10;
				vfo_40m = (float) (vfo_40m_5351) / 1000.0;
				break;
			case BAND_20M:
				vfo_20m_5351 = (vfo_20m_5351 / 10) * 10;
				vfo_20m = (float) (vfo_20m_5351) / 1000.0;
				break;
			}
		}
		else if (stepSize == 10)
		{
			stepSize = 1000;
			switch (currentBand)
			{
			case BAND_80M:
				vfo_80m_5351 = (vfo_80m_5351 / 1000) * 1000;
				vfo_80m = (float) (vfo_80m_5351) / 1000.0;
				break;
			case BAND_40M:
				vfo_40m_5351 = (vfo_40m_5351 / 1000) * 1000;
				vfo_40m = (float) (vfo_40m_5351) / 1000.0;
				break;
			case BAND_20M:
				vfo_20m_5351 = (vfo_20m_5351 / 1000) * 1000;
				vfo_20m = (float) (vfo_20m_5351) / 1000.0;
				break;
			}
		}
	}
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if ((htim == &htim23) && (VFOhasChangedforSI5351 == FALSE))
	{
		rotaryEncoderCounter++;
		sMeterTestData++;
		printf("The S Meter Data: %d \n", sMeterTestData);

		if (sMeterTestData >= 11)
			sMeterTestData = 0;

		if (rotaryEncoderCounter == 2)
		{
			if (__HAL_TIM_IS_TIM_COUNTING_DOWN(
					&htim23) && VFOhasChangedforSI5351 == FALSE)
			{
				switch (currentBand)
				{
				case BAND_80M:
					vfo_80m_5351 = vfo_80m_5351 - stepSize;
					break;
				case BAND_40M:
					vfo_40m_5351 = vfo_40m_5351 - stepSize;
					break;
				case BAND_20M:
					vfo_20m_5351 = vfo_20m_5351 - stepSize;
					break;
				}
			}
			else
			{
				switch (currentBand)
				{
				case BAND_80M:
					vfo_80m_5351 = vfo_80m_5351 + stepSize;
					break;
				case BAND_40M:
					vfo_40m_5351 = vfo_40m_5351 + stepSize;
					break;
				case BAND_20M:
					vfo_20m_5351 = vfo_20m_5351 + stepSize;
					break;
				}
			}

			switch (currentBand)
			{
			case BAND_80M:
				vfo_80m = (float) (vfo_80m_5351) / 1000.0;
				break;
			case BAND_40M:
				vfo_40m = (float) (vfo_40m_5351) / 1000.0;
				break;
			case BAND_20M:
				vfo_20m = (float) (vfo_20m_5351) / 1000.0;
				break;
			}

			VFOhasChangedforSI5351 = TRUE;
			VFOhasChangedforDisplay = TRUE;
			rotaryEncoderCounter = 0;
		}
	}
}

void sendDatatoSI5351(void)
{
	if ((VFOhasChangedforSI5351 == TRUE) && (readyToSendI2C == TRUE))
	{
		//xSemaphoreTake(i2cMutexHandle, (TickType_t ) 10);
		switch (currentBand)
		{
		case BAND_80M:
			si5351_CalcIQ(vfo_80m_5351, &pllConf, &outConf);
			break;
		case BAND_40M:
			si5351_CalcIQ(vfo_40m_5351, &pllConf, &outConf);
			break;
		case BAND_20M:
			si5351_CalcIQ(vfo_20m_5351, &pllConf, &outConf);
			break;
		}
		phaseOffset = (uint8_t) outConf.div;
		si5351_SetupOutput(0, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_4MA, &outConf, 0);
		si5351_SetupOutput(2, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_4MA, &outConf, phaseOffset);
		si5351_SetupPLL(SI5351_PLL_A, &pllConf);
		si5351_EnableOutputs((1 << 0) | (1 << 2));
		VFOhasChangedforSI5351 = FALSE;
		//xSemaphoreGive(i2cMutexHandle);
	}
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MPU Configuration--------------------------------------------------------*/
	MPU_Config();

	/* Enable the CPU Cache */

	/* Enable I-Cache---------------------------------------------------------*/
	SCB_EnableICache();

	/* Enable D-Cache---------------------------------------------------------*/
	SCB_EnableDCache();

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* Configure the peripherals common clocks */
	PeriphCommonClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_CRC_Init();
	MX_DMA2D_Init();
	MX_OCTOSPI1_Init();
	MX_OCTOSPI2_Init();
	MX_TIM23_Init();
	MX_I2C4_Init();
	MX_LTDC_Init();
	MX_TouchGFX_Init();
	/* Call PreOsInit function */
	MX_TouchGFX_PreOSInit();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Encoder_Start_IT(&htim23, TIM_CHANNEL_ALL);

	vfo_40m_5351 = 7074000;
	//set up I/Q
	si5351_Init(correction);
	si5351_CalcIQ(vfo_40m_5351, &pllConf, &outConf);

	/*
	 * `phaseOffset` is a 7bit value, calculated from Fpll, Fclk and desired phase shift.
	 * To get N° phase shift the value should be round( (N/360)*(4*Fpll/Fclk) )
	 * Two channels should use the same PLL to make it work. There are other restrictions.
	 * Please see AN619 for more details.
	 *
	 * si5351_CalcIQ() chooses PLL and MS parameters so that:
	 *   Fclk in [1.4..100] MHz
	 *   out_conf.div in [9..127]
	 *   out_conf.num = 0
	 *   out_conf.denum = 1
	 *   Fpll = out_conf.div * Fclk.
	 * This automatically gives 90° phase shift between two channels if you pass
	 *  0 and out_conf.div as a phaseOffset for these channels.
	 */
	phaseOffset = (uint8_t) outConf.div;
	//si5351_SetupOutput(0, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_4MA, &outConf, 0);
	//si5351_SetupOutput(2, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_4MA, &outConf,phaseOffset);
	/*
	 * The order is important! Setup the channels first, then setup the PLL.
	 * Alternatively you could reset the PLL after setting up PLL and channels.
	 * However since _SetupPLL() always resets the PLL this would only cause
	 * sending extra I2C commands.
	 */
	si5351_SetupOutput(0, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_4MA, &outConf, 0);
	si5351_SetupOutput(2, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_4MA, &outConf, phaseOffset);
	si5351_SetupPLL(SI5351_PLL_A, &pllConf);
	si5351_EnableOutputs((1 << 0) | (1 << 2));

	/* USER CODE END 2 */

	/* Init scheduler */
	osKernelInitialize();
	/* Create the mutex(es) */
	/* creation of i2cMutex */
	i2cMutexHandle = osMutexNew(&i2cMutex_attributes);

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* Create the timer(s) */
	/* creation of myTimerI2CWindowA */
	myTimerI2CWindowAHandle = osTimerNew(myTimerCallbackA, osTimerOnce, NULL, &myTimerI2CWindowA_attributes);

	/* creation of myTimerI2CWindowB */
	myTimerI2CWindowBHandle = osTimerNew(myTimerCallbackB, osTimerOnce, NULL, &myTimerI2CWindowB_attributes);

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of defaultTask */
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

	/* creation of TouchGFXTask */
	TouchGFXTaskHandle = osThreadNew(TouchGFX_Task, NULL, &TouchGFXTask_attributes);

	/* creation of myTaskToGUI */
	myTaskToGUIHandle = osThreadNew(StartTaskToGui, NULL, &myTaskToGUI_attributes);

	/* creation of myTaskFromGui */
	myTaskFromGuiHandle = osThreadNew(StartTaskFromGui, NULL, &myTaskFromGui_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct =
	{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
	{ 0 };

	/** Supply configuration update enable
	 */
	HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
	{
	}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 2;
	RCC_OscInitStruct.PLL.PLLN = 44;
	RCC_OscInitStruct.PLL.PLLP = 1;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1
			| RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct =
	{ 0 };

	/** Initializes the peripherals clock
	 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
	PeriphClkInitStruct.PLL2.PLL2M = 5;
	PeriphClkInitStruct.PLL2.PLL2N = 80;
	PeriphClkInitStruct.PLL2.PLL2P = 2;
	PeriphClkInitStruct.PLL2.PLL2Q = 2;
	PeriphClkInitStruct.PLL2.PLL2R = 2;
	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
	PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
	PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_PLL2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief CRC Initialization Function
 * @param None
 * @retval None
 */
static void MX_CRC_Init(void)
{

	/* USER CODE BEGIN CRC_Init 0 */

	/* USER CODE END CRC_Init 0 */

	/* USER CODE BEGIN CRC_Init 1 */

	/* USER CODE END CRC_Init 1 */
	hcrc.Instance = CRC;
	hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
	hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
	hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
	hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
	hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
	if (HAL_CRC_Init(&hcrc) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN CRC_Init 2 */

	/* USER CODE END CRC_Init 2 */

}

/**
 * @brief DMA2D Initialization Function
 * @param None
 * @retval None
 */
static void MX_DMA2D_Init(void)
{

	/* USER CODE BEGIN DMA2D_Init 0 */

	/* USER CODE END DMA2D_Init 0 */

	/* USER CODE BEGIN DMA2D_Init 1 */

	/* USER CODE END DMA2D_Init 1 */
	hdma2d.Instance = DMA2D;
	hdma2d.Init.Mode = DMA2D_R2M;
	hdma2d.Init.ColorMode = DMA2D_OUTPUT_RGB888;
	hdma2d.Init.OutputOffset = 0;
	if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN DMA2D_Init 2 */

	/* USER CODE END DMA2D_Init 2 */

}

/**
 * @brief I2C4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C4_Init(void)
{

	/* USER CODE BEGIN I2C4_Init 0 */

	/* USER CODE END I2C4_Init 0 */

	/* USER CODE BEGIN I2C4_Init 1 */

	/* USER CODE END I2C4_Init 1 */
	hi2c4.Instance = I2C4;
	hi2c4.Init.Timing = 0x20B0C8FF;
	hi2c4.Init.OwnAddress1 = 0;
	hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c4.Init.OwnAddress2 = 0;
	hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c4) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Analogue filter
	 */
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_DISABLE) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Digital filter
	 */
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 0) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN I2C4_Init 2 */

	/* USER CODE END I2C4_Init 2 */

}

/**
 * @brief LTDC Initialization Function
 * @param None
 * @retval None
 */
static void MX_LTDC_Init(void)
{

	/* USER CODE BEGIN LTDC_Init 0 */

	/* USER CODE END LTDC_Init 0 */

	LTDC_LayerCfgTypeDef pLayerCfg =
	{ 0 };

	/* USER CODE BEGIN LTDC_Init 1 */

	/* USER CODE END LTDC_Init 1 */
	hltdc.Instance = LTDC;
	hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
	hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
	hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
	hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
	hltdc.Init.HorizontalSync = 7;
	hltdc.Init.VerticalSync = 3;
	hltdc.Init.AccumulatedHBP = 14;
	hltdc.Init.AccumulatedVBP = 5;
	hltdc.Init.AccumulatedActiveW = 654;
	hltdc.Init.AccumulatedActiveH = 485;
	hltdc.Init.TotalWidth = 660;
	hltdc.Init.TotalHeigh = 487;
	hltdc.Init.Backcolor.Blue = 0;
	hltdc.Init.Backcolor.Green = 0;
	hltdc.Init.Backcolor.Red = 0;
	if (HAL_LTDC_Init(&hltdc) != HAL_OK)
	{
		Error_Handler();
	}
	pLayerCfg.WindowX0 = 0;
	pLayerCfg.WindowX1 = 480;
	pLayerCfg.WindowY0 = 0;
	pLayerCfg.WindowY1 = 272;
	pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;
	pLayerCfg.Alpha = 255;
	pLayerCfg.Alpha0 = 0;
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
	pLayerCfg.FBStartAdress = 0x70000000;
	pLayerCfg.ImageWidth = 480;
	pLayerCfg.ImageHeight = 272;
	pLayerCfg.Backcolor.Blue = 0;
	pLayerCfg.Backcolor.Green = 0;
	pLayerCfg.Backcolor.Red = 0;
	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN LTDC_Init 2 */

	/* USER CODE END LTDC_Init 2 */

}

/**
 * @brief OCTOSPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_OCTOSPI1_Init(void)
{

	/* USER CODE BEGIN OCTOSPI1_Init 0 */
	BSP_OSPI_NOR_Init_t ospi_nor_int;
	/* USER CODE END OCTOSPI1_Init 0 */

	OSPIM_CfgTypeDef sOspiManagerCfg =
	{ 0 };

	/* USER CODE BEGIN OCTOSPI1_Init 1 */

	/* USER CODE END OCTOSPI1_Init 1 */
	/* OCTOSPI1 parameter configuration*/
	hospi1.Instance = OCTOSPI1;
	hospi1.Init.FifoThreshold = 4;
	hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
	hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_MACRONIX;
	hospi1.Init.DeviceSize = 32;
	hospi1.Init.ChipSelectHighTime = 2;
	hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
	hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
	hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
	hospi1.Init.ClockPrescaler = 2;
	hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;
	hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_DISABLE;
	hospi1.Init.ChipSelectBoundary = 0;
	hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
	hospi1.Init.MaxTran = 0;
	hospi1.Init.Refresh = 0;
	if (HAL_OSPI_Init(&hospi1) != HAL_OK)
	{
		Error_Handler();
	}
	sOspiManagerCfg.ClkPort = 1;
	sOspiManagerCfg.DQSPort = 1;
	sOspiManagerCfg.NCSPort = 1;
	sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
	sOspiManagerCfg.IOHighPort = HAL_OSPIM_IOPORT_1_HIGH;
	if (HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN OCTOSPI1_Init 2 */
	HAL_OSPI_DeInit(&hospi1);
	ospi_nor_int.InterfaceMode = BSP_OSPI_NOR_OPI_MODE;
	ospi_nor_int.TransferRate = BSP_OSPI_NOR_DTR_TRANSFER;
	BSP_OSPI_NOR_DeInit(0);
	if (BSP_OSPI_NOR_Init(0, &ospi_nor_int) != BSP_ERROR_NONE)
	{
		Error_Handler();
	}
	if (BSP_OSPI_NOR_EnableMemoryMappedMode(0) != BSP_ERROR_NONE)
	{
		Error_Handler();
	}
	/* USER CODE END OCTOSPI1_Init 2 */

}

/**
 * @brief OCTOSPI2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_OCTOSPI2_Init(void)
{

	/* USER CODE BEGIN OCTOSPI2_Init 0 */
	//BSP_OSPI_RAM_Init_t ospi_ram_init;
	/* USER CODE END OCTOSPI2_Init 0 */

	OSPIM_CfgTypeDef sOspiManagerCfg =
	{ 0 };
	OSPI_HyperbusCfgTypeDef sHyperBusCfg =
	{ 0 };

	/* USER CODE BEGIN OCTOSPI2_Init 1 */
	OSPI_HyperbusCmdTypeDef sCommand =
	{ 0 };
	OSPI_MemoryMappedTypeDef sMemMappedCfg =
	{ 0 };
	/* USER CODE END OCTOSPI2_Init 1 */
	/* OCTOSPI2 parameter configuration*/
	hospi2.Instance = OCTOSPI2;
	hospi2.Init.FifoThreshold = 4;
	hospi2.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
	hospi2.Init.MemoryType = HAL_OSPI_MEMTYPE_HYPERBUS;
	hospi2.Init.DeviceSize = 24;
	hospi2.Init.ChipSelectHighTime = 4;
	hospi2.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
	hospi2.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
	hospi2.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
	hospi2.Init.ClockPrescaler = 2;
	hospi2.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;
	hospi2.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE;
	hospi2.Init.ChipSelectBoundary = 23;
	hospi2.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_USED;
	hospi2.Init.MaxTran = 0;
	hospi2.Init.Refresh = 400;
	if (HAL_OSPI_Init(&hospi2) != HAL_OK)
	{
		Error_Handler();
	}
	sOspiManagerCfg.ClkPort = 2;
	sOspiManagerCfg.DQSPort = 2;
	sOspiManagerCfg.NCSPort = 2;
	sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_2_LOW;
	sOspiManagerCfg.IOHighPort = HAL_OSPIM_IOPORT_2_HIGH;
	if (HAL_OSPIM_Config(&hospi2, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler();
	}
	sHyperBusCfg.RWRecoveryTime = 3;
	sHyperBusCfg.AccessTime = 6;
	sHyperBusCfg.WriteZeroLatency = HAL_OSPI_LATENCY_ON_WRITE;
	sHyperBusCfg.LatencyMode = HAL_OSPI_FIXED_LATENCY;
	if (HAL_OSPI_HyperbusCfg(&hospi2, &sHyperBusCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN OCTOSPI2_Init 2 */
	sCommand.AddressSpace = HAL_OSPI_MEMORY_ADDRESS_SPACE;
	sCommand.AddressSize = HAL_OSPI_ADDRESS_32_BITS;
	sCommand.DQSMode = HAL_OSPI_DQS_ENABLE;
	sCommand.Address = 0;
	sCommand.NbData = 1;

	if (HAL_OSPI_HyperbusCmd(&hospi2, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		Error_Handler();
	}

	sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_DISABLE;

	if (HAL_OSPI_MemoryMapped(&hospi2, &sMemMappedCfg) != HAL_OK)
	{
		Error_Handler();
	}

	/* USER CODE END OCTOSPI2_Init 2 */

}

/**
 * @brief TIM23 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM23_Init(void)
{

	/* USER CODE BEGIN TIM23_Init 0 */

	/* USER CODE END TIM23_Init 0 */

	TIM_Encoder_InitTypeDef sConfig =
	{ 0 };
	TIM_MasterConfigTypeDef sMasterConfig =
	{ 0 };

	/* USER CODE BEGIN TIM23_Init 1 */

	/* USER CODE END TIM23_Init 1 */
	htim23.Instance = TIM23;
	htim23.Init.Prescaler = 0;
	htim23.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim23.Init.Period = 400000;
	htim23.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim23.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 10;
	sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 10;
	if (HAL_TIM_Encoder_Init(&htim23, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim23, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM23_Init 2 */

	/* USER CODE END TIM23_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3 | GPIO_PIN_14, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOG, LCD_BL_CTRL_Pin | GPIO_PIN_5 | GPIO_PIN_4 | RENDER_TIME_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15 | LCD_DISP_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, MCU_ACTIVE_Pin | FRAME_RATE_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(VSYNC_FREQ_GPIO_Port, VSYNC_FREQ_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : PE3 PE14 */
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pins : LCD_BL_CTRL_Pin PG4 */
	GPIO_InitStruct.Pin = LCD_BL_CTRL_Pin | GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pin : PG5 */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pin : RENDER_TIME_Pin */
	GPIO_InitStruct.Pin = RENDER_TIME_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(RENDER_TIME_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : PF8 */
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	/*Configure GPIO pins : PD15 LCD_DISP_Pin */
	GPIO_InitStruct.Pin = GPIO_PIN_15 | LCD_DISP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pins : MCU_ACTIVE_Pin FRAME_RATE_Pin */
	GPIO_InitStruct.Pin = MCU_ACTIVE_Pin | FRAME_RATE_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : VSYNC_FREQ_Pin */
	GPIO_InitStruct.Pin = VSYNC_FREQ_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(VSYNC_FREQ_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		ITM_SendChar(*ptr++);
	}
	return len;
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
	/* USER CODE BEGIN 5 */

	/* Infinite loop */
	for (;;)
	{
		osDelay(100);
	}
	/* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTaskToGui */
/**
 * @brief Function implementing the myTaskToGUI thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTaskToGui */
void StartTaskToGui(void *argument)
{
	/* USER CODE BEGIN StartTaskToGui */
	//create the queue with data for the GUI
	xQueueHandle messageToView;
	messageToView = xQueueCreate(1, sizeof(viewData));

	/* Infinite loop */
	for (;;)
	{
		if (osMessageQueueGetSpace(messageToView) > 0)
		{
			osMessageQueuePut(messageToView, &viewData, 0, 0);
		}

		if (startTimerA == TRUE)
		{
			osTimerStart(myTimerI2CWindowAHandle, 3);
			startTimerA = FALSE;
		}

		sendDatatoSI5351();
		osDelay(1);
	}
	/* USER CODE END StartTaskToGui */
}

/* USER CODE BEGIN Header_StartTaskFromGui */
/**
 * @brief Function implementing the myTaskFromGui thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTaskFromGui */
void StartTaskFromGui(void *argument)
{
	/* USER CODE BEGIN StartTaskFromGui */
	/* Infinite loop */
	for (;;)
	{
		osDelay(1);
	}
	/* USER CODE END StartTaskFromGui */
}

/* myTimerCallbackA function */
void myTimerCallbackA(void *argument)
{
	/* USER CODE BEGIN myTimerCallbackA */
	//start timer B
	readyToSendI2C = TRUE;
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_SET);
	osTimerStart(myTimerI2CWindowBHandle, 25);
	/* USER CODE END myTimerCallbackA */
}

/* myTimerCallbackB function */
void myTimerCallbackB(void *argument)
{
	/* USER CODE BEGIN myTimerCallbackB */
	readyToSendI2C = FALSE;
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_RESET);
	/* USER CODE END myTimerCallbackB */
}

/* MPU Configuration */

void MPU_Config(void)
{
	MPU_Region_InitTypeDef MPU_InitStruct =
	{ 0 };

	/* Disables the MPU */
	HAL_MPU_Disable();

	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.BaseAddress = 0x24000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
	MPU_InitStruct.SubRegionDisable = 0x0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Number = MPU_REGION_NUMBER1;
	MPU_InitStruct.BaseAddress = 0x70000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_512MB;
	MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Number = MPU_REGION_NUMBER2;
	MPU_InitStruct.Size = MPU_REGION_SIZE_8MB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Number = MPU_REGION_NUMBER3;
	MPU_InitStruct.BaseAddress = 0x90000000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_512MB;
	MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/** Initializes and configures the Region and the memory to be protected
	 */
	MPU_InitStruct.Number = MPU_REGION_NUMBER4;
	MPU_InitStruct.Size = MPU_REGION_SIZE_64MB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	/* Enables the MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */
	if (htim->Instance == TIM6)
	{
		//readyToSendI2C = FALSE;
	}
	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM6)
	{
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
