//
// AIIT 4JG
// Inhalt: Plot der Messwerte
//
// Hardware: STM32F429I-Discovery  Board
// IDE:	MDK 4.72a
//

// Includes ------------------------------------------------------------------
#include "scope.h"
#include <stdio.h>

#include "stm32f429i_discovery_lcd.h"


// Defines --------------------------------------------------------------------
#define NR_SAMPLES		256
// Variables ------------------------------------------------------------------
static uint32_t gCh1Samples[NR_SAMPLES];
static float gCh1Values[NR_SAMPLES];


// Function Declarations ------------------------------------------------------
static void RccInit(void);
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
	RccInit();
	GpioInit();
	DmaInit();
	AdcInit();
}

/**
  * Task function of module scope.
	* @param None
  * @retval None
  */
void scopeTask(void){
	if (GetUserButtonPressed() == 1) {
		//TODO: Clear OVR, EOC and STRT flags in ADC



		//TODO: Re-Initialize DMA and ADC and start conversion




	}
	if (DMA_GetFlagStatus(DMA2_Stream2, DMA_FLAG_TCIF2) == SET) {
		DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
		ADC_Cmd(ADC2, DISABLE);
		//TODO: calculate voltage values and plot the result





	}

}

/**
  * @brief  Enable clock for all peripherals which are used
  * @param  None
  * @retval None
  */
static void RccInit(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
}

/**
  * @brief  Initialisierung der GPIOs fuer die Anwendung
  * @param  None
  * @retval None
  */
static void GpioInit(void)
{
	GPIO_InitTypeDef pc3;
  pc3.GPIO_Pin = GPIO_Pin_3;
	pc3.GPIO_Mode = GPIO_Mode_AN;
	pc3.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &pc3);
}

/**
  * @brief  Initialize DMA transfer
  * @param  None
  * @retval None
  */
static void DmaInit(void) {
	DMA_InitTypeDef dma;

	DMA_Cmd(DMA2_Stream2, DISABLE);
	DMA_DeInit(DMA2_Stream2);

	//TODO: Have a look at the following settings and understand them!!
	dma.DMA_Channel = DMA_Channel_1;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(ADC2->DR);				// <-- !!
	dma.DMA_Memory0BaseAddr = (uint32_t)gCh1Samples;					// <-- !!
	dma.DMA_DIR = DMA_DIR_PeripheralToMemory;									// <-- !!
	dma.DMA_BufferSize = NR_SAMPLES;													// <-- !!
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				// <-- !!
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;									// <-- !!
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	// <-- !!
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;					// <-- !!
	dma.DMA_Mode = DMA_Mode_Normal;
	dma.DMA_Priority = DMA_Priority_High;
	dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dma.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream2, &dma);
	DMA_Cmd(DMA2_Stream2, ENABLE);

}

/**
  * @brief  Initialisierung von ADC fuer die Anwendung
  * @param  None
  * @retval None
  */
static void AdcInit(void) {
	// TODO: Initialize ADCx for single conversion
	ADC_CommonInitTypeDef adcCommon;
	ADC_InitTypeDef       adc1;
  // Gemeinsame Peripherie f�r alle ADCs initialisieren
  // ADCs im Multimode oder unabh�ngig betreiben
	adcCommon.ADC_Mode = ADC_Mode_Independent;
	// ABP2 clock wird als Basis verwendet, mit dem Prescaler wird das Taktsignal
	// geteilt: ADC_Prescaler_Div4 -> fADCCLK = fAPB2 / 4
	adcCommon.ADC_Prescaler = ADC_Prescaler_Div4;
	adcCommon.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	adcCommon.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&adcCommon);

	// ADC2 mit 12bit initialisieren
	adc1.ADC_Resolution = ADC_Resolution_12b;
	adc1.ADC_ScanConvMode = DISABLE;
	adc1.ADC_ContinuousConvMode = ENABLE;
	adc1.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	// Ausrichtung der 12 Bit im Register
	adc1.ADC_DataAlign = ADC_DataAlign_Right;
	adc1.ADC_NbrOfConversion = 1;
	ADC_Init(ADC2, &adc1);

	// ADC2 aktivieren
	// ADC Kanal konfigurieren
	ADC_RegularChannelConfig(ADC2, ADC_Channel_13, 1, ADC_SampleTime_480Cycles);

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC2, ENABLE);
	/* Enable ADC2 DMA */
	ADC_DMACmd(ADC2, ENABLE);
	// Peripherie aktivieren
	ADC_Cmd(ADC2, ENABLE);

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
  LCD_SetColors(LCD_COLOR_BLUE, LCD_COLOR_BLUE);
  LCD_DrawFullRect(plotOffsetX, plotOffsetY - plotSizeY , plotSizeX, plotSizeY);

     // Ausgabe am Display
	// Rechteck ausgeben
	LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLUE);
	LCD_DrawRect(plotOffsetX, plotOffsetY - plotSizeY , plotSizeY, plotSizeX);

     // Raster f�r y(t)
  rasterY = (float)plotSizeY / (float) nrOfRasterY;
	xCoord1 = plotOffsetX;
	xCoord2 = plotOffsetX + plotSizeX;

  for (i=1; i<nrOfRasterY; i++) {
	  yCoord1 = plotOffsetY - i*rasterY;
		yCoord2 = yCoord1;

		LCD_DrawUniLine(xCoord1, yCoord1, xCoord2, yCoord2);
	}

  //    Nulllinie darstellen (Zeitachse)
  yCoord1 = (float)plotOffsetY - (float)plotSizeY/(float)2.0;
  yCoord2 = yCoord1;
  LCD_DrawUniLine(xCoord1, yCoord1+1, xCoord2, yCoord2+1);
  LCD_DrawUniLine(xCoord1, yCoord1, xCoord2, yCoord2);
  LCD_DrawUniLine(xCoord1, yCoord1-1, xCoord2, yCoord2-1);

  // Raster f�r t
  rasterX = (float)plotSizeX / (float) nrOfRasterX;
	yCoord1 = plotOffsetY;
	yCoord2 = plotOffsetY - plotSizeY;

  for (i=1; i < nrOfRasterX; i++) {
    xCoord1 = plotOffsetX + i * rasterX;
    xCoord2 = xCoord1;

    // 3) Zeichnen mit LCD_DrawUniLine (...)
    LCD_DrawUniLine(xCoord1, yCoord1, xCoord2, yCoord2);
	}

  //    Nulllinie darstellen (y(t)-Achse)
  xCoord1 = (float)plotOffsetX + (float)plotSizeX/(float)2.0;
  xCoord2 = xCoord1;

  LCD_DrawUniLine(xCoord1-1, yCoord1, xCoord2-1, yCoord2);
  LCD_DrawUniLine(xCoord1, yCoord1, xCoord2, yCoord2);
  LCD_DrawUniLine(xCoord1+1, yCoord1, xCoord2+1, yCoord2);

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
	  LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_BLUE);

    for (i=0;i<(nrOfData-1); i++) {
      xCoord1 = plotOffsetX + i/** dataDeltaMs*/* skalaX;
      yCoord1 = plotOffsetY - plotSizeY / 2.0 - data1[i] * skalaY;
      xCoord2 = plotOffsetX + (i+1)/** dataDeltaMs*/* skalaX;
      yCoord2 = plotOffsetY - plotSizeY / 2.0 - data1[i+1] * skalaY;

      // 3) Zeichnen mit LCD_DrawUniLine (...)
      LCD_DrawUniLine(xCoord1, yCoord1, xCoord2, yCoord2);
      LCD_DrawUniLine(xCoord1, yCoord1-2, xCoord2, yCoord2-2);
      LCD_DrawUniLine(xCoord1, yCoord1-1, xCoord2, yCoord2-1);
      LCD_DrawUniLine(xCoord1, yCoord1+1, xCoord2, yCoord2+1);
      LCD_DrawUniLine(xCoord1, yCoord1+2, xCoord2, yCoord2+2);
	  }
  }

  if (nrMData >= 2) {

	  LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLUE);

    for (i=0;i<(nrOfData-1); i++) {
      xCoord1 = plotOffsetX + i/** dataDeltaMs*/* skalaX;
      yCoord1 = plotOffsetY - plotSizeY / 2.0 - data2[i] * skalaY;
      xCoord2 = plotOffsetX + (i+1)/** dataDeltaMs*/* skalaX;
      yCoord2 = plotOffsetY - plotSizeY / 2.0 - data2[i+1] * skalaY;

      // 3) Zeichnen mit LCD_DrawUniLine (...)
      LCD_DrawUniLine(xCoord1, yCoord1, xCoord2, yCoord2);
      LCD_DrawUniLine(xCoord1, yCoord1-2, xCoord2, yCoord2-2);
      LCD_DrawUniLine(xCoord1, yCoord1-1, xCoord2, yCoord2-1);
      LCD_DrawUniLine(xCoord1, yCoord1+1, xCoord2, yCoord2+1);
      LCD_DrawUniLine(xCoord1, yCoord1+2, xCoord2, yCoord2+2);
	  }
  }

  if (nrMData >= 3) {

	  LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLUE);

    for (i=0;i<(nrOfData-1); i++) {
      xCoord1 = plotOffsetX + i/** dataDeltaMs*/* skalaX;
      yCoord1 = plotOffsetY - plotSizeY / 2.0 - data3[i] * skalaY;
      xCoord2 = plotOffsetX + (i+1)/** dataDeltaMs*/* skalaX;
      yCoord2 = plotOffsetY - plotSizeY / 2.0 - data3[i+1] * skalaY;

      // 3) Zeichnen mit LCD_DrawUniLine (...)
      LCD_DrawUniLine(xCoord1, yCoord1, xCoord2, yCoord2);
      LCD_DrawUniLine(xCoord1, yCoord1-2, xCoord2, yCoord2-2);
      LCD_DrawUniLine(xCoord1, yCoord1-1, xCoord2, yCoord2-1);
      LCD_DrawUniLine(xCoord1, yCoord1+1, xCoord2, yCoord2+1);
      LCD_DrawUniLine(xCoord1, yCoord1+2, xCoord2, yCoord2+2);
	  }
  }

	LCD_SetColors(LCD_COLOR_YELLOW, LCD_COLOR_BLUE);
	LCD_SetFont(&Font8x12);
  LCD_SetPrintPosition(16,0);
	printf(" Volts/DIV :%5.2f V", voltsPerDiv);

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
