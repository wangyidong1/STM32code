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
#include "tim.h"
#include "usart.h"
#include "gpio.h"
 
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "modbus.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t RES;
/***************** 发送一个字符  **********************/
//使用单字节数据发送前要使能发送引脚，发送后要使能接收引脚。
void Modbus_Send_Byte(  uint8_t ch )
{ 
	/* 发送一个字节数据到USART2 */
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xff);	
}

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
//485发送接收控制引脚
//接收使能
//#define RS485_RX_ENABLE  HAL_GPIO_WritePin(RS485_control_GPIO_Port,RS485_control_Pin,GPIO_PIN_RESET)

//发送使能
//#define RS485_TX_ENABLE HAL_GPIO_WritePin(RS485_control_GPIO_Port,RS485_control_Pin,GPIO_PIN_SET)

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
//  int i;
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
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	
	
	Modbus_Init();//本机作为从机使用时初始化

	
	HAL_TIM_Base_Start_IT(&htim1);
	printf("RS485_TEST_01\r\n");
	
	
	RS485_RX_ENABLE;
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&RES, 1);//调用接收中断函数
	
	

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
				/*
				共包含3部分测试（每一部分需单独测试）--功能实现了，但是没有加入按键结合使用
				1-主机读取从机数据测试（已经打开注释）
				2-主机向从机的一个寄存器中写入数据
				3-本设备作为从机使用，作为从机时地址为0x02
				测试完一个注释掉，再打开另一个测试
				*/ 
				if(modbus.Host_time_flag)//每1s发送一次数据
				{
				
					//01-读取从机数据测试
					//参数1：查看第i个从机数据
					Host_Read03_slave(0x01,0x0000,0x0002);//参数1从机地址，参数2起始地址，参数3寄存器个数
					if(modbus.Host_send_flag)
					{
						modbus.Host_Sendtime=0;//发送完毕后计数清零（距离上次的时间）
						modbus.Host_time_flag=0;//发送数据标志位清零
						modbus.Host_send_flag=0;//清空发送结束数据标志位

						HOST_ModbusRX();//接收数据进行处理
					}
					
			
					
					
//					

//					//02-写入数据测试					
//					Host_write06_slave(0x01,0x06,0x0001,0x0045);
//					if(modbus.Host_send_flag)
//					{
//						modbus.Host_Sendtime=0;//发送完毕后计数清零（距离上次的时间）
//						modbus.Host_time_flag=0;//发送数据标志位清零
//						modbus.Host_send_flag=0;//清空发送结束数据标志位

//						Host_Func6();//从机返回数据处理
//						printf("111");
//					}
//					
//					
//					
					
					
					
//					//3-作为从机使用
//					Modbus_Event();//本机作为从机使用时
//					//printf("01\r\n");
//					
//					
		}
		//HAL_Delay(500);


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the CPU, AHB and APB busses clocks 
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
  /** Initializes the CPU, AHB and APB busses clocks 
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



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == htim1.Instance) 
	{
		
		if(modbus.timrun != 0)//运行时间！=0表明
		 {
		  modbus.timout++;
		  if(modbus.timout >=8)
		  {
		   modbus.timrun = 0;
			 modbus.reflag = 1;//接收数据完毕
		  }
		}
		modbus.Host_Sendtime++;//发送完上一帧后的时间计数
		 if(modbus.Host_Sendtime>1000)//距离发送上一帧数据1s了
			{
				//1s时间到
				modbus.Host_time_flag=1;//发送数据标志位置1
				
			}
	}
}



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
