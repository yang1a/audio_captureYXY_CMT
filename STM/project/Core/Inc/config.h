/**
 * @file config.h
 * @brief 全局算法与物理参数配置
 *
 * 基于 STM32H7 的双麦克风 GCC-PHAT 声源方向估计系统
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* ========== 采样与帧参数 ========== */
#define FS_HZ 48000U  /* 采样率 48 kHz */
#define FRAME_N 1024U /* 帧长度（采样点数） */
#define FFT_L 2048U   /* FFT 长度（零填充后） */

/* ========== 物理参数 ========== */
#define MIC_DIST_M 0.12f   /* 麦克风间距 (m) */
#define SOUND_SPEED 343.0f /* 声速 (m/s) */

/* ========== 算法参数 ========== */
#define ALPHA_SMOOTH 0.2f /* 一阶低通滤波系数 */
#define EPS_PHAT 1e-12f   /* PHAT 加权防除零 */

/* ========== 可信度判决阈值 ========== */
#define PEAK_MIN 0.15f /* 峰值高度阈值 */
#define RATIO_MIN 1.5f /* 主峰/次峰比阈值 */

/* ========== 舵机参数 ========== */
#define SERVO_MIN_US 500U     /* 最小脉宽 (us) */
#define SERVO_MAX_US 2500U    /* 最大脉宽 (us) */
#define SERVO_CENTER_US 1500U /* 中心脉宽 (us) */

/* ========== 计算得出的参数 ========== */
/* 最大时间延迟 = d / c */
#define MAX_DELAY_S (MIC_DIST_M / SOUND_SPEED)

/* 最大延迟采样点数 = floor(d/c * Fs) ≈ 17 */
#define MAX_LAG_SAMPLES ((uint32_t)((MIC_DIST_M / SOUND_SPEED) * FS_HZ) + 1U)

/* DMA 缓冲区大小（双通道交错，FRAME_N 个采样点） */
#define DMA_BUFFER_SIZE (FRAME_N * 2U)

#ifdef __cplusplus
}
#endif

#endif /* __CONFIG_H__ */
