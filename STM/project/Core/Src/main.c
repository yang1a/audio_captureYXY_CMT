#include "main.h"
#include "gpio.h"
#include "USART.h"
#include "clock_mpu.h"
#include "Servo.h"
#include "adc_dma.h"
#include "app_doa.h"
#include "config.h"
#include <stdio.h>

/* 调试打印计数器 */
static uint32_t print_counter = 0;
#define PRINT_INTERVAL 10 /* 每 10 帧打印一次 */

int main(void)
{
  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();

  /* 初始化 DOA 系统 */
  if (app_doa_init() != HAL_OK)
  {
    printf("DOA init failed!\r\n");
    Error_Handler();
  }

  printf("DOA system started. FS=%dHz, FRAME=%d, FFT=%d\r\n",
         FS_HZ, FRAME_N, FFT_L);

  while (1)
  {
    /* 检查是否有新帧可处理 */
    if (app_doa_frame_ready())
    {
      /* 处理帧数据（GCC-PHAT + 可信度判决） */
      app_doa_process_frame();

      /* 更新舵机 */
      app_doa_servo_update();

      /* 定期打印调试信息 */
      print_counter++;
      if (print_counter >= PRINT_INTERVAL)
      {
        print_counter = 0;
        app_doa_debug_print();
        HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_7);
      }
    }
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
