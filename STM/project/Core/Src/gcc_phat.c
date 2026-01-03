/**
 ******************************************************************************
 * @file           : gcc_phat.c
 * @brief          : 用于 TDOA 估计的 GCC-PHAT 算法实现
 ******************************************************************************
 */

#include "gcc_phat.h"
#include "utils.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include <string.h>
#include <math.h>

/* 私有缓冲区 - 为 CMSIS-DSP 对齐 */
static float32_t x1_fft_input[FFT_L] __attribute__((aligned(4)));
static float32_t x2_fft_input[FFT_L] __attribute__((aligned(4)));
static float32_t X1[FFT_L * 2] __attribute__((aligned(4))); // 复数：实部、虚部交错
static float32_t X2[FFT_L * 2] __attribute__((aligned(4)));
static float32_t G[FFT_L * 2] __attribute__((aligned(4)));
static float32_t r_complex[FFT_L * 2] __attribute__((aligned(4)));
static float32_t r_real[FFT_L] __attribute__((aligned(4)));

/* FFT 实例 */
static arm_rfft_fast_instance_f32 fft_instance;
static uint8_t initialized = 0;

/**
 * @brief 初始化 GCC-PHAT 模块
 */
int GccPhat_Init(void)
{
    // 为 FFT_L 初始化 RFFT 实例
    arm_status status = arm_rfft_fast_init_f32(&fft_instance, FFT_L);

    if (status != ARM_MATH_SUCCESS)
        return -1;

    initialized = 1;
    return 0;
}

/**
 * @brief 反初始化 GCC-PHAT 模块
 */
void GccPhat_DeInit(void)
{
    initialized = 0;
}

/**
 * @brief 在窗口内寻找峰值并返回索引
 */
static uint32_t find_peak_in_window(const float32_t *data, uint32_t center,
                                    uint32_t window_half, uint32_t len,
                                    float32_t *peak_value)
{
    uint32_t start = (center > window_half) ? (center - window_half) : 0;
    uint32_t end = (center + window_half < len) ? (center + window_half) : (len - 1);

    uint32_t max_idx = start;
    float32_t max_val = data[start];

    for (uint32_t i = start; i <= end; i++)
    {
        if (data[i] > max_val)
        {
            max_val = data[i];
            max_idx = i;
        }
    }

    *peak_value = max_val;
    return max_idx;
}

/**
 * @brief 寻找第二峰值（排除第一峰附近的区域）
 */
static float32_t find_second_peak(const float32_t *data, uint32_t center,
                                  uint32_t window_half, uint32_t first_peak_idx,
                                  uint32_t len)
{
    uint32_t start = (center > window_half) ? (center - window_half) : 0;
    uint32_t end = (center + window_half < len) ? (center + window_half) : (len - 1);

    float32_t second_max = 0.0f;

    for (uint32_t i = start; i <= end; i++)
    {
        // 排除第一峰左右各 2 个采样点
        if (i >= first_peak_idx - 2 && i <= first_peak_idx + 2)
            continue;

        if (data[i] > second_max)
        {
            second_max = data[i];
        }
    }

    return second_max;
}

/**
 * @brief 抛物线插值，获取次采样级峰值位置
 */
static float32_t parabolic_interpolation(float32_t r1, float32_t r2, float32_t r3)
{
    float32_t denom = r1 - 2.0f * r2 + r3;

    // 避免除零
    if (fabsf(denom) < 1e-10f)
        return 0.0f;

    float32_t delta = (r1 - r3) / (2.0f * denom);

    // 将结果限制在 ±0.5 之间
    return clamp_f(delta, -0.5f, 0.5f);
}

/**
 * @brief 使用 GCC-PHAT 算法处理一帧
 */
int GccPhat_Process(const float *x1, const float *x2, GccPhat_Result_t *result)
{
    if (!initialized || x1 == NULL || x2 == NULL || result == NULL)
        return -1;

    // 初始化结果
    memset(result, 0, sizeof(GccPhat_Result_t));

    /* 步骤 1：预处理 - 去直流并加窗 */
    // 拷贝并处理 x1
    memcpy(x1_fft_input, x1, FRAME_N * sizeof(float32_t));
    remove_dc_f(x1_fft_input, FRAME_N);
    apply_hanning_window_f(x1_fft_input, FRAME_N);

    // 拷贝并处理 x2
    memcpy(x2_fft_input, x2, FRAME_N * sizeof(float32_t));
    remove_dc_f(x2_fft_input, FRAME_N);
    apply_hanning_window_f(x2_fft_input, FRAME_N);

    /* 步骤 2：零填充 */
    memset(x1_fft_input + FRAME_N, 0, (FFT_L - FRAME_N) * sizeof(float32_t));
    memset(x2_fft_input + FRAME_N, 0, (FFT_L - FRAME_N) * sizeof(float32_t));

    /* 步骤 3：FFT */
    arm_rfft_fast_f32(&fft_instance, x1_fft_input, X1, 0);
    arm_rfft_fast_f32(&fft_instance, x2_fft_input, X2, 0);

    /* 步骤 4：互功率谱 G = X1 * conj(X2) */
    for (uint32_t i = 0; i < FFT_L; i++)
    {
        float32_t x1_re = X1[2 * i];
        float32_t x1_im = X1[2 * i + 1];
        float32_t x2_re = X2[2 * i];
        float32_t x2_im = X2[2 * i + 1];

        // 复数乘法：(a+bi) * (c-di) = (ac+bd) + (bc-ad)i
        G[2 * i] = x1_re * x2_re + x1_im * x2_im;
        G[2 * i + 1] = x1_im * x2_re - x1_re * x2_im;
    }

    /* 步骤 5：PHAT 加权 G /= |G| + eps */
    for (uint32_t i = 0; i < FFT_L; i++)
    {
        float32_t g_re = G[2 * i];
        float32_t g_im = G[2 * i + 1];
        float32_t magnitude = sqrtf(g_re * g_re + g_im * g_im) + EPS_PHAT;

        G[2 * i] = g_re / magnitude;
        G[2 * i + 1] = g_im / magnitude;
    }

    /* 步骤 6：IFFT */
    arm_rfft_fast_f32(&fft_instance, G, r_complex, 1);

    /* 步骤 7：提取实部并进行 fftshift */
    // CMSIS-DSP RFFT 输出格式：[real0, real1, ..., realN/2, imag(N/2-1), ..., imag1]
    // 简化处理：直接视作相关结果
    // 需要 fftshift：将零时延移到中心

    // 简单做法：前半段移到尾部，后半段移到开头
    uint32_t half = FFT_L / 2;
    for (uint32_t i = 0; i < half; i++)
    {
        r_real[half + i] = r_complex[i];
        r_real[i] = r_complex[half + i];
    }

    /* 步骤 8：在物理可达窗口内寻找峰值 */
    uint32_t center = FFT_L / 2;
    float32_t peak_value = 0.0f;
    uint32_t peak_idx = find_peak_in_window(r_real, center, N_MAX, FFT_L, &peak_value);

    result->peak_value = peak_value;

    /* 步骤 9：置信度检查 - 峰值阈值 */
    if (peak_value < PEAK_MIN)
    {
        result->is_valid = 0;
        return 0; // Low confidence
    }

    /* 步骤 10：置信度检查 - 峰值比 */
    float32_t second_peak = find_second_peak(r_real, center, N_MAX, peak_idx, FFT_L);
    float32_t ratio = peak_value / (second_peak + EPS_PHAT);
    result->peak_ratio = ratio;

    if (ratio < RATIO_MIN)
    {
        result->is_valid = 0;
        return 0; // Low confidence
    }

    /* 步骤 11：次采样插值（三点抛物线） */
    float32_t delta = 0.0f;
    if (peak_idx > 0 && peak_idx < FFT_L - 1)
    {
        delta = parabolic_interpolation(r_real[peak_idx - 1],
                                        r_real[peak_idx],
                                        r_real[peak_idx + 1]);
    }

    /* 步骤 12：计算以采样点计的时延 */
    int32_t lag_int = (int32_t)peak_idx - (int32_t)center;
    result->lag_sub = (float32_t)lag_int + delta;

    /* 步骤 13：换算为时间差 */
    result->dt = result->lag_sub / (float32_t)FS_HZ;

    /* 步骤 14：换算为角度 */
    float32_t x = (SPEED_SOUND * result->dt) / MIC_DIST_M;
    x = clamp_f(x, -1.0f, 1.0f); // 确保 asin 的输入有效

    result->theta_deg = RAD_TO_DEG(asinf(x));
    result->is_valid = 1;

    return 0;
}
