/**
 * @file servo_ctrl.h
 * @brief 舵机控制模块
 *
 * 实现角度到 PWM 脉宽的映射，控制舵机指向声源方向
 */
#ifndef __SERVO_CTRL_H__
#define __SERVO_CTRL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "config.h"

    /**
     * @brief 初始化舵机控制模块
     * @note 将舵机移动到中心位置
     */
    void servo_ctrl_init(void);

    /**
     * @brief 设置舵机目标角度
     * @param theta_deg 声源角度 (-90 ~ +90 度)
     * @note 内部会转换为舵机角度 (0 ~ 180 度)
     */
    void servo_ctrl_set_angle(float theta_deg);

    /**
     * @brief 更新舵机位置
     * @note 在主循环中调用
     */
    void servo_ctrl_update(void);

    /**
     * @brief 获取当前舵机角度
     * @retval 当前舵机角度 (0 ~ 180 度)
     */
    float servo_ctrl_get_angle(void);

#ifdef __cplusplus
}
#endif

#endif /* __SERVO_CTRL_H__ */
