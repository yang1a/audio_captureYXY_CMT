/**
 ******************************************************************************
 * @file           : app.c
 * @brief          : 主应用逻辑实现
 ******************************************************************************
 */

#include "app.h"
#include "config.h"
#include "audio_capture.h"
#include "gcc_phat.h"
#include "servo.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

/* 私有变量 */
static uint8_t app_initialized = 0;
static float theta_smooth = 0.0f; // 平滑角度

/* 工作缓冲区 */
static float ch1_float[FRAME_N];
static float ch2_float[FRAME_N];

/**
 * @brief 初始化应用
 */
int App_Init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim, uint32_t servo_channel)
{
    if (hadc == NULL || htim == NULL)
        return -1;

    // 初始化音频采集
    if (AudioCapture_Init(hadc) != HAL_OK)
    {
        printf("Error: AudioCapture_Init failed\r\n");
        return -1;
    }

    // 初始化 GCC-PHAT
    if (GccPhat_Init() != 0)
    {
        printf("Error: GccPhat_Init failed\r\n");
        return -1;
    }

    // 初始化舵机
    if (Servo_Init(htim, servo_channel) != 0)
    {
        printf("Error: Servo_Init failed\r\n");
        return -1;
    }

    // 将平滑角度初始化为中心
    theta_smooth = 0.0f;

    app_initialized = 1;
    printf("App initialized successfully\r\n");

    return 0;
}

/**
 * @brief 启动应用
 */
int App_Start(void)
{
    if (!app_initialized)
        return -1;

    // 启动音频采集
    if (AudioCapture_Start() != HAL_OK)
    {
        printf("Error: AudioCapture_Start failed\r\n");
        return -1;
    }

    // 启动舵机
    if (Servo_Start() != 0)
    {
        printf("Error: Servo_Start failed\r\n");
        return -1;
    }

    printf("App started\r\n");
    return 0;
}

/**
 * @brief 停止应用
 */
int App_Stop(void)
{
    if (!app_initialized)
        return -1;

    AudioCapture_Stop();
    Servo_Stop();

    printf("App stopped\r\n");
    return 0;
}

/**
 * @brief 主应用循环
 */
void App_Loop(void)
{
    if (!app_initialized)
        return;

    // 检查是否有新帧就绪
    if (!AudioCapture_IsFrameReady())
        return;

    // 获取帧数据
    uint16_t *ch1_raw = NULL;
    uint16_t *ch2_raw = NULL;

    if (!AudioCapture_GetFrame(&ch1_raw, &ch2_raw))
    {
        return;
    }

    // 转换为浮点数
    adc_to_float(ch1_raw, ch1_float, FRAME_N);
    adc_to_float(ch2_raw, ch2_float, FRAME_N);

    // 使用 GCC-PHAT 处理
    GccPhat_Result_t result;
    if (GccPhat_Process(ch1_float, ch2_float, &result) != 0)
    {
        AudioCapture_FrameConsumed();
        return;
    }

    // 打印调试信息
#if DEBUG_PRINT_ENABLED
    printf("Lag: %.2f, dt: %.6f s, theta: %.1f deg, peak: %.3f, ratio: %.2f, valid: %d\r\n",
           result.lag_sub, result.dt, result.theta_deg,
           result.peak_value, result.peak_ratio, result.is_valid);
#endif

    // 仅在结果有效时更新舵机（通过置信度检查）
    if (result.is_valid)
    {
        // 应用指数平滑
        theta_smooth = ALPHA_SMOOTH * result.theta_deg + (1.0f - ALPHA_SMOOTH) * theta_smooth;

        // 转换为舵机角度：中心 (90°) + theta
        float servo_angle = SERVO_CENTER_DEG + theta_smooth;

        // 更新舵机位置
        Servo_SetAngle(servo_angle);

#if DEBUG_PRINT_ENABLED
        printf("Servo updated: %.1f deg (smoothed theta: %.1f)\r\n", servo_angle, theta_smooth);
#endif
    }
    else
    {
#if DEBUG_PRINT_ENABLED
        printf("Low confidence - servo not updated\r\n");
#endif
    }

    // 标记帧已处理
    AudioCapture_FrameConsumed();
}
