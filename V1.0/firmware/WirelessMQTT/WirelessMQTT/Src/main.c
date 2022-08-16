/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "misc.h"
#include "at_command.h"
#include "mqtt_app.h"
#include "my_lcd.h"
#include "my_lcd_init.h"
#include <stdio.h>
#include <stdlib.h>
#include "gui.h"
#include "pic.h"
#include "bsp_bh1750.h"
#include "bsp_sgp30.h"
#include "24l01.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define printf(...)    
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

mqtt_app_t  mqttAppObj;
uint16_t timer_1s=1000;
uint8_t timer_100ms=100;
uint8_t timer_10ms=10;

uint32_t timer=0;

uint32_t systick=0;
u8 tmp_buf[6];	
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
   uint8_t data=3;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
    
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_SPI1_Init();
  
  /* USER CODE BEGIN 2 */
    
    printf("system start......\r\n");
    dwt_init();
    
    iic_ll_init();
    
    GUI_Init();  //初始化
    GUI_SetBkColor(GUI_WHITE);

    GUI_Clear();
    //GUI_DispStringAt("hello ucGUI",0,0);
    //GUI_DispChars('*',30);
    
    LCD_ShowPicture(0,0,120,160,gImage_background);
//    LCD_ShowPicture(64,10,32,32,gImage_signal);
//    LCD_ShowPicture(10,90,32,32,gImage_planet);
//    LCD_ShowPicture(64,90,20,20,gImage_tree);
//    GUI_Delay(5000);
   
//    LCD_ShowPicture(0,0,20,20,gImage_lte_signal);
//    LCD_ShowPicture(40,0,20,20,gImage_wifi_signal);
    
    //GUI_SetBkColor(GUI_GREEN);
    //GUI_Clear();

    //draw lte logo
    GUI_SetPenSize(4);
    GUI_SetColor(GUI_BLACK);
    GUI_DrawLine(1,16,1,20);
    GUI_DrawLine(6,12,6,20);
    GUI_DrawLine(11,6,11,20);
    GUI_DrawLine(16,1,16,20);
    
    //draw wifi logo
    GUI_SetPenSize(2);
    GUI_DrawArc(50,20,1,1,60,120);
    GUI_DrawArc(50,20,6,6,60,120);
    GUI_DrawArc(50,20,12,12,60,120);
    GUI_DrawArc(50,20,17,17,60,120);
    
    GUI_SetBkColor(GUI_BLACK);
    GUI_SetColor(GUI_WHITE);
    
//    GUI_SetTextMode(GUI_TM_NORMAL);
//    GUI_DispStringHCenterAt("GUI_TM_NORMAL",64,30);
//    GUI_SetTextMode(GUI_TM_REV);
//    GUI_DispStringHCenterAt("GUI_TM_REV",64,60);
    GUI_SetTextMode(GUI_TM_TRANS);
//    GUI_DispStringHCenterAt("GUI_TM_TRANS",64,90);
//    GUI_SetTextMode(GUI_TM_XOR);
//    GUI_DispStringHCenterAt("GUI_TM_XOR",64,120);
        //GUI_SetTextMode(GUI_TM_NORMAL);
    
    
    //LCD_Fill(0,0,100,100,BLUE);
    //LCD_ShowChinese(20,20,"空气质量监测仪",RED,WHITE,16,0);
    GUI_SetFont(&GUI_Font16B_ASCII);
    GUI_DispStringAt("System Init......",2,30);
    //GUI_SetFont(&GUI_FontComic18B_1);
    //GUI_DispStringHCenterAt("System Init......",64,50);
//    GUI_SetFont(&GUI_Font4x6);
//    GUI_DispStringHCenterAt("System Init......",64,40);
//    GUI_SetFont(&GUI_Font6x8);
//    GUI_DispStringAt("System Init......",2,50);
//    GUI_SetFont(&GUI_Font10S_ASCII);
//    GUI_DispStringAtCEOL("System Init......",5,60);
//    GUI_SetFont(&GUI_Font10_ASCII);
//    GUI_DispStringHCenterAt("System Init......",64,70);
//    GUI_SetFont(&GUI_Font16_ASCII);
//    GUI_DispStringHCenterAt("System Init......",64,80);
//    GUI_SetFont(&GUI_Font16B_ASCII);
//    GUI_DispStringHCenterAt("System Init......",64,100);
//    GUI_SetFont(&GUI_Font8x16);
//    GUI_DispStringAtCEOL("System Init......",4,120);
    
    GUI_SetFont(&GUI_Font8x16);
    GUI_DispStringAtCEOL("SGP30 Init......",0,50);
    sgp30_init();
    sgp30_get_feature();    
    sgp30_get_id();
    
    if(sgp30.sgp30_status == SGP30_STATUS_OK)
        GUI_DispStringAtCEOL("SGP30 OK......",0,50);
    else
        GUI_DispStringAtCEOL("SGP30 Error......",0,50);
    delay_ms(500);
    
    GUI_DispStringAtCEOL("BH1750 Init......",0,70);
    
    bh1750_init();
    bh1750_start();
    if(brigh.brigh_status==BRIGH_STATUS_OK)
        GUI_DispStringAtCEOL("BH1750 OK......",0,70);
    else
        GUI_DispStringAtCEOL("BH1750 Error......",0,70);
    delay_ms(500);
    
    GUI_DispStringAt("LTE check......",0,90);
    if(lte_check())
        GUI_DispStringAt("LTE check OK......",0,90);
    else
        GUI_DispStringAt("LTE check Error......",0,90);
    delay_ms(500);
    
    GUI_DispStringAt("ESP8266 check......",0,110);
    if(esp8266_check())
        GUI_DispStringAt("wifi check OK......",0,110);
    else
        GUI_DispStringAt("wifi check Error......",0,110);
    delay_ms(500);
    
    NRF24L01_Init();    		    	//初始化NRF24L01 
    GUI_DispStringAt("24L01 check......",0,130);
    if(NRF24L01_Check())
        GUI_DispStringAt("24L01 check Error......",0,130);
    else
        GUI_DispStringAt("24L01 check OK......",0,130);
    NRF24L01_RX_Mode();
    
    data=3;
    while(data >0)
    {
        LED1_ON;
        LED2_OFF;
        delay_ms(500);
        LED1_OFF;
        LED2_ON;
        delay_ms(500);

        data--;
    }
    
    mqtt_app_init(&mqttAppObj,MQTT_TRANS_LTE);
    delay_ms(500);
    HAL_TIM_Base_Start_IT(&htim3);

    GUI_Clear();
    GUI_SetTextMode(GUI_TM_NORMAL);
    GUI_SetFont(&GUI_Font8x16);
    GUI_SetBkColor(GUI_BLUE);
    GUI_DispStringAt("CO2eq:",10,10);
    GUI_DispStringAt("ppm",105,10);
    GUI_DispStringAt("TVOC :",10,30);
    GUI_DispStringAt("ppb",105,30);
    
    GUI_DispStringAt("Brigh:",10,50);
    GUI_DispStringAt("lux",105,50);
    GUI_DispStringAt("PM25 :",10,70);
    GUI_DispStringAt("ug/m3",105,70);    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      if(timer_10ms == 0)
      {
          timer_10ms=10;
          
          sgp30_get_meas();
          bh1750_read();
          
        GUI_GotoXY(60,10);
        GUI_DispDecSpace(sgp30.sgp30Data.co2,5);

        GUI_GotoXY(60,30);
        GUI_DispDecSpace(sgp30.sgp30Data.tvoc,5);

        GUI_GotoXY(60,50);
        GUI_DispFloat(brigh.lux,5);
      }
      
      
    if(timer_1s == 0)
    {
        timer_1s=1000;
        timer++;
        systick++;
        bh1750_read();
        if(timer>=200)
        {
            timer=0;
            if(mqttAppObj.MQStatus == MQTT_STATUS_SERVER_CONNECT_OK)
                mqtt_app_publish(&mqttAppObj,MQTT_CLIENT_USER_PUB1);
            else
                printf("connect server fail!\r\n");
        }
        
        GUI_DispDecAt(data++,10,100,3);
    }
    
    if(NRF24L01_RxPacket(tmp_buf)==0)//一旦接收到信息,则显示出来.
    {
        tmp_buf[5]=0;//加入字符串结束符
        //LCD_ShowString(0,190,lcddev.width-1,32,16,tmp_buf);  
        GUI_DispStringAt(tmp_buf,10,120);        
    }
    //HAL_Delay(5000);
    
//    if(systick>30)
//    {
//        HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
//        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
//        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
//        HAL_PWR_EnterSTANDBYMode();
//    }        
   
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
