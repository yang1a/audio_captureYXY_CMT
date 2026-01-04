/**
 * @file app_doa.c
 * @brief DOA 主业务逻辑实现
 */
#include "app_doa.h"
#include "audio_frame.h"
#include "gcc_phat.h"
#include "servo_ctrl.h"
#include "config.h"
#include <stdio.h>

/* 音频帧缓冲区 */
static float frame_x1[FRAME_N];
static float frame_x2[FRAME_N];

/* GCC-PHAT 结果 */
static gcc_phat_result_t gcc_result;

/* 平滑后的角度 */
static float theta_smooth = 0.0f;

/* 调试信息 */
static float debug_lag_sub = 0.0f;
static float debug_dt = 0.0f;
static float debug_theta = 0.0f;
static float debug_peak = 0.0f;
static float debug_ratio = 0.0f;

/**
 * @brief 初始化 DOA 系统
 */
HAL_StatusTypeDef app_doa_init(void)
{
    /* 初始化各模块 */
    gcc_phat_init();
    servo_ctrl_init();

    /* 初始化平滑角度 */
    theta_smooth = 0.0f;

    /* 初始化音频帧采集 */
    return audio_frame_init();
}

/**
 * @brief 检查是否有新帧
 */
bool app_doa_frame_ready(void)
{
    return audio_frame_available();
}

/**
 * @brief 处理一帧音频数据
 */
void app_doa_process_frame(void)
{
    /* 获取帧数据 */
    audio_frame_get(frame_x1, frame_x2);

    /* 执行 GCC-PHAT */
    gcc_phat_process(frame_x1, frame_x2, &gcc_result);

    /* 保存调试信息 */
    debug_lag_sub = gcc_result.lag_sub;
    debug_dt = gcc_result.dt;
    debug_theta = gcc_result.theta_deg;
    debug_peak = gcc_result.peak;
    debug_ratio = gcc_result.ratio;

    /* 如果结果有效，更新平滑角度 */
    if (gcc_result.valid)
    {
        /* 一阶指数低通滤波 */
        theta_smooth = ALPHA_SMOOTH * gcc_result.theta_deg +
                       (1.0f - ALPHA_SMOOTH) * theta_smooth;

        /* 更新舵机目标角度 */
        servo_ctrl_set_angle(theta_smooth);
    }
    /* 如果结果无效，保持原角度不变 */
}

/**
 * @brief 更新舵机位置
 */
void app_doa_servo_update(void)
{
    servo_ctrl_update();
}

/**
 * @brief 打印调试信息
 */
void app_doa_debug_print(void)
{
    printf("lag:%.2f dt:%.6f theta:%.1f peak:%.3f ratio:%.2f smooth:%.1f %s\r\n",
           debug_lag_sub,
           debug_dt,
           debug_theta,
           debug_peak,
           debug_ratio,
           theta_smooth,
           gcc_result.valid ? "OK" : "SKIP");
}
