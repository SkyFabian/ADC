//
// AIIT 4JG
// Inhalt: Plot der Messwerte
//
// Hardware: STM32F429I-Discovery  Board
// IDE:	MDK 4.72a
//

#ifndef SCOPE_H
#define SCOPE_H

/* Defines -------------------------------------------------------------------*/
#define SET_OFFSET_TO_ZERO     1

/* Includes ------------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void scopeInit(void);
void scopeTask(void);
void PlotData(int nrMData,          // Anzahl der Messreihen 
							float* data1,         // zu zeichnende Messreihe1
							float* data2,         // zu zeichnende Messreihe2
							float* data3,         // zu zeichnende Messreihe3
							int nrOfData);        // Anzahl der Messwerte

#endif // SCOPE_H
