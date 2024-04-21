/**
 @file adc.c
 @brief Using ADC for simple measurements
 @author Bernhard Breinbauer
 */

// Includes ------------------------------------------------------------------
#include <stdio.h>

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f4xx_hal_adc.h"
#include "adc.h"

// Variables ------------------------------------------------------------------
static ADC_HandleTypeDef adc1Handle;

// Function Declarations ------------------------------------------------------
static void GpioInit(void);
static void AdcInit(void);

// Functions ------------------------------------------------------------------

/**
 * Initialization Code for module adc
 * @param None
 * @retval None
 */
void adcInit(void) {
	GpioInit();
	AdcInit();

}

/**
 * return voltage value of ADC measurement on potentiometer
 * @param None
 * @retval voltage value
 */
float adcPotiVoltage(void) {
	uint16_t messwert;
	float voltage;

	ADC_ChannelConfTypeDef CH5;
	// TODO: Set up ADC1 - channel 5
	CH5.Channel = ADC_CHANNEL_5;
	CH5.Offset = 0;
	CH5.Rank = 1;
	CH5.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&adc1Handle, &CH5);

	// start ADC measurement
	HAL_ADC_Start(&adc1Handle);
	HAL_ADC_PollForConversion(&adc1Handle, 5);

	// wait until conversion is finished
	// (could be done with interrupt - would require different configuration)

	// get measurement result, calculate voltage value
	messwert = HAL_ADC_GetValue(&adc1Handle);

	voltage = messwert * 0.0007324; //(3/2^12)=0.0007324

	return voltage;
}

float InternTemperature(void) {
	uint16_t messwert;
	float voltage;
	float temp;

	ADC_ChannelConfTypeDef CH18;
	// TODO: Set up ADC1 - channel 18
	CH18.Channel = ADC_CHANNEL_18;
	CH18.Offset = 0;
	CH18.Rank = 1;
	CH18.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	HAL_ADC_ConfigChannel(&adc1Handle, &CH18);

	// start ADC measurement
	HAL_ADC_Start(&adc1Handle);
	HAL_ADC_PollForConversion(&adc1Handle, 5);

	//get measurement result, calculate temp
	messwert = HAL_ADC_GetValue(&adc1Handle);
	voltage = messwert * 0.0007324; //(3/2^12)=0.0007324
	temp = (voltage - 0.76) / 0.0025 + 25;

	return temp;

}

/**
 * @brief  Configuration of used GPIOs
 * @param  None
 * @retval None
 */
static void GpioInit(void) {
	// TODO: What GPIOs do we need? -> Configure them!
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef pa5;
	pa5.Mode = GPIO_MODE_ANALOG;
	pa5.Speed = GPIO_SPEED_MEDIUM;
	pa5.Pull = GPIO_NOPULL;
	pa5.Alternate = 0;
	pa5.Pin = GPIO_PIN_5;
	HAL_GPIO_Init(GPIOA, &pa5);

}

/**
 * @brief  Configuration of ADC1 and ADC3
 * @param  None
 * @retval None
 */
static void AdcInit(void) {
	__HAL_RCC_ADC1_CLK_ENABLE();

	// TODO: Initialize ADC1 for single conversion
	adc1Handle.Instance = ADC1;
	adc1Handle.Init.Resolution = ADC_RESOLUTION12b;
	adc1Handle.Init.EOCSelection = DISABLE;
	adc1Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc1Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	adc1Handle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
	;
	adc1Handle.Init.ContinuousConvMode = DISABLE;
	adc1Handle.Init.DMAContinuousRequests = DISABLE;
	adc1Handle.Init.DiscontinuousConvMode = DISABLE;
	adc1Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	adc1Handle.Init.NbrOfConversion = 1;
	adc1Handle.Init.NbrOfDiscConversion = 0;
	adc1Handle.Init.ScanConvMode = DISABLE;

	// TODO: Use HAL_ADC_Init() to configure the ADC
	HAL_ADC_Init(&adc1Handle);

}

