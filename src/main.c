/**
   @file main.c
   @brief AIIT Projektvorlage fuer Uebung ADC
      STM32F429I-Discovery Board
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

#include "scope.h"

// Variables ------------------------------------------------------------------
volatile int32_t SysTickCnt;

// Function Declarations ------------------------------------------------------
static void LcdInit(void);

// Functions ------------------------------------------------------------------
/**
  * ISR (Interrupt Service Routine) of SysTick.
	* @param None
	* @Note This function has to be called every 10ms! See> SysTick_Config()
  * @retval None
  */
void SysTick_Handler(void) {
  SysTickCnt++;
}

/**
   @brief Main Function
   @param None
   @return This function should never return
*/
int main(void) {

  RCC_ClocksTypeDef Clocks;
  static int32_t SysTickCnt_Hold10ms;
  static int32_t SysTickCnt_Hold100ms;
  static int32_t SysTickCnt_Hold1s;

  SysTickCnt = 0;
  SysTickCnt_Hold10ms = 0;
  SysTickCnt_Hold100ms = 0;
  SysTickCnt_Hold1s = 0;

  RCC_GetClocksFreq(&Clocks);
  SysTick_Config( Clocks.HCLK_Frequency/100 - 1 );	// 100 Hz ( T=10ms)
															// Inline-Funktion siehe core_cm4.h (ARM-Grundmodul)
  LcdInit();

  while (1) {
		/* The following code implements a simple scheduler.
		 * Add your functions to the one of the time slots of the scheduler.
		 * Note: This is a non-preemptive and cooperation-based scheduler: If a function
		 * blocks execution, the scheduler will fail to execute other functions. */

		// execute 10ms tasks
		if( SysTickCnt - SysTickCnt_Hold10ms >= 1 ) {
			SysTickCnt_Hold10ms += 1;
			// add tasks here
		}

		// execute 100ms tasks
		if( SysTickCnt - SysTickCnt_Hold100ms >= 10 ) {
			SysTickCnt_Hold100ms += 10;
		}

		// execute 1s tasks
		if( SysTickCnt - SysTickCnt_Hold1s >= 100 ) {
			SysTickCnt_Hold1s += 100;
			// add tasks here
		}
  }
}

/**
  * Initialize LCD and print default text
	* @param None
  * @retval None
  */
static void LcdInit(void) {
  LCD_Init();					// LCD initiatization
  LCD_LayerInit(); 		// LCD Layer initiatization
  LTDC_Cmd(ENABLE);  	// Enable the LTDC
  LCD_SetLayer(LCD_FOREGROUND_LAYER);	// Set LCD foreground layer
  LCD_Clear(LCD_COLOR_BLUE);
  LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLUE); // TextColor,BackColor

  LCD_SetFont(&Font16x24);				//  line: 0...12	, column: 0...14		(default)
  LCD_SetPrintPosition(1,0);  // line 1, column 0
  printf("   HTL-WELS    ");

  LCD_SetFont(&Font8x8);					// line: 0...39	, column: 0...29
  LCD_SetPrintPosition(39,0);			// line 39, column 0
  LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLUE); // TextColor,BackColor
  printf("  (c) Bernhard Breinbauer  ");

  LCD_SetFont(&Font16x24);
  LCD_SetColors(LCD_COLOR_WHITE, LCD_COLOR_BLUE);
}
