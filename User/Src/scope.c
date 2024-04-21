/*
 * scope.c
 *
 * Plotting of ADC measurement results
 *
 *  Created on: 29.11.2022
 *      Author: ber
 */

// Includes ------------------------------------------------------------------
#include "scope.h"
#include <stdio.h>

#include "stm32f429i_discovery_lcd.h"
#include "stm32f4xx_hal.h"
#include "string.h"


// Defines --------------------------------------------------------------------
#define NR_SAMPLES		256
// Variables ------------------------------------------------------------------
static DMA_HandleTypeDef dmaHandle;
static ADC_HandleTypeDef adcHandle;
static uint32_t ch1Samples[NR_SAMPLES];
static float ch1Values[NR_SAMPLES];
static volatile int dmaFinished = 0;


// Function Declarations ------------------------------------------------------
static void GpioInit(void);
static void DmaInit(void);
static void AdcInit(void);
static int GetUserButtonPressed(void);

// Functions ------------------------------------------------------------------
/**
 * Initialization Code for module scope
 * @param None
 * @retval None
 */
void scopeInit(void) {
	GpioInit();
	AdcInit();
	DmaInit();

	memset(ch1Samples, 0, NR_SAMPLES*4);

}

/**
 * Task function of module scope.
 * @param None
 * @retval None
 */
void scopeTask(void){
	// TODO: start measurement if user button is pressed


	// TODO: if new measurement data is available calculate the voltage and show it on
	// on the display using PlotData() function


}

/**
 * @brief  This function handles DMA interrupt request.
 * @param  None
 * @retval None
 */
void DMA2_Stream1_IRQHandler(void)
{
	// Let HAL take care of interrupt flags
	HAL_DMA_IRQHandler(adcHandle.DMA_Handle);
}

/**
 * @brief  Conversion complete callback in non blocking mode
 * @param  AdcHandle : AdcHandle handle
 * @retval None
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
	// Callback when (all) ADC conversions finished / DMA transfer complete
	// TODO: signal to main loop, that new measurement data is available in ch1Samples array

}

/**
 * @brief  Configure GPIOs
 * @param  None
 * @retval None
 */
static void GpioInit(void)
{
	// PF6: LDR on extension board
	__HAL_RCC_GPIOF_CLK_ENABLE();
	GPIO_InitTypeDef in;
	in.Mode = GPIO_MODE_ANALOG;
	in.Pull = GPIO_NOPULL;
	in.Alternate = 0;
	in.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOF, &in);
}

/**
 * @brief  Configure DMA
 * note: uses adcHandle, therefore AdcInit has to be called before
 * @param  None
 * @retval None
 */
static void DmaInit(void) {
	__HAL_RCC_DMA2_CLK_ENABLE();

	dmaHandle.Instance = DMA2_Stream1;
	dmaHandle.Init.Channel  = DMA_CHANNEL_2;
	dmaHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;
	dmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;
	dmaHandle.Init.MemInc = DMA_MINC_ENABLE;
	dmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	dmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	dmaHandle.Init.Mode = DMA_NORMAL;
	dmaHandle.Init.Priority = DMA_PRIORITY_HIGH;
	dmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	dmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
	dmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;
	dmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;

	HAL_DMA_Init(&dmaHandle);

	/* Associate the initialized DMA handle to the ADC handle */
	__HAL_LINKDMA(&adcHandle, DMA_Handle, dmaHandle);

	/* NVIC configuration for DMA transfer complete interrupt */
	HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);

}

/**
 * @brief  Configure ADC
 * @param  None
 * @retval None
 */
static void AdcInit(void) {
	// NOTE: This configuration samples ~5ms into data array. To get higher
	//  values we would need bigger array or trigger ADC with timer.

	__HAL_RCC_ADC3_CLK_ENABLE();
	/*##-1- Configure the ADC peripheral #######################################*/
	adcHandle.Instance                   = ADC3;
	adcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4;
	adcHandle.Init.Resolution            = ADC_RESOLUTION12b;
	adcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
	adcHandle.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode disabled to have only 1 conversion at each conversion trig */
	adcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
	adcHandle.Init.NbrOfDiscConversion   = 0;
	adcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Conversion start trigged at each external event */
	adcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
	adcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	adcHandle.Init.NbrOfConversion       = 1;
	adcHandle.Init.DMAContinuousRequests = ENABLE;
	adcHandle.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&adcHandle);	


	/*##-2- Configure ADC regular channel ######################################*/
	ADC_ChannelConfTypeDef sConfig;
	sConfig.Channel      = ADC_CHANNEL_4;
	sConfig.Rank         = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset       = 0;

	HAL_ADC_ConfigChannel(&adcHandle, &sConfig);
}


/**
 * @brief Plots up to three data arrays to the screen
 *
 * @param nrMData Number of arrays to plot, eg: value 1 means only data in array
 * data1 is plotted, value 3 results in all 3 data arrays to be plotted.
 * @param data1 Data points of signal 1
 * @param data2 Data points of signal 2
 * @param data3 Data points of signal 3
 * @param nrOfData size of data arrays
 */
void PlotData (int nrMData, float* data1, float* data2, float* data3,
		int nrOfData) {

	int i;
	int plotSizeX, plotSizeY;
	int plotOffsetX, plotOffsetY;
	int nrOfRasterX, nrOfRasterY;
	float skalaX, skalaY;
	float rasterX, rasterY;
	float maxh;
	float voltsPerDiv;
	int xCoord1, yCoord1, xCoord2, yCoord2;

	plotSizeX = 220;
	plotSizeY = 140;
	plotOffsetX = 10;
	plotOffsetY = 180;
	nrOfRasterY = 8;
	nrOfRasterX = 10;

	// Ausgabefl�che l�schen
	LCD_SetColors(LCD_COLOR_BLACK, LCD_COLOR_BLACK);
	LCD_FillRect(plotOffsetX, plotOffsetY - plotSizeY , plotSizeX, plotSizeY);

	// Ausgabe am Display
	// Rechteck ausgeben
	LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLACK);
	LCD_DrawRect(plotOffsetX, plotOffsetY - plotSizeY , plotSizeX, plotSizeY);

	// Raster f�r y(t)
	rasterY = (float)plotSizeY / (float) nrOfRasterY;
	xCoord1 = plotOffsetX;
	xCoord2 = plotOffsetX + plotSizeX;

	for (i=1; i<nrOfRasterY; i++) {
		yCoord1 = plotOffsetY - i*rasterY;
		yCoord2 = yCoord1;

		LCD_DrawLine(xCoord1, yCoord1, xCoord2, yCoord2);
	}

	//    Nulllinie darstellen (Zeitachse)
	yCoord1 = (float)plotOffsetY - (float)plotSizeY/(float)2.0;
	yCoord2 = yCoord1;
	LCD_DrawLine(xCoord1, yCoord1+1, xCoord2, yCoord2+1);
	LCD_DrawLine(xCoord1, yCoord1, xCoord2, yCoord2);
	LCD_DrawLine(xCoord1, yCoord1-1, xCoord2, yCoord2-1);

	// Raster f�r t
	rasterX = (float)plotSizeX / (float) nrOfRasterX;
	yCoord1 = plotOffsetY;
	yCoord2 = plotOffsetY - plotSizeY;

	for (i=1; i < nrOfRasterX; i++) {
		xCoord1 = plotOffsetX + i * rasterX;
		xCoord2 = xCoord1;

		// 3) Zeichnen mit LCD_DrawLine (...)
		LCD_DrawLine(xCoord1, yCoord1, xCoord2, yCoord2);
	}

	//    Nulllinie darstellen (y(t)-Achse)
	xCoord1 = (float)plotOffsetX + (float)plotSizeX/(float)2.0;
	xCoord2 = xCoord1;

	LCD_DrawLine(xCoord1-1, yCoord1, xCoord2-1, yCoord2);
	LCD_DrawLine(xCoord1, yCoord1, xCoord2, yCoord2);
	LCD_DrawLine(xCoord1+1, yCoord1, xCoord2+1, yCoord2);

	// Ermittlung V/DIV
	//    Min/Max bestimmen
	maxh = 0;
	for (i = 0; i < nrOfData; i++)
	{
		if (maxh < data1[i])
			maxh = data1[i];
		if (data2 != NULL && maxh < data2[i])
			maxh = data2[i];
		if (data3 != NULL && maxh < data3[i])
			maxh = data3[i];
	}

	//    V/DIV bestimmen (Bereich: 1mV .. 100 V)
	//    Achtung: keine Bereichspr�fung
	voltsPerDiv = 100.0;

	while ( (voltsPerDiv * (float)nrOfRasterY/(float)2.0) > (float) maxh) {
		voltsPerDiv = voltsPerDiv / (float)10.0;
	}
	voltsPerDiv = voltsPerDiv * (float)10.0;

	if ( (voltsPerDiv /(float)5.0 * (float)nrOfRasterY/(float)2.0) > (float) maxh) {
		voltsPerDiv = voltsPerDiv / (float)5.0;
	} else if ( (voltsPerDiv /(float)2.0 * (float)nrOfRasterY/(float)2.0) > (float) maxh) {
		voltsPerDiv = voltsPerDiv / (float)2.0;
	}

	// 1) Skalierungsfaktor f�r Darstellung berechnen
	skalaX = (float)plotSizeX / nrOfData/*(float)datatGesamtMs*/;
	skalaY = (float)plotSizeY/(float)(voltsPerDiv*nrOfRasterY);

	// 2) Koordinaten in der Darstellung mit Skalierungsfaktor berechnen

	if (nrMData >= 1) {
		LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLACK);

		for (i=0;i<(nrOfData-1); i++) {
			xCoord1 = plotOffsetX + i/** dataDeltaMs*/* skalaX;
			yCoord1 = plotOffsetY - plotSizeY / 2.0 - data1[i] * skalaY;
			xCoord2 = plotOffsetX + (i+1)/** dataDeltaMs*/* skalaX;
			yCoord2 = plotOffsetY - plotSizeY / 2.0 - data1[i+1] * skalaY;

			// 3) Zeichnen mit LCD_DrawLine (...)
			LCD_DrawLine(xCoord1, yCoord1, xCoord2, yCoord2);
			LCD_DrawLine(xCoord1, yCoord1-2, xCoord2, yCoord2-2);
			LCD_DrawLine(xCoord1, yCoord1-1, xCoord2, yCoord2-1);
			LCD_DrawLine(xCoord1, yCoord1+1, xCoord2, yCoord2+1);
			LCD_DrawLine(xCoord1, yCoord1+2, xCoord2, yCoord2+2);
		}
	}

	if (nrMData >= 2) {

		LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLACK);

		for (i=0;i<(nrOfData-1); i++) {
			xCoord1 = plotOffsetX + i/** dataDeltaMs*/* skalaX;
			yCoord1 = plotOffsetY - plotSizeY / 2.0 - data2[i] * skalaY;
			xCoord2 = plotOffsetX + (i+1)/** dataDeltaMs*/* skalaX;
			yCoord2 = plotOffsetY - plotSizeY / 2.0 - data2[i+1] * skalaY;

			// 3) Zeichnen mit LCD_DrawLine (...)
			LCD_DrawLine(xCoord1, yCoord1, xCoord2, yCoord2);
			LCD_DrawLine(xCoord1, yCoord1-2, xCoord2, yCoord2-2);
			LCD_DrawLine(xCoord1, yCoord1-1, xCoord2, yCoord2-1);
			LCD_DrawLine(xCoord1, yCoord1+1, xCoord2, yCoord2+1);
			LCD_DrawLine(xCoord1, yCoord1+2, xCoord2, yCoord2+2);
		}
	}

	if (nrMData >= 3) {

		LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK);

		for (i=0;i<(nrOfData-1); i++) {
			xCoord1 = plotOffsetX + i/** dataDeltaMs*/* skalaX;
			yCoord1 = plotOffsetY - plotSizeY / 2.0 - data3[i] * skalaY;
			xCoord2 = plotOffsetX + (i+1)/** dataDeltaMs*/* skalaX;
			yCoord2 = plotOffsetY - plotSizeY / 2.0 - data3[i+1] * skalaY;

			// 3) Zeichnen mit LCD_DrawLine (...)
			LCD_DrawLine(xCoord1, yCoord1, xCoord2, yCoord2);
			LCD_DrawLine(xCoord1, yCoord1-2, xCoord2, yCoord2-2);
			LCD_DrawLine(xCoord1, yCoord1-1, xCoord2, yCoord2-1);
			LCD_DrawLine(xCoord1, yCoord1+1, xCoord2, yCoord2+1);
			LCD_DrawLine(xCoord1, yCoord1+2, xCoord2, yCoord2+2);
		}
	}

	LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLACK);
	LCD_SetFont(&Font8);
	LCD_SetPrintPosition(23,0);
	printf("  Volts/DIV :%5.2f V", voltsPerDiv);

	return;

}

/**
 * @brief Check if user button has been pressed
 *
 * @return 1 if user button has been pressed, otherwise 0
 */
static int GetUserButtonPressed () {

	return (GPIOA->IDR & 0x0001);
}
