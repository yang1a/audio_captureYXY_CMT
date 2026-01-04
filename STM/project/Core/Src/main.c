#include "main.h"
#include "gpio.h"
#include "USART.h"
#include "clock_mpu.h"
#include "Servo.h"
#include "adc_dma.h"
#include <stdio.h>

int main(void)
{
  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();

  while (1)
  {
    printf("System running...\r\n");
    HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_7);
    HAL_Delay(50);
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {

  }
}
