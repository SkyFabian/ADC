/**
 ******************************************************************************
 * @file    ts_calibration.c
 * @brief   This application code shows how to calibrate the touchscreen.
 ******************************************************************************
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "ts_calibration.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"


/** @addtogroup STM32F4xx_HAL_Applications
 * @{
 */

/** @addtogroup LTDC_Paint
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static TS_StateTypeDef  TS_State;
static uint8_t Calibration_Done = 0;
static int16_t  A1, A2, B1, B2;
static int16_t aPhysX[2], aPhysY[2], aLogX[2], aLogY[2];

/* Private function prototypes -----------------------------------------------*/
static void TouchscreenCalibration_SetHint(void);
static void GetPhysValues(int16_t LogX, int16_t LogY, int16_t * pPhysX, int16_t * pPhysY);
static void WaitForPressedState(uint8_t Pressed);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Performs the TS calibration
 * @param  None
 * @retval None
 */
void TS_Calibration(void)
{ 
	uint8_t status = 0;
	uint8_t i = 0;

	TouchscreenCalibration_SetHint();

	//  status = TS_Init(LCD_GetXSize(), LCD_GetYSize());
	//
	//  if (status != TS_OK)
	//  {
	//    LCD_SetBackColor(LCD_COLOR_WHITE);
	//    LCD_SetTextColor(LCD_COLOR_RED);
	//    LCD_DisplayStringAt(0, LCD_GetYSize()- 95, "ERROR", CENTER_MODE);
	//    LCD_DisplayStringAt(0, LCD_GetYSize()- 80, "Touchscreen cannot be initialized", CENTER_MODE);
	//  }

	while (1)
	{
		if (status == TS_OK)
		{
			aLogX[0] = 15;
			aLogY[0] = 15;
			aLogX[1] = LCD_GetXSize() - 15;
			aLogY[1] = LCD_GetYSize() - 15;

			for (i = 0; i < 2; i++)
			{
				GetPhysValues(aLogX[i], aLogY[i], &aPhysX[i], &aPhysY[i]);
			}
			A1 = (1000 * ( aLogX[1] - aLogX[0]))/ ( aPhysX[1] - aPhysX[0]);
			B1 = (1000 * aLogX[0]) - A1 * aPhysX[0];

			A2 = (1000 * ( aLogY[1] - aLogY[0]))/ ( aPhysY[1] - aPhysY[0]);
			B2 = (1000 * aLogY[0]) - A2 * aPhysY[0];

			Calibration_Done = 1;
			return;
		}

		HAL_Delay(5);
	}
}

/**
 * @brief  Display calibration hint
 * @param  None
 * @retval None
 */
static void TouchscreenCalibration_SetHint(void)
{
	/* Clear the LCD */
	LCD_Clear(LCD_COLOR_WHITE);

	/* Set Touchscreen Demo description */
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetBackColor(LCD_COLOR_WHITE);

	LCD_SetFont(&Font12);
	LCD_DisplayStringAt(0, LCD_GetYSize()/2 - 27, "Before using the Touchscreen", CENTER_MODE);
	LCD_DisplayStringAt(0, LCD_GetYSize()/2 - 12, "you need to calibrate it.", CENTER_MODE);
	LCD_DisplayStringAt(0, LCD_GetYSize()/2 + 3,  "Press on the black circles", CENTER_MODE);
}

/**
 * @brief  Get Physical position
 * @param  LogX : logical X position
 * @param  LogY : logical Y position
 * @param  pPhysX : Physical X position
 * @param  pPhysY : Physical Y position
 * @retval None
 */
static void GetPhysValues(int16_t LogX, int16_t LogY, int16_t * pPhysX, int16_t * pPhysY) 
{
	/* Draw the ring */
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_FillCircle(LogX, LogY, 5);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_FillCircle(LogX, LogY, 2);

	/* Wait until touch is pressed */
	WaitForPressedState(1);

	TS_GetState(&TS_State);
	*pPhysX = TS_State.X;
	*pPhysY = TS_State.Y;

	/* Wait until touch is released */
	WaitForPressedState(0);
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_FillCircle(LogX, LogY, 5);
}

/**
 * @brief  Wait For Pressed State
 * @param  Pressed: Pressed State
 * @retval None
 */
static void WaitForPressedState(uint8_t Pressed) 
{
	TS_StateTypeDef  State;

	do
	{
		TS_GetState(&State);
		HAL_Delay(10);
		if (State.TouchDetected == Pressed)
		{
			uint16_t TimeStart = HAL_GetTick();
			do {
				TS_GetState(&State);
				HAL_Delay(10);
				if (State.TouchDetected != Pressed)
				{
					break;
				} else if ((HAL_GetTick() - 100) > TimeStart)
				{
					return;
				}
			} while (1);
		}
	} while (1);
}

/**
 * @brief  Calibrate X position
 * @param  x: X position
 * @retval calibrated x
 */
uint16_t TS_Calibration_GetX(uint16_t x)
{
	return (((A1 * x) + B1)/1000);
}

/**
 * @brief  Calibrate Y position
 * @param  y: Y position
 * @retval calibrated y
 */
uint16_t TS_Calibration_GetY(uint16_t y)
{
	return (((A2 * y) + B2)/1000);
}

/**
 * @brief  Check if the TS is calibrated
 * @param  None
 * @retval calibration state
 */
uint8_t TS_IsCalibrationDone(void)
{
	return (Calibration_Done);
}

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
