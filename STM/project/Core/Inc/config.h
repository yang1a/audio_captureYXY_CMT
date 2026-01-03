/**
 ******************************************************************************
 * @file           : config.h
 * @brief          : 双麦克风 DOA 系统的配置参数
 ******************************************************************************
 */

#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <math.h>

/* ==================== 音频采集配置 ==================== */
#define FS_HZ 48000  // 采样频率 (Hz)
#define FRAME_N 1024 // 处理帧长 (采样点数)
#define FFT_L 2048   // FFT 长度（2 的幂，且 >= 2*FRAME_N）

/* ==================== 麦克风配置 ==================== */
#define MIC_DIST_M 0.12f   // 麦克风间距 (米)
#define SPEED_SOUND 343.0f // 声速 (m/s)

/* ==================== GCC-PHAT 参数 ==================== */
#define EPS_PHAT 1e-12f // 避免除零的极小值
#define PEAK_MIN 0.15f  // 置信度峰值下限
#define RATIO_MIN 1.5f  // 置信度峰值比（峰1/峰2）下限

/* ==================== 角度平滑 ==================== */
#define ALPHA_SMOOTH 0.2f // 指数平滑系数 (0~1)

/* ==================== 舵机配置 ==================== */
#define SERVO_MIN_MS 0.5f         // 舵机最小脉宽 (ms)
#define SERVO_MAX_MS 2.5f         // 舵机最大脉宽 (ms)
#define SERVO_CENTER_DEG 90.0f    // 舵机中心角度 (度)
#define SERVO_PWM_FREQ 50         // 舵机 PWM 频率 (Hz)
#define SERVO_PWM_PERIOD_MS 20.0f // 舵机 PWM 周期 (ms)

/* ==================== 派生参数 ==================== */
#define DT_MAX (MIC_DIST_M / SPEED_SOUND)  // 最大时间差 (s)
#define N_MAX ((uint32_t)(DT_MAX * FS_HZ)) // 最大采样点差

/* ==================== 数学常量 ==================== */
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0f)
#define RAD_TO_DEG(rad) ((rad) * 180.0f / M_PI)

/* ==================== 调试配置 ==================== */
#define DEBUG_PRINT_ENABLED 1 // 通过 UART 启用调试打印

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
