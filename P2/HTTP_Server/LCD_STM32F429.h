#ifndef __LCD_STM32F429_H
#define __LCD_STM32F429_H

#include "stdio.h"
#include "string.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "stm32f4xx_hal.h"



/*Cabeceras de funciones del LCD*/
 void delay(uint32_t n_microsegundos);
 void init_SPI(void);
 void init_LCD(void);
 void reset_lcd(void);
 void LCD_wr_data(unsigned char data);
 void LCD_wr_cmd(unsigned char cmd);
 void LCD_update(void);
 void EscribeLetra_L1(uint8_t letra);
 void EscribeLetra_L2(uint8_t letra);
 void EscribeLetra_LCD(uint8_t linea, uint8_t letra);
 void borrarBuffer(void);
 void empezar(void);
#endif
