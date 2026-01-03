/**
 ******************************************************************************
 * @file           : retarget.h
 * @brief          : printf 重定向到 UART 的头文件
 ******************************************************************************
 * @note           : 若需使用 printf 调试输出，请将其加入工程
 */

#ifndef RETARGET_H
#define RETARGET_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_uart.h"

    /**
     * @brief 初始化 printf 重定向
     * @param huart: 用于 printf 输出的 UART 句柄
     */
    void RetargetInit(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif /* RETARGET_H */
