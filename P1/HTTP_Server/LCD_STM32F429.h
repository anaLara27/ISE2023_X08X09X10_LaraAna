#ifndef __LCD_STM32F429_H
#define __LCD_STM32F429_H

#include "stdio.h"
#include "string.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"
#include "Arial12x12.h"


/*Cabeceras de funciones del LCD*/
 void delay(uint32_t n_microsegundos);
 void init_SPI(void);
 void init_LCD(void);
 void LCD_wr_data(unsigned char data);
 void LCD_wr_cmd(unsigned char cmd);



#endif