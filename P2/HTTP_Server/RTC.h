#ifndef __RTC_H
#define __RTC_H
#include "LCD_STM32F429.h"
#include "ledsSTM32F429.h"

#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

void init_RTC(void);
 void Error_Handler(void);
 void RTC_CalendarConfig(void);
 void RTC_CalendarShow(void);
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);

#endif
