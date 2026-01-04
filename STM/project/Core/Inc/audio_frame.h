/**
 * @file audio_frame.h
 * @brief DMA 音频帧管理模块
 *
 * 将 DMA 连续采集的数据组织为算法处理所需的帧
 */
#ifndef __AUDIO_FRAME_H__
#define __AUDIO_FRAME_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "config.h"
#include <stdbool.h>

    /**
     * @brief 初始化音频帧模块并启动 DMA 采集
     * @retval HAL_StatusTypeDef
     */
    HAL_StatusTypeDef audio_frame_init(void);

    /**
     * @brief 检查是否有新帧可用
     * @retval true: 有帧可用; false: 无帧
     */
    bool audio_frame_available(void);

    /**
     * @brief 获取当前帧数据（转换为浮点）
     * @param x1 麦克风1数据输出缓冲区 (长度 FRAME_N)
     * @param x2 麦克风2数据输出缓冲区 (长度 FRAME_N)
     */
    void audio_frame_get(float *x1, float *x2);

    /**
     * @brief DMA 半传输完成回调（由中断调用）
     */
    void audio_frame_half_cplt_callback(void);

    /**
     * @brief DMA 全传输完成回调（由中断调用）
     */
    void audio_frame_cplt_callback(void);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_FRAME_H__ */
