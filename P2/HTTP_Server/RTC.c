#include "RTC.h"
#include "cmsis_os2.h" 
#include "time.h"
#include "rl_net.h"

RTC_HandleTypeDef RtcHandle;
RTC_AlarmTypeDef alarmRTC;
//RTC_AlarmTypeDef alarmSyncSNTP;
char fechaRTC[20];
char horaRTC[20];
extern osThreadId_t TID_RTC;
int error;//variable para depuración

 // Timer
extern TIM_HandleTypeDef timer6;

/****SNTP*****/
const NET_ADDR4 ntp_server = { NET_ADDR_IP4, 0, 185,179,104,7};
static void time_callback (uint32_t seconds, uint32_t seconds_fraction);

void init_RTC(){
	  /*##-1- Configure the RTC peripheral #######################################*/
  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
      - Hour Format    = Format 24
      - Asynch Prediv  = Value according to source clock
      - Synch Prediv   = Value according to source clock
      - OutPut         = Output Disable
      - OutPutPolarity = High Polarity
      - OutPutType     = Open Drain */ 
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
	HAL_RTC_MspInit(&RtcHandle);
	get_time();

  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  /*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
  /* Read the Back Up Register 1 Data */
  if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1) != 0x32F2)
  {
    /* Configure RTC Calendar */
    RTC_CalendarConfig();
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
    {
      /* Turn on LED2: Power on reset occurred */
      LED_On(1);
    }
    /* Check if Pin Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {
      /* Turn on LED1: External reset occurred */
      LED_On(0);
    }
    /* Clear source Reset Flag */
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }
		//Configuración de la alarma A
	alarmRTC.AlarmTime.Hours = 0x00;
  alarmRTC.AlarmTime.Minutes = 0x00;
  alarmRTC.AlarmTime.Seconds = 0x00;
  alarmRTC.AlarmTime.SubSeconds = 0x00;
	alarmRTC.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
  alarmRTC.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  alarmRTC.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  alarmRTC.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY|RTC_ALARMMASK_HOURS|RTC_ALARMMASK_MINUTES;
  alarmRTC.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  alarmRTC.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  alarmRTC.AlarmDateWeekDay = 0x01;
  alarmRTC.Alarm = RTC_ALARM_A;
	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	if(HAL_RTC_SetAlarm_IT(&RtcHandle,&alarmRTC,RTC_FORMAT_BIN)!= HAL_OK){
	    Error_Handler();
	}
}

void Error_Handler(void)
{
	LED_On(2);
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}
void RTC_CalendarConfig(void)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: 18 - 3 -2023*/
  sdatestructure.Year = 0x23;
  sdatestructure.Month = RTC_MONTH_MARCH;
  sdatestructure.Date = 0x18;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = 0x02;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
	

}

/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
 void RTC_CalendarShow(void){
	int i;
	 
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
	 
	borrarBuffer();
  empezar();
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
	 
  /* Display time Format : hh:mm:ss */
  sprintf(horaRTC, "%2d:%2d:%2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format : mm-dd-yy */
  sprintf(fechaRTC, "%d-%d-%d",sdatestructureget.Date , sdatestructureget.Month, 2000 + sdatestructureget.Year);
	
	for(i=0; horaRTC[i]!= 0x00; i++){
          EscribeLetra_L1(horaRTC[i]);
  }
	for(i=0; fechaRTC[i]!= 0x00; i++){
          EscribeLetra_L2(fechaRTC[i]);
  }
	LCD_update();

}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  /* To change the source clock of the RTC feature (LSE, LSI), You have to:
     - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
     - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
       configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
       __HAL_RCC_BACKUPRESET_RELEASE().
     - Configure the needed RTc clock source */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  
  /*##-2- Configure LSE as RTC clock source ###################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
    Error_Handler();
  }
  
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
    Error_Handler();
  }
  
  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
}

/**
  * @brief RTC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_RTC_DISABLE();

  /*##-2- Disables the PWR Clock and Disables access to the backup domain ###################################*/
  HAL_PWR_DisableBkUpAccess();
  __HAL_RCC_PWR_CLK_DISABLE();
  
}
//conrol de interrupciones de la alarma del RTC
void RTC_Alarm_IRQHandler(void) {
  HAL_RTC_AlarmIRQHandler(&RtcHandle);
}
//callback de la alarma A
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){
	osThreadFlagsSet (TID_RTC, 0x02);
}

/*****************SNTP**************************/

 
void get_time (void) {
  if (netSNTPc_GetTime ((NET_ADDR *)&ntp_server, time_callback) == netOK) {
//    printf ("SNTP request sent.\n");
		error=0;
  }
  else {
    //printf ("SNTP not ready or bad parameters.\n");
		error=1;
  }
}
 
static void time_callback (uint32_t seconds, uint32_t seconds_fraction) {
  time_t tiempo = seconds;
  struct tm timeSNTP;
	RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;
	if (seconds == 0) {
    //printf ("Server not responding or bad response.\n");
		error=1;
  }
  else {
    //printf ("%d seconds elapsed since 1.1.1970\n", seconds);
		timeSNTP = *localtime(&tiempo);  //timeSNTP apunta a la estructura "tiempo"
		sdatestructure.Date = timeSNTP.tm_mday;
		sdatestructure.Month = timeSNTP.tm_mon +1; //mes del año del 0-11
		sdatestructure.Year = timeSNTP.tm_year-100;
		if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN) != HAL_OK){
			/* Initialization Error */
			Error_Handler();
		}
		stimestructure.Hours = timeSNTP.tm_hour+1;
		stimestructure.Minutes = timeSNTP.tm_min;
		stimestructure.Seconds = timeSNTP.tm_sec;
		if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN) != HAL_OK){
			/* Initialization Error */
			Error_Handler();
		}
  }
}
void initTimer6(void){
 // Configurar y arrancar el timer para generar un evento pasados n_microsegundos
	timer6.Instance = TIM6;
	timer6.Init.Prescaler = 5999;//Prescaler=84M/6000=14khz
	timer6.Init.Period = 25219999;//14k/2520000=0.00555Hz=180s=3min
  __HAL_RCC_TIM6_CLK_ENABLE();
	HAL_TIM_Base_Init(&timer6);//estas dos lineas sirven para arrancar el timer
	HAL_TIM_Base_Start(&timer6);
}
