/**
 ******************************************************************************
 * @file           : utils.c
 * @brief          : Utility functions implementation
 ******************************************************************************
 */

#include "utils.h"
#include "config.h"
#include <math.h>

/**
 * @brief Clamp a float value between min and max
 */
float clamp_f(float value, float min, float max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

/**
 * @brief Calculate mean (average) of a float array
 */
float mean_f(const float *data, uint32_t len)
{
    if (len == 0)
        return 0.0f;

    float sum = 0.0f;
    for (uint32_t i = 0; i < len; i++)
    {
        sum += data[i];
    }
    return sum / (float)len;
}

/**
 * @brief Remove DC component from signal (subtract mean)
 */
void remove_dc_f(float *data, uint32_t len)
{
    float dc = mean_f(data, len);
    for (uint32_t i = 0; i < len; i++)
    {
        data[i] -= dc;
    }
}

/**
 * @brief Apply Hanning window to signal
 */
void apply_hanning_window_f(float *data, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        float w = 0.5f * (1.0f - cosf(2.0f * M_PI * (float)i / (float)(len - 1)));
        data[i] *= w;
    }
}

/**
 * @brief Generate Hanning window coefficients
 */
void generate_hanning_window_f(float *window, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        window[i] = 0.5f * (1.0f - cosf(2.0f * M_PI * (float)i / (float)(len - 1)));
    }
}

/**
 * @brief Convert uint16_t ADC samples to float32
 */
void adc_to_float(const uint16_t *src, float *dst, uint32_t len)
{
    // Convert ADC values (0-65535) to float (-1.0 to 1.0)
    // Assuming 16-bit ADC resolution
    const float scale = 1.0f / 32768.0f;

    for (uint32_t i = 0; i < len; i++)
    {
        // Convert to signed and normalize
        dst[i] = ((float)((int32_t)src[i] - 32768)) * scale;
    }
}
