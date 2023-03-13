#include "stm32f4xx_hal.h"
#ifndef __ADC_H
void ADC_Initialize(void);
int32_t ADC_Uninitialize (void);
int32_t ADC_StartConversion (void);
int32_t ADC_ConversionDone (void);
int32_t ADC_GetValue (void) ;
uint32_t ADC_GetResolution (void);
#endif
