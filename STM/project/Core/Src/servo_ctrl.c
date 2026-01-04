/**
 * @file servo_ctrl.c
 * @brief 舵机控制模块实现
 */
#include "servo_ctrl.h"
#include "Servo.h"

/* 当前目标舵机角度 (0-180度) */
static float current_servo_angle = 90.0f;

/**
 * @brief 初始化舵机控制
 */
void servo_ctrl_init(void)
{
    current_servo_angle = 90.0f;
    Servo_SetAngle(90);
}

/**
 * @brief 设置舵机目标角度
 * @param theta_deg 声源角度 (-90 ~ +90 度)
 */
void servo_ctrl_set_angle(float theta_deg)
{
    /* 声源角度 -> 舵机角度映射 */
    /* theta = 0 时舵机在 90 度（中间） */
    /* theta = -90 时舵机在 0 度（左） */
    /* theta = +90 时舵机在 180 度（右） */
    float servo_deg = 90.0f + theta_deg;

    /* Clamp 到 0-180 度 */
    if (servo_deg < 0.0f)
        servo_deg = 0.0f;
    if (servo_deg > 180.0f)
        servo_deg = 180.0f;

    current_servo_angle = servo_deg;
}

/**
 * @brief 更新舵机位置
 */
void servo_ctrl_update(void)
{
    uint8_t angle = (uint8_t)(current_servo_angle + 0.5f);
    Servo_SetAngle(angle);
}

/**
 * @brief 获取当前舵机角度
 */
float servo_ctrl_get_angle(void)
{
    return current_servo_angle;
}
