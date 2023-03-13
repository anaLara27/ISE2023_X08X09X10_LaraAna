#include "stm32f4xx_hal.h"
//#include "stm32f429xx.h"
#define RESOLUTION_12B 4096U
#define VREF 3.3f
static ADC_HandleTypeDef hadc;
static volatile uint8_t  AD_done;       /* AD conversion done flag            */



/**
  * @brief config the use of analog inputs ADC123_IN10 and ADC123_IN13 and enable ADC1 clock
  * @param None
  * @retval None
  */
void ADC_Initialize(void){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
		ADC_ChannelConfTypeDef sConfig;
		
	__HAL_RCC_ADC1_CLK_ENABLE();//se habilita el reloj del ADC
	__HAL_RCC_GPIOC_CLK_ENABLE();//Se habilita el reloj del perto C
	/*PC0     ------> ADC1_IN10
    PC3     ------> ADC1_IN13
    */
		//Pot 1 inicialización del pin 
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		//Pot 2 inicialización del pin
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    //Configuración del ADC
	hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;//inicialización del preescaler
  hadc.Init.Resolution = ADC_RESOLUTION_12B;//resolución 12 bits
  hadc.Init.ScanConvMode = DISABLE;//desabilita los modos de escaneo continuo y discontinuo
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  HAL_ADC_Init(&hadc);
	
	sConfig.Channel = 10;//configura el canal 10
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;//tiempo de muestreo de 3 ciclos
	HAL_ADC_ConfigChannel(&hadc, &sConfig);
  }

int32_t ADC_Uninitialize (void) {

  /* Peripheral clock disable */
  __HAL_RCC_ADC1_CLK_DISABLE();

  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_3);

  return 0;
}

/**
  \fn          int32_t ADC_StartConversion (void)
  \brief       Start conversion
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_StartConversion (void) {
  __HAL_ADC_CLEAR_FLAG(&hadc, ADC_FLAG_EOC);
    HAL_ADC_Start(&hadc);

    AD_done = 0;
  return 0;
}

/**
  \fn          int32_t ADC_ConversionDone (void)
  \brief       Check if conversion finished
  \returns
   - \b  0: conversion finished
   - \b -1: conversion in progress
*/
int32_t ADC_ConversionDone (void) {
  HAL_StatusTypeDef status;

  status = HAL_ADC_PollForConversion(&hadc, 0);
  if (status == HAL_OK) {
    AD_done = 1;
    return 0;
  } else {
    AD_done = 0;
    return -1;
  }
}

/**
  \fn          int32_t ADC_GetValue (void)
  \brief       Get converted value
  \returns
   - <b> >=0</b>: converted value
   - \b -1: conversion in progress or failed
*/
int32_t ADC_GetValue (void) {
  HAL_StatusTypeDef status;
  int32_t val;

  if (AD_done == 0) {
    status = HAL_ADC_PollForConversion(&hadc, 0);
    if (status != HAL_OK) return -1;
  } else {
    AD_done = 0;
  }

  val = (int32_t)HAL_ADC_GetValue(&hadc);

  return val;
}

/**
  \fn          uint32_t ADC_GetResolution (void)
  \brief       Get resolution of Analog-to-Digital Converter
  \returns     Resolution (in bits)
*/
uint32_t ADC_GetResolution (void) {
  return ADC_RESOLUTION_12B;
}


