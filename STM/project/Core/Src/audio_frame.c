/**
 * @file audio_frame.c
 * @brief DMA 音频帧管理模块实现
 */
#include "audio_frame.h"
#include "adc_dma.h"
#include <string.h>

/* DMA 缓冲区 - 放在 D2 SRAM 避免 DCache 问题 */
/* 双通道交错存储: [CH0, CH1, CH0, CH1, ...] */
__attribute__((section(".bss"), aligned(32))) static uint16_t dma_buffer[DMA_BUFFER_SIZE];

/* 帧就绪标志 */
static volatile bool frame_ready = false;
static volatile uint8_t active_half = 0; /* 0: 前半, 1: 后半 */

/**
 * @brief 初始化音频帧模块
 */
HAL_StatusTypeDef audio_frame_init(void)
{
    /* 清零缓冲区 */
    memset(dma_buffer, 0, sizeof(dma_buffer));
    frame_ready = false;
    active_half = 0;

    /* 启动 ADC DMA 采集 */
    return ADC_DMA_Start(dma_buffer, DMA_BUFFER_SIZE);
}

/**
 * @brief 检查是否有帧可用
 */
bool audio_frame_available(void)
{
    return frame_ready;
}

/**
 * @brief 获取帧数据并转换为浮点
 */
void audio_frame_get(float *x1, float *x2)
{
    uint16_t *src;
    uint32_t i;

    /* 根据活动半区选择数据源 */
    if (active_half == 0)
    {
        /* 使用后半缓冲区（前半正在 DMA） */
        src = &dma_buffer[DMA_BUFFER_SIZE / 2];
    }
    else
    {
        /* 使用前半缓冲区（后半正在 DMA） */
        src = &dma_buffer[0];
    }

    /* STM32H7 DCache 失效处理 */
    SCB_InvalidateDCache_by_Addr((uint32_t *)src, DMA_BUFFER_SIZE);

    /* 分离双通道并转换为浮点 [-1, 1] */
    for (i = 0; i < FRAME_N; i++)
    {
        /* ADC 16位数据，转换为归一化浮点 */
        x1[i] = ((float)src[i * 2] / 32768.0f) - 1.0f;
        x2[i] = ((float)src[i * 2 + 1] / 32768.0f) - 1.0f;
    }

    /* 清除帧就绪标志 */
    frame_ready = false;
}

/**
 * @brief DMA 半传输完成回调
 */
void audio_frame_half_cplt_callback(void)
{
    active_half = 0;
    frame_ready = true;
}

/**
 * @brief DMA 全传输完成回调
 */
void audio_frame_cplt_callback(void)
{
    active_half = 1;
    frame_ready = true;
}

/**
 * @brief HAL DMA 半传输完成中断回调
 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        audio_frame_half_cplt_callback();
    }
}

/**
 * @brief HAL DMA 全传输完成中断回调
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        audio_frame_cplt_callback();
    }
}
