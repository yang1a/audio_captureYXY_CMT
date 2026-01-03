/**
 ******************************************************************************
 * @file           : servo.h
 * @brief          : 舵机 PWM 控制头文件
 ******************************************************************************
 */

#ifndef SERVO_H
#define SERVO_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_tim.h"
#include "config.h"

    /**
     * @brief 初始化舵机控制
     * @param htim: 用于 PWM 产生的定时器句柄
     * @param channel: PWM 输出的定时器通道
     * @return 成功返回 0，失败返回 -1
     */
    int Servo_Init(TIM_HandleTypeDef *htim, uint32_t channel);

    /**
     * @brief 设置舵机角度
     * @param angle_deg: 目标角度（0~180 度）
     * @return 成功返回 0，失败返回 -1
     */
    int Servo_SetAngle(float angle_deg);

    /**
     * @brief 启动舵机 PWM 输出
     * @return 成功返回 0，失败返回 -1
     */
    int Servo_Start(void);

    /**
     * @brief 停止舵机 PWM 输出
     * @return 成功返回 0，失败返回 -1
     */
    int Servo_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /* SERVO_H */
