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
#include <math.h>
#include <string.h> //for memcpy
#include "arm_math.h"
#include  "arm_const_structs.h"
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
#define INPUT_SAMPLES 2048
#define BAND_80M 80
#define BAND_40M 40
#define BAND_20M 20
#define ADC_SCALING 0.0001
#define AUDIO_FILTER_TAP_NUM 73
#define	AUDIO_FILTER_ADCDATA_BLOCKSIZE 10

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc3;

CRC_HandleTypeDef hcrc;

DAC_HandleTypeDef hdac1;
DMA_HandleTypeDef hdma_dac1_ch1;

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c3;
I2C_HandleTypeDef hi2c4;

LTDC_HandleTypeDef hltdc;

OSPI_HandleTypeDef hospi1;
OSPI_HandleTypeDef hospi2;

OPAMP_HandleTypeDef hopamp1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim23;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes =
{ .name = "defaultTask", .stack_size = 128 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for TouchGFXTask */
osThreadId_t TouchGFXTaskHandle;
const osThreadAttr_t TouchGFXTask_attributes =
{ .name = "TouchGFXTask", .stack_size = 4096 * 4, .priority = (osPriority_t) osPriorityAboveNormal, };
/* Definitions for myTaskToGUI */
osThreadId_t myTaskToGUIHandle;
const osThreadAttr_t myTaskToGUI_attributes =
{ .name = "myTaskToGUI", .stack_size = 1024 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for myTaskFromGui */
osThreadId_t myTaskFromGuiHandle;
const osThreadAttr_t myTaskFromGui_attributes =
{ .name = "myTaskFromGui", .stack_size = 1024 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for myTaskFFT */
osThreadId_t myTaskFFTHandle;
const osThreadAttr_t myTaskFFT_attributes =
{ .name = "myTaskFFT", .stack_size = 4096 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for myTaskFrameRate */
osThreadId_t myTaskFrameRateHandle;
const osThreadAttr_t myTaskFrameRate_attributes =
{ .name = "myTaskFrameRate", .stack_size = 1024 * 4, .priority = (osPriority_t) osPriorityNormal, };
/* Definitions for sMeterQueue */
osMessageQueueId_t sMeterQueueHandle;
const osMessageQueueAttr_t sMeterQueue_attributes =
{ .name = "sMeterQueue" };
/* Definitions for vfoQueue */
osMessageQueueId_t vfoQueueHandle;
const osMessageQueueAttr_t vfoQueue_attributes =
{ .name = "vfoQueue" };
/* Definitions for dynGraphQueue */
osMessageQueueId_t dynGraphQueueHandle;
const osMessageQueueAttr_t dynGraphQueue_attributes =
{ .name = "dynGraphQueue" };
/* USER CODE BEGIN PV */

//config SI5351
uint8_t phaseOffset;
const int32_t correction = 114840; //the higher the correction, the lower the frequency
si5351PLLConfig_t pllConf;
si5351OutputConfig_t outConf;

//VFO settings
float vfo_80m = 7146.0; //3573 * 4
float vfo_40m = 28296.0; //7074 * 4
float vfo_20m = 28148.0; //14074 * 4
uint32_t vfo_80m_5351 = 7146000;
uint32_t vfo_40m_5351 = 28296000;
uint32_t vfo_20m_5351 = 28148000;
uint8_t VFOhasChangedforDisplay;
uint8_t VFOhasChangedforSI5351 = TRUE;
uint8_t vfoDataHasChanged = TRUE;
uint8_t sMeterDataHasChanged;
uint8_t stateRXTX = 0; //0 = RX
uint8_t stateBand = 40;
uint8_t oldStateBand = 80;
uint8_t stateLSBUSB = 0; // 1 = LSB
uint8_t lastStateLSBUSB = 0;
uint8_t stateWFBW;
uint8_t graphHasBeenDisplayed = TRUE;

uint16_t stateStepSize = 1000;
uint16_t oldStateStepSize = 1000;
uint8_t rotaryEncoderCounter;
int8_t rotaryEncoderValue;
uint8_t newDataToBeSendToSI5351 = TRUE;

//FFT
uint32_t adcDualInputBuffer[INPUT_SAMPLES];
uint16_t fftComplexIntBuffer[2 * INPUT_SAMPLES];
float32_t fftComplexFloatBuffer[2 * INPUT_SAMPLES], fftOutputComplexMagnitudeBuffer[INPUT_SAMPLES], fftAdjustedMagnitudeBuffer[INPUT_SAMPLES];
float32_t hanningWIN[INPUT_SAMPLES];
uint16_t sMeterAverageValue[10];
uint16_t sMeterValue;
uint16_t oldSMeterValue;
uint8_t newADCDataAvailable;
int dynamicGraphValue[341];
typedef struct
{
	int dynGraphData[341];
} QueueElement;
QueueElement data;

/*
 FIR filter designed with
 http://t-filter.appspot.com
 sampling frequency: 10000 Hz
 * 0 Hz - 100 Hz
 gain = 0
 desired attenuation = -40 dB
 actual attenuation = -41.1368078119893 dB
 * 300 Hz - 2700 Hz
 gain = 1
 desired ripple = 1 dB
 actual ripple = 0.6596799066074697 dB
 * 3000 Hz - 5000 Hz
 gain = 0
 desired attenuation = -40 dB
 actual attenuation = -41.1368078119893 dB
 */
float audioFIRfilterTaps[AUDIO_FILTER_TAP_NUM] =
{ 0.01152326293750213, 0.012154895789079734, 0.006226696811174554, 0.0031360229190002384, 0.004533074687298142, 0.004267958631943703, 0.004987435595038369,
		0.0066418229411827215, 0.0027163108110072436, 0.00027090315437662164, 0.005902207325118628, 0.004735983505267677, -0.005197795293248476,
		-0.0023608515736345953, 0.005625500529680012, -0.005691754190516723, -0.01513510180914045, -0.0017393843732896829, -0.0016332656883314323,
		-0.02389136688141353, -0.019740746069879963, -0.0007470228959563541, -0.0212745486475054, -0.04158683314879713, -0.01331902676731243,
		-0.008231996733598158, -0.05270738350221322, -0.04414103179669497, 0.0028877689913779955, -0.03728161208835806, -0.08667998568743926,
		-0.01037576001167545, 0.020023411669254126, -0.12428389117184645, -0.10897358554195391, 0.2681713680270627, 0.5273396714099711, 0.2681713680270627,
		-0.10897358554195391, -0.12428389117184645, 0.020023411669254126, -0.01037576001167545, -0.08667998568743926, -0.03728161208835806,
		0.0028877689913779955, -0.04414103179669497, -0.05270738350221322, -0.008231996733598158, -0.01331902676731243, -0.04158683314879713,
		-0.0212745486475054, -0.0007470228959563541, -0.019740746069879963, -0.02389136688141353, -0.0016332656883314323, -0.0017393843732896829,
		-0.01513510180914045, -0.005691754190516723, 0.005625500529680012, -0.0023608515736345953, -0.005197795293248476, 0.004735983505267677,
		0.005902207325118628, 0.00027090315437662164, 0.0027163108110072436, 0.0066418229411827215, 0.004987435595038369, 0.004267958631943703,
		0.004533074687298142, 0.0031360229190002384, 0.006226696811174554, 0.012154895789079734, 0.01152326293750213 };

float audioFirSourceArray[AUDIO_FILTER_ADCDATA_BLOCKSIZE];
float audioFirDestinationArray[AUDIO_FILTER_ADCDATA_BLOCKSIZE];
float audioFirStateBuffer[AUDIO_FILTER_TAP_NUM + AUDIO_FILTER_ADCDATA_BLOCKSIZE - 1];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_BDMA_Init(void);
static void MX_DMA2D_Init(void);
static void MX_OCTOSPI1_Init(void);
static void MX_OCTOSPI2_Init(void);
static void MX_TIM23_Init(void);
static void MX_I2C4_Init(void);
static void MX_LTDC_Init(void);
static void MX_ADC1_Init(void);
static void MX_CRC_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C3_Init(void);
static void MX_ADC2_Init(void);
static void MX_ADC3_Init(void);
static void MX_DAC1_Init(void);
static void MX_OPAMP1_Init(void);
static void MX_TIM8_Init(void);
void StartDefaultTask(void *argument);
extern void TouchGFX_Task(void *argument);
void StartTaskToGui(void *argument);
void StartTaskFromGui(void *argument);
void StartTaskFFT(void *argument);
void StartTaskFRSync(void *argument);

/* USER CODE BEGIN PFP */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void sendDatatoSI5351(void);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void stepSizeChanged()
{
	if (stateStepSize == 1000)
	{
		switch (stateBand)
		{
		case BAND_80M:
			vfo_80m_5351 = (vfo_80m_5351 / 1000) * 1000;
			vfo_80m = (float) (vfo_80m_5351/2) / 1000.0;
			break;
		case BAND_40M:
			vfo_40m_5351 = (vfo_40m_5351 / 1000) * 1000;
			vfo_40m = (float) (vfo_40m_5351/4) / 1000.0;
			break;
		case BAND_20M:
			vfo_20m_5351 = (vfo_20m_5351 / 1000) * 1000;
			vfo_20m = (float) (vfo_20m_5351/2) / 1000.0;
			break;
		}
	}
	else if (stateStepSize == 100)
	{
		switch (stateBand)
		{
		case BAND_80M:
			vfo_80m_5351 = (vfo_80m_5351 / 100) * 100;
			vfo_80m = (float) (vfo_80m_5351/2) / 1000.0;
			break;
		case BAND_40M:
			vfo_40m_5351 = (vfo_40m_5351 / 100) * 100;
			vfo_40m = (float) (vfo_40m_5351/4) / 1000.0;
			break;
		case BAND_20M:
			vfo_20m_5351 = (vfo_20m_5351 / 100) * 100;
			vfo_20m = (float) (vfo_20m_5351/4) / 1000.0;
			break;
		}
	}
	else if (stateStepSize == 10)
	{
		switch (stateBand)
		{
		case BAND_80M:
			vfo_80m_5351 = (vfo_80m_5351 / 10) * 10;
			vfo_80m = (float) (vfo_80m_5351/2) / 1000.0;
			break;
		case BAND_40M:
			vfo_40m_5351 = (vfo_40m_5351 / 10) * 10;
			vfo_40m = (float) (vfo_40m_5351/4) / 1000.0;
			break;
		case BAND_20M:
			vfo_20m_5351 = (vfo_20m_5351 / 10) * 10;
			vfo_20m = (float) (vfo_20m_5351/2) / 1000.0;
			break;
		}
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim23)
	{
		rotaryEncoderCounter++;
		if (rotaryEncoderCounter == 2)
		{
			vfoDataHasChanged = TRUE;

			if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim23))
			{
				switch (stateBand)
				{
				case BAND_80M:
					vfo_80m_5351 = vfo_80m_5351 - stateStepSize;
					break;
				case BAND_40M:
					vfo_40m_5351 = vfo_40m_5351 - stateStepSize;
					break;
				case BAND_20M:
					vfo_20m_5351 = vfo_20m_5351 - stateStepSize;
					break;
				}
				VFOhasChangedforSI5351 = TRUE;
				VFOhasChangedforDisplay = TRUE;
				newDataToBeSendToSI5351 = TRUE;
				//sendDatatoSI5351();
			}
			else
			{
				switch (stateBand)
				{
				case BAND_80M:
					vfo_80m_5351 = vfo_80m_5351 + stateStepSize;
					break;
				case BAND_40M:
					vfo_40m_5351 = vfo_40m_5351 + stateStepSize;
					break;
				case BAND_20M:
					vfo_20m_5351 = vfo_20m_5351 + stateStepSize;
					break;
				}
				VFOhasChangedforSI5351 = TRUE;
				VFOhasChangedforDisplay = TRUE;
				newDataToBeSendToSI5351 = TRUE;
				//sendDatatoSI5351();
			}

			switch (stateBand)
			{
			case BAND_80M:
				vfo_80m = ((float) (vfo_80m_5351)) / 1000.0;
				break;
			case BAND_40M:
				vfo_40m = ((float) (vfo_40m_5351)) / 1000.0;
				break;
			case BAND_20M:
				vfo_20m = ((float) (vfo_20m_5351)) / 1000.0;
				break;
			}

			rotaryEncoderCounter = 0;
		}
	}
}

void sendDatatoSI5351(void)
{
	if (VFOhasChangedforSI5351 == TRUE)
	{
		switch (stateBand)
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

		si5351_SetupOutput(0, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_8MA, &outConf, 0);
		//si5351_SetupOutput(2, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_8MA, &outConf, phaseOffset);
		si5351_SetupPLL(SI5351_PLL_A, &pllConf);
		VFOhasChangedforSI5351 = FALSE;
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc == &hadc1)
	{
		uint16_t j = 0;
		for (uint16_t i = 0; i < 2 * INPUT_SAMPLES; i++)
		{
			if (i % 2 == 0)
			{
				fftComplexIntBuffer[i] = (uint16_t) (adcDualInputBuffer[j] & 0x0000FFFF); //real vector, I
			}
			else
			{
				fftComplexIntBuffer[i] = (uint16_t) (adcDualInputBuffer[j] >> 16); //imaginary vector, Q
				j++;
			}
		}
		newADCDataAvailable = TRUE;
	}
	if (hadc == &hadc3)
	{

	}
}



void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc == &hadc3)
	{

	}
}



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
	MX_DMA_Init();
	MX_BDMA_Init();
	MX_DMA2D_Init();
	MX_OCTOSPI1_Init();
	MX_OCTOSPI2_Init();
	MX_TIM23_Init();
	MX_I2C4_Init();
	MX_LTDC_Init();
	MX_ADC1_Init();
	MX_CRC_Init();
	MX_TIM1_Init();
	MX_I2C3_Init();
	MX_ADC2_Init();
	MX_ADC3_Init();
	MX_DAC1_Init();
	MX_OPAMP1_Init();
	MX_TIM8_Init();
	MX_TouchGFX_Init();
	/* Call PreOsInit function */
	MX_TouchGFX_PreOSInit();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Encoder_Start_IT(&htim23, TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start(&htim1);

	HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
	HAL_ADC_Start(&hadc2);
	HAL_ADCEx_MultiModeStart_DMA(&hadc1, adcDualInputBuffer, INPUT_SAMPLES);
	newADCDataAvailable = TRUE;

	for (uint16_t i = 0; i < INPUT_SAMPLES; i++)
	{
		hanningWIN[i] = ADC_SCALING * (0.5 - (0.5 * cos((2.0 * PI * i) / (INPUT_SAMPLES - 1)))); //with scaling 0.0001
	}

	oldStateBand = stateBand = 40;
	vfo_40m_5351 = 28296000;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); //set input bandpass to 40m
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //set input bandpass to 40m
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); //LSB
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

	/*
	 * The order is important! Setup the channels first, then setup the PLL.
	 * Alternatively you could reset the PLL after setting up PLL and channels.
	 * However since _SetupPLL() always resets the PLL this would only cause
	 * sending extra I2C commands.
	 */
	si5351_SetupOutput(0, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_8MA, &outConf, 0);
	si5351_SetupOutput(2, SI5351_PLL_A, SI5351_DRIVE_STRENGTH_8MA, &outConf, phaseOffset);
	si5351_SetupPLL(SI5351_PLL_A, &pllConf);
	si5351_EnableOutputs((1 << 0) | (1 << 2));
	sendDatatoSI5351();
	arm_fir_instance_f32 audioFIRfilterInstance;
	arm_fir_init_f32(&audioFIRfilterInstance, AUDIO_FILTER_TAP_NUM, audioFIRfilterTaps, audioFirStateBuffer, AUDIO_FILTER_ADCDATA_BLOCKSIZE);
	//arm_fir_f32(&audioFIRfilterInstance, audioFirSourceArray, audioFirDestinationArray, AUDIO_FILTER_ADCDATA_BLOCKSIZE);

	/* USER CODE END 2 */

	/* Init scheduler */
	osKernelInitialize();

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* Create the queue(s) */
	/* creation of sMeterQueue */
	sMeterQueueHandle = osMessageQueueNew(1, sizeof(uint16_t), &sMeterQueue_attributes);

	/* creation of vfoQueue */
	vfoQueueHandle = osMessageQueueNew(1, sizeof(float), &vfoQueue_attributes);

	/* creation of dynGraphQueue */
	dynGraphQueueHandle = osMessageQueueNew(1, 1364, &dynGraphQueue_attributes);

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

	/* creation of myTaskFFT */
	myTaskFFTHandle = osThreadNew(StartTaskFFT, NULL, &myTaskFFT_attributes);

	/* creation of myTaskFrameRate */
	myTaskFrameRateHandle = osThreadNew(StartTaskFRSync, NULL, &myTaskFrameRate_attributes);

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
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = 64;
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
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI | RCC_PERIPHCLK_CKPER;
	PeriphClkInitStruct.PLL2.PLL2M = 5;
	PeriphClkInitStruct.PLL2.PLL2N = 80;
	PeriphClkInitStruct.PLL2.PLL2P = 2;
	PeriphClkInitStruct.PLL2.PLL2Q = 2;
	PeriphClkInitStruct.PLL2.PLL2R = 2;
	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
	PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
	PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_PLL2;
	PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void)
{

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_MultiModeTypeDef multimode =
	{ 0 };
	ADC_ChannelConfTypeDef sConfig =
	{ 0 };

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */

	/** Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc1.Init.Resolution = ADC_RESOLUTION_14B;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_ONESHOT;
	hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	hadc1.Init.OversamplingMode = DISABLE;
	hadc1.Init.Oversampling.Ratio = 1;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure the ADC multi-mode
	 */
	multimode.Mode = ADC_DUALMODE_REGSIMULT;
	multimode.DualModeData = ADC_DUALMODEDATAFORMAT_32_10_BITS;
	multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_1CYCLE;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_16CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	sConfig.OffsetSignedSaturation = DISABLE;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief ADC2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC2_Init(void)
{

	/* USER CODE BEGIN ADC2_Init 0 */

	/* USER CODE END ADC2_Init 0 */

	ADC_ChannelConfTypeDef sConfig =
	{ 0 };

	/* USER CODE BEGIN ADC2_Init 1 */

	/* USER CODE END ADC2_Init 1 */

	/** Common config
	 */
	hadc2.Instance = ADC2;
	hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc2.Init.Resolution = ADC_RESOLUTION_14B;
	hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	hadc2.Init.LowPowerAutoWait = DISABLE;
	hadc2.Init.ContinuousConvMode = DISABLE;
	hadc2.Init.NbrOfConversion = 1;
	hadc2.Init.DiscontinuousConvMode = DISABLE;
	hadc2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
	hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc2.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	hadc2.Init.OversamplingMode = DISABLE;
	hadc2.Init.Oversampling.Ratio = 1;
	if (HAL_ADC_Init(&hadc2) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	sConfig.OffsetSignedSaturation = DISABLE;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC2_Init 2 */

	/* USER CODE END ADC2_Init 2 */

}

/**
 * @brief ADC3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC3_Init(void)
{

	/* USER CODE BEGIN ADC3_Init 0 */

	/* USER CODE END ADC3_Init 0 */

	ADC_ChannelConfTypeDef sConfig =
	{ 0 };

	/* USER CODE BEGIN ADC3_Init 1 */

	/* USER CODE END ADC3_Init 1 */

	/** Common config
	 */
	hadc3.Instance = ADC3;
	hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc3.Init.Resolution = ADC_RESOLUTION_12B;
	hadc3.Init.DataAlign = ADC3_DATAALIGN_RIGHT;
	hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc3.Init.LowPowerAutoWait = DISABLE;
	hadc3.Init.ContinuousConvMode = ENABLE;
	hadc3.Init.NbrOfConversion = 1;
	hadc3.Init.DiscontinuousConvMode = DISABLE;
	hadc3.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T8_TRGO;
	hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc3.Init.DMAContinuousRequests = ENABLE;
	hadc3.Init.SamplingMode = ADC_SAMPLING_MODE_NORMAL;
	hadc3.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
	hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc3.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	hadc3.Init.OversamplingMode = DISABLE;
	hadc3.Init.Oversampling.Ratio = ADC3_OVERSAMPLING_RATIO_2;
	if (HAL_ADC_Init(&hadc3) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC3_SAMPLETIME_2CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	sConfig.OffsetSign = ADC3_OFFSET_SIGN_NEGATIVE;
	if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC3_Init 2 */

	/* USER CODE END ADC3_Init 2 */

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
 * @brief DAC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_DAC1_Init(void)
{

	/* USER CODE BEGIN DAC1_Init 0 */

	/* USER CODE END DAC1_Init 0 */

	DAC_ChannelConfTypeDef sConfig =
	{ 0 };

	/* USER CODE BEGIN DAC1_Init 1 */

	/* USER CODE END DAC1_Init 1 */

	/** DAC Initialization
	 */
	hdac1.Instance = DAC1;
	if (HAL_DAC_Init(&hdac1) != HAL_OK)
	{
		Error_Handler();
	}

	/** DAC channel OUT1 config
	 */
	sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
	sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE;
	sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
	if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

	/** DAC channel OUT2 config
	 */
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
	if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN DAC1_Init 2 */

	/* USER CODE END DAC1_Init 2 */

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
	hi2c3.Init.Timing = 0x60404E72;
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
 * @brief OPAMP1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_OPAMP1_Init(void)
{

	/* USER CODE BEGIN OPAMP1_Init 0 */

	/* USER CODE END OPAMP1_Init 0 */

	/* USER CODE BEGIN OPAMP1_Init 1 */

	/* USER CODE END OPAMP1_Init 1 */
	hopamp1.Instance = OPAMP1;
	hopamp1.Init.Mode = OPAMP_FOLLOWER_MODE;
	hopamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_DAC_CH;
	hopamp1.Init.PowerMode = OPAMP_POWERMODE_NORMAL;
	hopamp1.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
	if (HAL_OPAMP_Init(&hopamp1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN OPAMP1_Init 2 */

	/* USER CODE END OPAMP1_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void)
{

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig =
	{ 0 };
	TIM_MasterConfigTypeDef sMasterConfig =
	{ 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 275 - 1;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 10 - 1;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

}

/**
 * @brief TIM8 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM8_Init(void)
{

	/* USER CODE BEGIN TIM8_Init 0 */

	/* USER CODE END TIM8_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig =
	{ 0 };
	TIM_MasterConfigTypeDef sMasterConfig =
	{ 0 };

	/* USER CODE BEGIN TIM8_Init 1 */

	/* USER CODE END TIM8_Init 1 */
	htim8.Instance = TIM8;
	htim8.Init.Prescaler = 275 - 1;
	htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim8.Init.Period = 100 - 1;
	htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim8.Init.RepetitionCounter = 0;
	htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM8_Init 2 */

	/* USER CODE END TIM8_Init 2 */

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
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 10;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
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
 * Enable DMA controller clock
 */
static void MX_BDMA_Init(void)
{

	/* DMA controller clock enable */
	__HAL_RCC_BDMA_CLK_ENABLE();

	/* DMA interrupt init */
	/* BDMA_Channel0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(BDMA_Channel0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(BDMA_Channel0_IRQn);

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
	/* DMA1_Stream1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

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
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12 | VSYNC_FREQ_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOG, LCD_BL_CTRL_Pin | GPIO_PIN_5 | GPIO_PIN_4 | RENDER_TIME_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15 | LCD_DISP_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, MCU_ACTIVE_Pin | FRAME_RATE_Pin | GPIO_PIN_13 | GPIO_PIN_12, GPIO_PIN_RESET);

	/*Configure GPIO pins : PE3 PE14 */
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

	/*Configure GPIO pin : PA12 */
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : LCD_BL_CTRL_Pin PG5 PG4 */
	GPIO_InitStruct.Pin = LCD_BL_CTRL_Pin | GPIO_PIN_5 | GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pin : RENDER_TIME_Pin */
	GPIO_InitStruct.Pin = RENDER_TIME_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(RENDER_TIME_GPIO_Port, &GPIO_InitStruct);

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

	/*Configure GPIO pins : PB13 PB12 */
	GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : VSYNC_FREQ_Pin */
	GPIO_InitStruct.Pin = VSYNC_FREQ_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(VSYNC_FREQ_GPIO_Port, &GPIO_InitStruct);

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
		osDelay(50);
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

	/* Infinite loop */
	for (;;)
	{
		sMeterValue = 0;
		if (stateLSBUSB == 1)
		{
			for (uint8_t i = 0; i < 15; i++)
			{
				sMeterValue = sMeterValue + dynamicGraphValue[168 - i];
			}
			sMeterValue = sMeterValue / 15;
			//printf("The S Meter Data: %d \n", sMeterValue);
		}
		else
		{
			for (uint8_t i = 0; i < 15; i++)
			{
				sMeterValue = sMeterValue + dynamicGraphValue[171 + i];
			}
			sMeterValue = sMeterValue / 15;
		}

		if (sMeterValue != oldSMeterValue)
		{
			if (osMessageQueueGetCount(sMeterQueueHandle) == 0)
				osMessageQueuePut(sMeterQueueHandle, &sMeterValue, 0U, 0);
			oldSMeterValue = sMeterValue;
		}

		if (vfoDataHasChanged == TRUE)
		{
			if (osMessageQueueGetCount(vfoQueueHandle) == 0)
			{
				if (stateBand == 80)
					osMessageQueuePut(vfoQueueHandle, &vfo_80m, 0U, 0);
				if (stateBand == 40)
					osMessageQueuePut(vfoQueueHandle, &vfo_40m, 0U, 0);
				if (stateBand == 20)
					osMessageQueuePut(vfoQueueHandle, &vfo_20m, 0U, 0);
			}
			vfoDataHasChanged = FALSE;
		}
		osDelay(100);
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
		if (stateRXTX == FALSE)
		{
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_5, GPIO_PIN_RESET);
		}

		if (stateLSBUSB != lastStateLSBUSB)
		{
			if (stateLSBUSB == 1)
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET); //USB
			}
			else
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET); //LSB
			}
			lastStateLSBUSB = stateLSBUSB;
		}

		if ((oldStateBand != stateBand) && (stateBand == 20))
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
			vfoDataHasChanged = TRUE;
			VFOhasChangedforDisplay = TRUE;
			VFOhasChangedforSI5351 = TRUE;
			oldStateBand = stateBand;
			newDataToBeSendToSI5351 = TRUE;
			//sendDatatoSI5351();
		}

		if ((oldStateBand != stateBand) && (stateBand == 40))
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
			vfoDataHasChanged = TRUE;
			VFOhasChangedforDisplay = TRUE;
			VFOhasChangedforSI5351 = TRUE;
			oldStateBand = stateBand;
			newDataToBeSendToSI5351 = TRUE;
			//sendDatatoSI5351();
		}

		if ((oldStateBand != stateBand) && (stateBand == 80))
		{
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
			vfoDataHasChanged = TRUE;
			VFOhasChangedforDisplay = TRUE;
			VFOhasChangedforSI5351 = TRUE;
			oldStateBand = stateBand;
			newDataToBeSendToSI5351 = TRUE;
			//sendDatatoSI5351();
		}
		if (oldStateStepSize != stateStepSize)
		{
			stepSizeChanged();
			vfoDataHasChanged = TRUE;
			VFOhasChangedforDisplay = TRUE;
			VFOhasChangedforSI5351 = TRUE;
			oldStateStepSize = stateStepSize;
			newDataToBeSendToSI5351 = TRUE;
			//sendDatatoSI5351();
		}
		osDelay(100);
	}
	/* USER CODE END StartTaskFromGui */
}

/* USER CODE BEGIN Header_StartTaskFFT */
/**
 * @brief Function implementing the myTaskFFT thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTaskFFT */
void StartTaskFFT(void *argument)
{
	/* USER CODE BEGIN StartTaskFFT */
	/* Infinite loop */
	for (;;)
	{
		if (newADCDataAvailable == TRUE)
		{

			uint16_t k = 0;
			for (uint16_t i = 0; i < 2 * INPUT_SAMPLES; i++)
			{
				if ((i % 2) == 0 && (i > 0))
				{
					k++;
				}
				fftComplexFloatBuffer[i] = ((float) fftComplexIntBuffer[i]) * hanningWIN[k]; //* Hanning Window
			}

			if (INPUT_SAMPLES == 2048)
			{
				arm_cfft_f32(&arm_cfft_sR_f32_len2048, fftComplexFloatBuffer, 0, 1);
			}

			arm_cmplx_mag_f32(fftComplexFloatBuffer, fftOutputComplexMagnitudeBuffer, INPUT_SAMPLES);
			//fftOutputComplexMagnitudeBuffer[0] = 0;
			fftOutputComplexMagnitudeBuffer[1] = 0;
			fftOutputComplexMagnitudeBuffer[2047] = 0;
			fftOutputComplexMagnitudeBuffer[2046] = 0;

			for (uint16_t i = 0; i < 1024; i++) //move the bins at the edges (upper and lower) to the middle of the array and scale
			{
				fftAdjustedMagnitudeBuffer[i] = fftOutputComplexMagnitudeBuffer[1024 + i];
			}
			for (uint16_t i = 1024; i < 2048; i++)
			{
				fftAdjustedMagnitudeBuffer[i] = fftOutputComplexMagnitudeBuffer[i - 1024];
			}

			uint16_t j = 0;
			if (INPUT_SAMPLES == 2048 && stateWFBW == 0)
			{
				for (uint16_t i = 0; i < 2048; i += 6)
				{
					dynamicGraphValue[j] = (int) ((fftAdjustedMagnitudeBuffer[i] + fftAdjustedMagnitudeBuffer[i + 1] + fftAdjustedMagnitudeBuffer[i + 2]
							+ fftAdjustedMagnitudeBuffer[i + 3] + fftAdjustedMagnitudeBuffer[i + 4] + fftAdjustedMagnitudeBuffer[i + 5]) / 6.0);
					if (dynamicGraphValue[j] > 50)
						dynamicGraphValue[j] = 50;
					j++;
				}
			}

			if (INPUT_SAMPLES == 2048 && stateWFBW == 1) //16k BW
			{
				for (uint16_t i = 342; i < 1707; i += 4)
				{
					dynamicGraphValue[j] = (int) ((fftAdjustedMagnitudeBuffer[i] + fftAdjustedMagnitudeBuffer[i + 1] + fftAdjustedMagnitudeBuffer[i + 2]
							+ fftAdjustedMagnitudeBuffer[i + 3]) / 4.0);
					if (dynamicGraphValue[j] > 50)
						dynamicGraphValue[j] = 50;
					j++;
				}
			}

			if (INPUT_SAMPLES == 2048 && stateWFBW == 2)
			{
				for (uint16_t i = 683; i < 1366; i += 2)
				{
					dynamicGraphValue[j] = (int) ((fftAdjustedMagnitudeBuffer[i] + fftAdjustedMagnitudeBuffer[i + 1]) / 2.0);
					if (dynamicGraphValue[j] > 50)
						dynamicGraphValue[j] = 50;
					j++;
				}
			}

			if (INPUT_SAMPLES == 2048 && stateWFBW == 3) //16k BW
			{
				for (uint16_t i = 854; i < 1196; i++)
				{
					dynamicGraphValue[j] = (int) fftAdjustedMagnitudeBuffer[i];
					if (dynamicGraphValue[j] > 50)
						dynamicGraphValue[j] = 50;
					j++;
				}
			}

			memcpy(data.dynGraphData, dynamicGraphValue, sizeof(dynamicGraphValue));

			if (osMessageQueueGetCount(dynGraphQueueHandle) == 0)
				osMessageQueuePut(dynGraphQueueHandle, &data, 0U, 0);
			newADCDataAvailable = FALSE;
			HAL_ADCEx_MultiModeStart_DMA(&hadc1, adcDualInputBuffer, INPUT_SAMPLES);
		}
		osDelay(50);
	}
	/* USER CODE END StartTaskFFT */
}

/* USER CODE BEGIN Header_StartTaskFRSync */
/**
 * @brief Function implementing the myTaskFrameRate thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartTaskFRSync */
void StartTaskFRSync(void *argument)
{
	/* USER CODE BEGIN StartTaskFRSync */
	/* Infinite loop */
	for (;;)
	{
		if (newDataToBeSendToSI5351 == TRUE)
		{
			sendDatatoSI5351();
			newDataToBeSendToSI5351 = FALSE;
		}
		osDelay(30);
	}
	/* USER CODE END StartTaskFRSync */
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
	if (htim->Instance == TIM1)
	{
		if (newADCDataAvailable == TRUE)
		{
			newADCDataAvailable = FALSE;
		}
		else
		{
			newADCDataAvailable = TRUE;
		}
	}
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
