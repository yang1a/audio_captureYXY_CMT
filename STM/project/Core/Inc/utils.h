/**
 ******************************************************************************
 * @file           : utils.h
 * @brief          : 工具函数头文件
 ******************************************************************************
 */

#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    /**
     * @brief 将浮点数限制在最小值与最大值之间
     * @param value: 输入值
     * @param min: 最小值
     * @param max: 最大值
     * @return 限制后的值
     */
    float clamp_f(float value, float min, float max);

    /**
     * @brief 计算浮点数组的均值
     * @param data: 输入数组
     * @param len: 数组长度
     * @return 均值
     */
    float mean_f(const float *data, uint32_t len);

    /**
     * @brief 去除信号直流分量（减去均值）
     * @param data: 输入/输出数组（就地修改）
     * @param len: 数组长度
     */
    void remove_dc_f(float *data, uint32_t len);

    /**
     * @brief 对信号施加 Hanning 窗
     * @param data: 输入/输出数组（就地修改）
     * @param len: 数组长度
     */
    void apply_hanning_window_f(float *data, uint32_t len);

    /**
     * @brief 生成 Hanning 窗系数
     * @param window: 输出窗数组
     * @param len: 窗长度
     */
    void generate_hanning_window_f(float *window, uint32_t len);

    /**
     * @brief 将 uint16_t 的 ADC 采样转换为 float32
     * @param src: 源 ADC 数据 (uint16_t)
     * @param dst: 目标浮点数组
     * @param len: 采样点数
     */
    void adc_to_float(const uint16_t *src, float *dst, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* UTILS_H */
