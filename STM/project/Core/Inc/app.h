/**
 ******************************************************************************
 * @file           : app.h
 * @brief          : 主应用逻辑头文件
 ******************************************************************************
 */

#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_tim.h"

    /**
     * @brief 初始化应用
     * @param hadc: 用于音频采集的 ADC 句柄
     * @param htim: 用于舵机控制的定时器句柄
     * @param servo_channel: 舵机 PWM 的定时器通道
     * @return 成功返回 0，失败返回 -1
     */
    int App_Init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, uint32_t servo_channel);

    /**
     * @brief 启动应用（启动音频采集和舵机）
     * @return 成功返回 0，失败返回 -1
     */
    int App_Start(void);

    /**
     * @brief 主循环函数 - 在主循环中反复调用
     * 此函数处理音频帧并更新舵机
     */
    void App_Loop(void);

    /**
     * @brief 停止应用
     * @return 成功返回 0，失败返回 -1
     */
    int App_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H */
