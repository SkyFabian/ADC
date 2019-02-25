/**
   @file adc.c
   @brief Using ADC for simple measurements
   @author Bernhard Breinbauer
*/

// Includes ------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ioe.h"

#include "adc.h"

// Variables ------------------------------------------------------------------

// Function Declarations ------------------------------------------------------
static void RccInit(void);

// Functions ------------------------------------------------------------------

/**
  * Initialization Code for module adc
	* @param None
  * @retval None
  */
void adcInit(void) {
	RccInit();
}

/**
  * Task function of module adc.
	* @param None
  * @retval None
  */
void adcTask(void){
	// TODO: What has to be done in our main loop?

}


/**
  * @brief  Enable clock for all peripherals which are used
  * @param  None
  * @retval None
  */
static void RccInit(void)
{
	// TODO: What GPIOs do we need? -> Configure them!
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}


