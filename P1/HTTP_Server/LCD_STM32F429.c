#include "LCD_STM32F429.h"

//Variables del SPI Driver
 extern ARM_DRIVER_SPI Driver_SPI1;
 ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
 // Timer
 TIM_HandleTypeDef timer7;
 unsigned char buffer[512];
 uint16_t posicionL1=0;
 uint16_t posicionL2=256;
 GPIO_InitTypeDef GPIO_InitStruct2;
 
 //función para el retardo
 void delay(uint32_t n_microsegundos){
 // Configurar y arrancar el timer para generar un evento pasados n_microsegundos
	timer7.Instance = TIM7;
	timer7.Init.Prescaler = 20;//Prescaler=84M/21=4Mhz
	timer7.Init.Period = (n_microsegundos/4)-1;
  __HAL_RCC_TIM7_CLK_ENABLE();
	HAL_TIM_Base_Init(&timer7);//estas dos lineas sirven para arrancar el timer
	HAL_TIM_Base_Start(&timer7);
 // Esperar a que se active el flag del registro de Match
	while(TIM7 -> CNT);
 	HAL_TIM_Base_DeInit(&timer7);
	HAL_TIM_Base_Stop(&timer7);
 }
 
 void init_SPI(void){
	 /*Inicialización y configuración del driver SPI para el LCD*/
  SPIdrv->Initialize(NULL);
	SPIdrv->PowerControl(ARM_POWER_FULL);
	SPIdrv->Control(ARM_SPI_MODE_MASTER|ARM_SPI_CPOL1_CPHA1|ARM_SPI_MSB_LSB|ARM_SPI_SS_MASTER_UNUSED|ARM_SPI_DATA_BITS(8),20000000);
    //Configuramos los pines RESET,A0,CS como salida
      //Instanciamos los pines como salida.
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct2.Pin = GPIO_PIN_6;//RESET
	GPIO_InitStruct2.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct2.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct2);
	 
  __HAL_RCC_GPIOF_CLK_ENABLE();
	GPIO_InitStruct2.Pin = GPIO_PIN_13;//A0
	GPIO_InitStruct2.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct2.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct2);
	 
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitStruct2.Pin = GPIO_PIN_14;//CS
	GPIO_InitStruct2.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct2.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct2);
		
	//Inicialmente los pines de RESTE, A0, CS :
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);//CS
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);//RESET
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);//A0

 }
 
 void init_LCD(void){
	  //Display off
	LCD_wr_cmd(0xAE);
	//Polarizacion lcd a 1/9
	LCD_wr_cmd(0xA2);
	//Direccionamiento ram
	LCD_wr_cmd(0xA0);
	//Salidas com 
	LCD_wr_cmd(0xC8);
	//Resistencias internas
	LCD_wr_cmd(0x22);
	//Power on
	LCD_wr_cmd(0x2F);
	//Display en la linea 0
	LCD_wr_cmd(0x40);
	//Dislpay ON
	LCD_wr_cmd(0xAF);
	//Contraste
	LCD_wr_cmd(0x81);
	//Valor contraste
	LCD_wr_cmd(0x13);
	//Display all points normal
	LCD_wr_cmd(0xA4);
	//LCD display normal
	LCD_wr_cmd(0xA6); 
 }
 
 /*Función para escribir un dato en el LCD*/
 void LCD_wr_data(unsigned char data){
	 ARM_SPI_STATUS stat;
 // Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);//CS
 // Seleccionar A0 = 1;
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_SET);//A0
 // Escribir un dato (data) usando la función SPIDrv->Send(…);
	SPIdrv->Send(&data,sizeof(data));//Tamaño en bytes
 // Esperar a que se libere el bus SPI;
	 do{
	 stat = SPIdrv->GetStatus();
 }while(stat.busy);
	 delay(1000);
 // Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);//CS
	 
 }
 
 /*Función para escribir un comando del LCD*/
 void LCD_wr_cmd(unsigned char cmd){
	ARM_SPI_STATUS stat;
 // Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_RESET);//CS
 // Seleccionar A0 = 0;
 HAL_GPIO_WritePin(GPIOF,GPIO_PIN_13,GPIO_PIN_RESET);//A0
 // Escribir un comando (cmd) usando la función SPIDrv->Send(…);
 SPIdrv->Send(&cmd,sizeof(cmd));//Tamaño en bytes
 // Esperar a que se libere el bus SPI;
 
	 do{
	 stat = SPIdrv->GetStatus();
 }while(stat.busy);

 delay(1000);
 // Seleccionar CS = 1;
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,GPIO_PIN_SET);//CS
 }
 