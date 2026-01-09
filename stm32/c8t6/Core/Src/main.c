/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>

#include "cc/neolux/auto/motor.h"
#include "cc/neolux/auto/infrared.h"
#include "cc/neolux/auto/parser.h"
#include "stdio.h"

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
uint8_t rx1_data;
uint8_t rx1_rv_cnt=0;
#define RX_BUF_SIZE 64
uint8_t rx1_buf[RX_BUF_SIZE];
int l_velocity=0, r_velocity=0;

Motor_t motor_l ={}, motor_r ={};
#define IR_CNT 6
InfraredArray_t array;
Infrared_t irir[IR_CNT]; // 0-5: left-right

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart == &huart1) {
    if (rx1_rv_cnt >= RX_BUF_SIZE - 1) {
      rx1_rv_cnt = 0;
      memset(rx1_buf, 0, RX_BUF_SIZE);
    }

    rx1_buf[rx1_rv_cnt++] = rx1_data;

    if (rx1_data == '\n' || rx1_data == '\r') {
      if (rx1_rv_cnt > 1) {
        rx1_buf[rx1_rv_cnt] = '\0';
        parse_cmd(rx1_buf, &l_velocity, &r_velocity);
      }

      rx1_rv_cnt = 0;
      memset(rx1_buf, 0, RX_BUF_SIZE);
    }

    HAL_UART_Receive_IT(huart, &rx1_data, 1);
  }
}


void app_motors_init(void) {
  motor_l.timer = &htim2;
  motor_l.channel = TIM_CHANNEL_3;
  motor_l.dir_port = MotorDir_R_GPIO_Port;
  motor_l.dir_pin = MotorDir_R_Pin;
  motor_r.timer = &htim3;
  motor_r.channel = TIM_CHANNEL_1;
  motor_r.dir_port = MotorDir_L_GPIO_Port;
  motor_r.dir_pin = MotorDir_L_Pin;

  motor_on(&motor_l);
  motor_on(&motor_r);
  motor_set_speed_dir(&motor_l, MOTOR_DIR_FORWARD, 0);
  motor_set_speed_dir(&motor_r, MOTOR_DIR_FORWARD, 0);
}

void app_irs_init(void) {
  infrared_init(&irir[0], IR0_GPIO_Port, IR0_Pin);
  infrared_init(&irir[1], IR3_GPIO_Port, IR3_Pin);
  infrared_init(&irir[2], IR2_GPIO_Port, IR2_Pin);
  infrared_init(&irir[3], IR5_GPIO_Port, IR5_Pin);
  infrared_init(&irir[4], IR1_GPIO_Port, IR1_Pin);
  infrared_init(&irir[5], IR4_GPIO_Port, IR4_Pin);
  infrared_array_init(&array, IR_CNT, irir);
}

void printf_binary(uint8_t i) {
  for (int8_t j = 7; j >= 0; j--) {
    putchar( (i & (1 << j)) ? '1' : '0' );
  }
  putchar('\r');
  putchar('\n');
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
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  app_irs_init();
  app_motors_init();
  HAL_UART_Receive_IT(&huart1, &rx1_data, 1); // Enable UART receive interrupt
  uint8_t ir_value=0;

  while (1)
  {
    motor_set_speed(&motor_l, l_velocity);
    motor_set_speed(&motor_r, r_velocity);

    HAL_Delay(3); // 1627
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
