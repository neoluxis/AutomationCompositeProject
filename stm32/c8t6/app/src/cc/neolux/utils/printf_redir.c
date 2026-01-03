//
// Created by neolux on 2025/12/25.
//

#include "cc/neolux/utils/printf_redir.h"
#include "stdio.h"
#include "main.h"
#include "usart.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
/* USER CODE END PFP */

/* USER CODE BEGIN 4 */
/*retargets the C library printf function to the USART*/
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1,(uint8_t*)&ch, 1, 0xFFFF);
    return ch;
}
/* USER CODE END 4 */