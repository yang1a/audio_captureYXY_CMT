/**
 ******************************************************************************
 * @file           : audio_capture.c
 * @brief          : 双通道 ADC+DMA 音频采集实现
 ******************************************************************************
 */

#include "audio_capture.h"
#include <string.h>

/* 私有变量 */
static AudioCapture_t g_audio;
static ADC_HandleTypeDef *g_hadc = NULL;

/**
 * @brief 初始化音频采集模块
 */
HAL_StatusTypeDef AudioCapture_Init(ADC_HandleTypeDef *hadc)
{
    if (hadc == NULL)
        return HAL_ERROR;

    g_hadc = hadc;

    // 清空缓冲区
    memset(&g_audio, 0, sizeof(AudioCapture_t));

    return HAL_OK;
}

/**
 * @brief 启动 DMA 音频采集
 */
HAL_StatusTypeDef AudioCapture_Start(void)
{
    if (g_hadc == NULL)
        return HAL_ERROR;

    g_audio.frame_ready = 0;
    g_audio.current_buffer = 0;

    // 以循环模式启动 ADC DMA
    // 注意：假设 ADC 已配置为 2 通道扫描模式
    // 将优先填充缓冲区 0
    return HAL_ADC_Start_DMA(g_hadc,
                             (uint32_t *)g_audio.buffer[0],
                             DUAL_BUFFER_SIZE);
}

/**
 * @brief 停止音频采集
 */
HAL_StatusTypeDef AudioCapture_Stop(void)
{
    if (g_hadc == NULL)
        return HAL_ERROR;

    return HAL_ADC_Stop_DMA(g_hadc);
}

/**
 * @brief 检查是否有可处理的帧
 */
uint8_t AudioCapture_IsFrameReady(void)
{
    return (g_audio.frame_ready != 0);
}

/**
 * @brief 获取已就绪帧的数据指针
 */
uint8_t AudioCapture_GetFrame(uint16_t **ch1, uint16_t **ch2)
{
    if (g_audio.frame_ready == 0)
        return 0;

    // 判断哪个缓冲区已就绪
    uint8_t ready_buffer = (g_audio.frame_ready == 1) ? 0 : 1;

    // 分配临时数组保存分离后的数据
    static uint16_t ch1_data[FRAME_N];
    static uint16_t ch2_data[FRAME_N];

    // 去交织：缓冲区数据为 [ch1, ch2, ch1, ch2, ...]
    for (uint32_t i = 0; i < FRAME_N; i++)
    {
        ch1_data[i] = g_audio.buffer[ready_buffer][2 * i];
        ch2_data[i] = g_audio.buffer[ready_buffer][2 * i + 1];
    }

    *ch1 = ch1_data;
    *ch2 = ch2_data;

    return 1;
}

/**
 * @brief 标记当前帧已被处理
 */
void AudioCapture_FrameConsumed(void)
{
    g_audio.frame_ready = 0;
}

/**
 * @brief DMA 半传输回调
 * 当缓冲区前半部分填满时调用
 */
void AudioCapture_HalfCpltCallback(void)
{
    // 第一半（缓冲区 0）就绪
    if (g_audio.frame_ready == 0) // 仅在未处理时设置
    {
        g_audio.frame_ready = 1;
    }
}

/**
 * @brief DMA 全传输回调
 * 当缓冲区后半部分填满时调用
 */
void AudioCapture_CpltCallback(void)
{
    // 第二半（缓冲区 1）就绪
    if (g_audio.frame_ready == 0) // 仅在未处理时设置
    {
        g_audio.frame_ready = 2;
    }
}

/**
 * @brief HAL ADC 半转换完成回调
 * 应在 stm32h7xx_it.c 或 main.c 中调用
 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc == g_hadc)
    {
        AudioCapture_HalfCpltCallback();
    }
}

/**
 * @brief HAL ADC 全转换完成回调
 * 应在 stm32h7xx_it.c 或 main.c 中调用
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc == g_hadc)
    {
        AudioCapture_CpltCallback();
    }
}
