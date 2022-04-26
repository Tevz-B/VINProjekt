/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "adc.h"
#include "crc.h"
#include "dac.h"
#include "dma.h"
#include "fdcan.h"
#include "i2c.h"
#include "quadspi.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LED.h"
#include "kbd.h"
#include "sci.h"
#include "lcd.h"
#include "ugui.h"
#include "XPT2046_touch.h"
#include "ColorSpaces.h"
#include "joystick.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void UserPixelSetFunction(UG_S16 x, UG_S16 y, UG_COLOR c)
{
	ILI9341_SetDisplayWindow(x, y, 1, 1);
	ILI9341_SendData((LCD_IO_Data_t *)&c, 1);
}

UG_RESULT _HW_FillFrame_(UG_S16 x, UG_S16 y, UG_S16 w, UG_S16 h, UG_COLOR c)
{
	LCD_FillRect(x, y, w, h, c);

	return UG_RESULT_OK;
}

UG_GUI gui;

void DrawStartScreen()
{

	UG_FillScreen(C_BLACK);

	uint16_t pozicija_y=135, pozicija_x=150;

	UG_FontSelect(&FONT_32X53);
	UG_SetForecolor(C_VIOLET);
	UG_PutString(pozicija_x-90,pozicija_y,"M");
	UG_SetForecolor(C_BLUE);
	UG_PutString(pozicija_x-60,pozicija_y,"i");
	UG_SetForecolor(C_CYAN);
	UG_PutString(pozicija_x-30,pozicija_y,"S");
	UG_SetForecolor(C_GREEN);
	UG_PutString(pozicija_x,pozicija_y,"K");
	UG_SetForecolor(C_YELLOW);
	UG_PutString(pozicija_x+30,pozicija_y,"o");
	UG_SetForecolor(C_RED);
	UG_PutString(pozicija_x+70,pozicija_y,"3");
	UG_SetForecolor(C_WHITE);
	UG_FontSelect(&FONT_16X26);
	UG_PutString(5,pozicija_y+50,"To mi deli, Borut!");
}

float DrawColors(float intensity)
{
	uint16_t Data;
	uint32_t time1, time2;
	float framerate;
	HSV_t HSV;
	RGB_t RGB;

	uint16_t *array;
	uint16_t counter;
	array = (uint16_t *) malloc(180*100);
	counter = 0;
	HSV.V=intensity;

	ILI9341_SetDisplayWindow(0, 0, 180, 100);

	for (int j=0; j<100;j++) //vrstice
	{
		HSV.S=j;
		for (int i=0; i<180;i++) //stolpci
		{
			HSV.H=360-2*i;
			HSVtoRGB(&HSV, &RGB);
			array[counter]=RGB888_to_RGB565(&RGB);
			counter++;
	  }
	}


	time1 = HAL_GetTick();
	for (counter = 0; counter<100*180; counter+=100)
	{
		Data = array[counter];
		array[counter]=0;
		ILI9341_SendData(array, 100*180);
		array[counter] = Data;
	}
	time2=HAL_GetTick()-time1;

	framerate = 180*1000.0/time2;
	free(array);
	return framerate;
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	coord_t joystick_raw, joystick_out;
	joystick_t joystick;
	uint8_t MSG[100]={0};
	uint16_t touch_x = 0, touch_y = 0;

	char str[10];
	float framerate;

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
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_FMC_Init();
  MX_I2C2_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_QUADSPI1_Init();
  MX_SPI1_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM20_Init();
  MX_ADC3_Init();
  MX_DAC1_Init();
  MX_DAC2_Init();
  MX_FDCAN2_Init();
  MX_I2C1_Init();
  MX_TIM15_Init();
  MX_USART3_UART_Init();
  MX_ADC4_Init();
  MX_USB_Device_Init();
  MX_DMA_Init();
  MX_CRC_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  LED_init();
  KBD_init();
  SCI_init();
  joystick_init(&joystick);

  for (uint8_t i=0;i<3;i++)
  {
	  HAL_Delay(250);
	  LEDs_on(0xFF);
	  HAL_Delay(250);
	  LEDs_off(0xFF);
  }

  LCD_Init();
  UG_Init(&gui, UserPixelSetFunction, ILI9341_GetParam(LCD_WIDTH), ILI9341_GetParam(LCD_HEIGHT));
  UG_FontSelect(&FONT_8X12);
  UG_SetForecolor(C_WHITE);
  UG_SetBackcolor(C_BLACK);
  UG_DriverRegister(DRIVER_FILL_FRAME, (void *)_HW_FillFrame_);
  UG_DriverEnable(DRIVER_FILL_FRAME);

  DrawStartScreen();
  framerate = DrawColors(80);

  UG_SetForecolor(C_WHITE);
  UG_FontSelect(&FONT_16X26);
  sprintf(str,"%.0f fps",framerate);
  UG_PutString(5,105,str);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  LED_set(LED0, !KBD_get_button_state(BTN_OK));
	  LED_set(LED1, !KBD_get_button_state(BTN_DOWN));
	  LED_set(LED2, !KBD_get_button_state(BTN_RIGHT));
	  LED_set(LED3, !KBD_get_button_state(BTN_UP));
	  LED_set(LED4, !KBD_get_button_state(BTN_LEFT));
	  LED_set(LED6, !KBD_get_button_state(BTN_ESC));
	  LED_set(LED7, !KBD_get_button_state(BTN_JOY));

	 HAL_ADC_Start(&hadc4);
	 HAL_ADC_PollForConversion(&hadc4,10);// Waiting for ADC conversion
	 joystick_raw.x=HAL_ADC_GetValue(&hadc4);

	 HAL_ADC_Start(&hadc4);
	 HAL_ADC_PollForConversion(&hadc4,10);// Waiting for ADC conversion
	 joystick_raw.y=HAL_ADC_GetValue(&hadc4);
	 HAL_ADC_Stop(&hadc4);

	 joystick_get(&joystick_raw, &joystick_out, &joystick);
	 UG_DrawCircle(joystick_out.x+250, joystick_out.y+50,5, C_YELLOW);


	if(XPT2046_TouchPressed())
	{
		uint16_t x = 0, y = 0;

		if(XPT2046_TouchGetCoordinates(&x, &y, 0))
		{
			touch_x = x;
			touch_y = y;
			UG_FillCircle(x, y,2, C_GREEN);
			UG_FillCircle(250, 50, 49, C_BLACK);
		}
	}

	 sprintf(MSG, "Joystick X:%05d, Y:%05d, Touch: X:%05d, Y:%05d    \r",joystick_out.x,joystick_out.y, touch_x, touch_y);
	 SCI_send_string(MSG);
	 CDC_Transmit_FS(MSG, strlen(MSG));
	 UG_DrawCircle(250, 50, 50, C_RED);

	 HAL_Delay(20);
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV6;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
