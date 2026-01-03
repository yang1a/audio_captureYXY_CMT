/**
 ******************************************************************************
 * @file           : retarget.c
 * @brief          : printf 重定向到 UART 的实现
 ******************************************************************************
 * @note           : 使 printf 可以通过 UART 输出
 */

#include "retarget.h"
#include <stdio.h>

static UART_HandleTypeDef *gHuart;

/**
 * @brief 初始化 printf 重定向
 */
void RetargetInit(UART_HandleTypeDef *huart)
{
    gHuart = huart;

    /* Disable I/O buffering for stdout */
    setvbuf(stdout, NULL, _IONBF, 0);
}

/**
 * @brief 为 printf 重定向 _write 函数
 */
int _write(int file, char *ptr, int len)
{
    (void)file; /* Unused parameter */

    if (gHuart != NULL)
    {
        HAL_UART_Transmit(gHuart, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    }

    return len;
}
