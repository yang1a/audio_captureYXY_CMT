/**
 ******************************************************************************
 * @file           : audio_capture.h
 * @brief          : 双通道 ADC+DMA 音频采集头文件
 ******************************************************************************
 */

#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "config.h"

/* 双缓冲大小：2 个通道 * 帧长度 */
#define DUAL_BUFFER_SIZE (2 * FRAME_N)

    /**
     * @brief 音频采集状态结构体
     */
    typedef struct
    {
        uint16_t buffer[2][DUAL_BUFFER_SIZE]; // 乒乓缓冲区
        volatile uint8_t frame_ready;         // 帧就绪标志 (0=无, 1=buffer0, 2=buffer1)
        volatile uint8_t current_buffer;      // 当前 DMA 正在填充的缓冲区
    } AudioCapture_t;

    /**
     * @brief 初始化音频采集模块
     * @param hadc: ADC 句柄指针
     * @return HAL 状态码
     */
    HAL_StatusTypeDef AudioCapture_Init(ADC_HandleTypeDef *hadc);

    /**
     * @brief 启动 DMA 音频采集
     * @return HAL 状态码
     */
    HAL_StatusTypeDef AudioCapture_Start(void);

    /**
     * @brief 停止音频采集
     * @return HAL 状态码
     */
    HAL_StatusTypeDef AudioCapture_Stop(void);

    /**
     * @brief 检查是否有帧可供处理
     * @return 就绪返回 1，否则返回 0
     */
    uint8_t AudioCapture_IsFrameReady(void);

    /**
     * @brief 获取已就绪帧的数据指针
     * @param ch1: 输出通道 1 数据指针
     * @param ch2: 输出通道 2 数据指针
     * @return 成功返回 1，无就绪帧返回 0
     */
    uint8_t AudioCapture_GetFrame(uint16_t **ch1, uint16_t **ch2);

    /**
     * @brief 标记当前帧已被处理
     */
    void AudioCapture_FrameConsumed(void);

    /**
     * @brief DMA 半传输回调（在 HAL_ADC_ConvHalfCpltCallback 中调用）
     */
    void AudioCapture_HalfCpltCallback(void);

    /**
     * @brief DMA 全传输回调（在 HAL_ADC_ConvCpltCallback 中调用）
     */
    void AudioCapture_CpltCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_CAPTURE_H */
