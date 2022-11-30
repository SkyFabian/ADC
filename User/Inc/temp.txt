/*
 * scope.h
 *
 *  Created on: 29.11.2022
 *      Author: ber
 */

#ifndef INC_TEMP_TXT_
#define INC_TEMP_TXT_

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
void PlotData(int nrMData,          // number of arrays provided to the function
							float* data1,         // measurement values array 1
							float* data2,         // measurement values array 2
							float* data3,         // measurement values array 3
							int nrOfData);        // number of values in each array


#endif /* INC_TEMP_TXT_ */
