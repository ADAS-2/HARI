/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "UartRingbuffer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
char data;
char buffer[50];
char buff1[90];
char buff2[90];
char clockwise[20];
char time_buff[20];
uint8_t count_sec = 0;
uint8_t flag=0;
uint8_t time=0;
uint8_t stop=0;

uint8_t tim_val = 0;
//int motor_flag=1;

int time_flag=0;
uint8_t sw1_flag=0;
uint8_t sw2_flag=0;

uint8_t iot_flag=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
int data_from_iot(void);
void motor_drive_clockwise(void);
void motor_drive_anticlockwise(void);
int iot_data_validation(char data[]);
void switch1_release(void);
void laser_detect(void);
void timer_start(void);
void Buzzer(void);
void motor_drive_stop(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int data_from_iot(void) {
	/*
	 * First checking IOT data is available if its available, read from UART 1 and store data variable
	 * and Validating data is 1 return 1 to main function
	 */
	if (IsDataAvailable()) {
		data = Uart_read();

		if (data == '1') {
			Uart_flush();
			HAL_UART_Transmit(&huart2, "SUcces\r\n", strlen("Succes\r\n"), 100);
			return 1;
		}
		/*
		 * If data is not match 1 return 0
		 */else {
			HAL_UART_Transmit(&huart2, "failure\r\n", strlen("failure\r\n"),
					100);

			return 0;
		}

	}

}

void motor_drive_clockwise(void) {

	/*
	 * I start motor clockwise to reach Default state condition
	 * Motor Input 1- SET
	 * Motor Input 2- Reset
	 */
	HAL_UART_Transmit(&huart2, "clock enter\r\n", strlen("clock enter\r\n"),
			100);
	HAL_GPIO_WritePin(GPIOC, MOTOR2_IN1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, MOTOR2_IN2_Pin, GPIO_PIN_RESET);

}
void motor_drive_anticlockwise(void) {

	/*
		 * I start motor counter clockwise to reach 90 degree state condition
		 * Motor Input 1- Reset
		 * Motor Input 2- Set
		 */
	HAL_UART_Transmit(&huart2, "motor enter\r\n", strlen("motor enter\r\n"),
			100);
	HAL_GPIO_WritePin(GPIOC, MOTOR2_IN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, MOTOR2_IN2_Pin, GPIO_PIN_SET);

}
void motor_drive_stop(void) {
	/*
		 * Once Motor Reach 90 degree turn off motor and Motor reach default state Turn off motor
		 * Motor Input 1- Reset
		 * Motor Input 2- Reset
		 */
	HAL_UART_Transmit(&huart2, "motor stop\r\n", strlen("motor stop\r\n"), 100);
	HAL_GPIO_WritePin(GPIOC, MOTOR2_IN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, MOTOR2_IN2_Pin, GPIO_PIN_RESET);

}
void timer_start(void) {
 /*
  *  Once Motor Reach 90 degree angle position To turn on the Timer 10 Second
  *  and Turn On the Buzzer
  */
	HAL_UART_Transmit(&huart2, "time enter\r\n", strlen("time enter\r\n"), 100);

	Buzzer();
	HAL_TIM_Base_Start_IT(&htim6);

	time_flag = 1; // Set Timer flag to indicate the timer on call laser detect function in superloop

}
void Buzzer(void) {
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 1);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 0);
}

void laser_detect(void) {
/*
 *  It's Keep on detecting laser if it is detected TO turn on the clock wise motor then turn off the motor
 *  and Buzzer On
 */
	if (HAL_GPIO_ReadPin(LASER_GPIO_Port, LASER_Pin) == 1) {
		HAL_UART_Transmit(&huart2, "laser enter\r\n", strlen("laser enter\r\n"),
				100);

		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 0);
		Uart_write(2);
		HAL_Delay(2000);
		motor_drive_clockwise();
		time_flag = 0; // Reset timer flag for another time will execute this function
        sw1_flag=1;// Increment sw flag to turn on switch sw2
		count_sec=0;
		HAL_TIM_Base_Stop_IT(&htim6);



	}

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  Ringbuf_init ();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	if (iot_flag == 0) {       // I set Iot flag if it is 0 then entering to Data from iot
			if (data_from_iot())// Data from iot function return 1 then set the iot flag
				iot_flag = 1;
		}
		if (iot_flag) { // Iot flag is 1 entering to this function

			/*
			 * Bottom Switch- Initially switch will be High state, sw flag 0 and then entering to this condition
			 * To turn on the anticlockwise motor and increment the sw flag to execute sw2 condition
			 */
			if ( sw1_flag == 0)

			{
				sw1_flag=2;
				HAL_UART_Transmit(&huart2, "anticlockwise motor start\r\n",
						strlen("anticlockwise motor start\r\n"), 100);
				motor_drive_anticlockwise();
				Uart_write(0);

			}
           /*
            *  Now motor is running to reach 90 degree, Once Switch 2 is High and sw flag is 1 to turn off the motor
            *  And increment the sw flag  timer interrupt function. whether timer is working around 10 sec
            *  To turn on the clock wise motor
            */
			if (HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == 1 && sw2_flag == 0)

			{
				sw2_flag=1;
				HAL_UART_Transmit(&huart2, "motor stop\r\n",
						strlen("motor stop\r\n"), 100);
				motor_drive_stop();
				HAL_Delay(1000);
				timer_start();


			}
            /*
             * The clockwise motor is started from timer completed and laser detect to turn off
             * 90 degree switch, and checking sw2 reset and sw flag 3 to increment the sw flag
             * to execute Bottom switch
             */
			if (HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == 0 && sw2_flag == 1) {
				sw2_flag=0;

				HAL_UART_Transmit(&huart2, "clockwise motor start\r\n",
						strlen("clockwise motor start\r\n"), 100);


			}
			/*
			 * Once Bottom switch is high to stop the motor
			 */
			if (HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == 1 && sw1_flag == 1)

			{
				sw1_flag = 0;
				HAL_UART_Transmit(&huart2, "default position\r\n",
						strlen("default position\r\n"), 100);
				motor_drive_stop();
				iot_flag = 0;

			}
			/*
			 *  if time flag is set in timer start function to execute laser function
			 */
			if (time_flag == 1) {
				laser_detect();
			}
			/*
			 * Timer flag is reset in timer interrupt to disable the Rf tx pin
			 */
			else
			{
				HAL_Delay(500);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 0);
			}

		}







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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 15999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, MOTOR_EN1_Pin|MOTOR2_IN2_Pin|MOTOR2_IN1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MOTOR_EN1_Pin MOTOR2_IN2_Pin MOTOR2_IN1_Pin */
  GPIO_InitStruct.Pin = MOTOR_EN1_Pin|MOTOR2_IN2_Pin|MOTOR2_IN1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 LED_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SW1_Pin */
  GPIO_InitStruct.Pin = SW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(SW1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LASER_Pin */
  GPIO_InitStruct.Pin = LASER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LASER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUZZER_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BUZZER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SW2_Pin */
  GPIO_InitStruct.Pin = SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(SW2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	count_sec++;
	HAL_UART_Transmit(&huart2, buffer, sprintf(buffer,"count:%d\r\n",count_sec), 100);

		if(count_sec==10)
		{
			motor_drive_clockwise();
            sw1_flag=1;
			HAL_UART_Transmit(&huart2, "Time over\r\n",strlen("Time over\r\n"), 100);
			Uart_write(3);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, 1);
			time_flag=0;
			count_sec=0;
			HAL_TIM_Base_Stop_IT(&htim6);


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
