/**
 * @file gcc_phat.c
 * @brief GCC-PHAT 时延估计算法实现
 *
 * 使用 CMSIS-DSP 库实现 FFT/IFFT
 */
#include "gcc_phat.h"
#include "arm_math.h"
#include <math.h>
#include <string.h>

/* FFT 实例 */
static arm_rfft_fast_instance_f32 fft_inst;

/* 汉宁窗 */
static float hann_window[FRAME_N];

/* FFT 工作缓冲区 - 对齐到 32 字节 */
__attribute__((aligned(32))) static float fft_buf1[FFT_L];

__attribute__((aligned(32))) static float fft_buf2[FFT_L];

__attribute__((aligned(32))) static float cross_spectrum[FFT_L];

__attribute__((aligned(32))) static float gcc_output[FFT_L];

/* 临时预处理缓冲区 */
static float temp_x1[FRAME_N];
static float temp_x2[FRAME_N];

/**
 * @brief 初始化汉宁窗
 */
static void init_hann_window(void)
{
    for (uint32_t n = 0; n < FRAME_N; n++)
    {
        hann_window[n] = 0.5f * (1.0f - arm_cos_f32(2.0f * PI * (float)n / (float)(FRAME_N - 1)));
    }
}

/**
 * @brief 初始化 GCC-PHAT 模块
 */
void gcc_phat_init(void)
{
    /* 初始化 FFT */
    arm_rfft_fast_init_f32(&fft_inst, FFT_L);

    /* 初始化汉宁窗 */
    init_hann_window();

    /* 清零缓冲区 */
    memset(fft_buf1, 0, sizeof(fft_buf1));
    memset(fft_buf2, 0, sizeof(fft_buf2));
    memset(cross_spectrum, 0, sizeof(cross_spectrum));
    memset(gcc_output, 0, sizeof(gcc_output));
}

/**
 * @brief 预处理：去直流 + 加窗
 */
static void preprocess(const float *input, float *output)
{
    float mean = 0.0f;

    /* 计算均值（直流分量） */
    for (uint32_t i = 0; i < FRAME_N; i++)
    {
        mean += input[i];
    }
    mean /= (float)FRAME_N;

    /* 去直流 + 乘汉宁窗 */
    for (uint32_t i = 0; i < FRAME_N; i++)
    {
        output[i] = (input[i] - mean) * hann_window[i];
    }
}

/**
 * @brief 复数乘法: result = a * conj(b)
 * CMSIS-DSP 复数格式: [Re0, Im0, Re1, Im1, ...]
 */
static void complex_mult_conj(const float *a, const float *b, float *result, uint32_t len)
{
    for (uint32_t i = 0; i < len; i += 2)
    {
        float a_re = a[i];
        float a_im = a[i + 1];
        float b_re = b[i];
        float b_im = -b[i + 1]; /* 取共轭 */

        result[i] = a_re * b_re - a_im * b_im;     /* 实部 */
        result[i + 1] = a_re * b_im + a_im * b_re; /* 虚部 */
    }
}

/**
 * @brief PHAT 加权
 */
static void phat_weighting(float *spectrum, uint32_t len)
{
    for (uint32_t i = 0; i < len; i += 2)
    {
        float re = spectrum[i];
        float im = spectrum[i + 1];
        float mag = sqrtf(re * re + im * im) + EPS_PHAT;

        spectrum[i] = re / mag;
        spectrum[i + 1] = im / mag;
    }
}

/**
 * @brief FFT shift：将零延迟移到中心
 */
static void fftshift(float *data, uint32_t len)
{
    uint32_t half = len / 2;
    float temp;

    for (uint32_t i = 0; i < half; i++)
    {
        temp = data[i];
        data[i] = data[i + half];
        data[i + half] = temp;
    }
}

/**
 * @brief 在物理约束范围内寻找峰值
 * @param data GCC 输出（已 fftshift）
 * @param len 数据长度
 * @param peak_idx 输出峰值索引
 * @param peak_val 输出峰值
 * @param second_peak 输出次峰值
 */
static void find_peak_constrained(const float *data, uint32_t len,
                                  int32_t *peak_idx, float *peak_val, float *second_peak)
{
    int32_t center = (int32_t)(len / 2);
    int32_t search_start = center - (int32_t)MAX_LAG_SAMPLES;
    int32_t search_end = center + (int32_t)MAX_LAG_SAMPLES;

    /* 边界检查 */
    if (search_start < 0)
        search_start = 0;
    if (search_end >= (int32_t)len)
        search_end = (int32_t)len - 1;

    *peak_val = -1e10f;
    *second_peak = -1e10f;
    *peak_idx = center;

    /* 寻找峰值 */
    for (int32_t i = search_start; i <= search_end; i++)
    {
        float val = fabsf(data[i]);
        if (val > *peak_val)
        {
            *second_peak = *peak_val;
            *peak_val = val;
            *peak_idx = i;
        }
        else if (val > *second_peak)
        {
            *second_peak = val;
        }
    }
}

/**
 * @brief 三点抛物线插值求亚采样延迟
 */
static float parabolic_interp(const float *data, int32_t peak_idx, uint32_t len)
{
    if (peak_idx <= 0 || peak_idx >= (int32_t)(len - 1))
    {
        return (float)peak_idx;
    }

    float y0 = fabsf(data[peak_idx - 1]);
    float y1 = fabsf(data[peak_idx]);
    float y2 = fabsf(data[peak_idx + 1]);

    float denom = 2.0f * (2.0f * y1 - y0 - y2);
    if (fabsf(denom) < 1e-10f)
    {
        return (float)peak_idx;
    }

    float delta = (y0 - y2) / denom;

    /* 限制插值偏移在 [-0.5, 0.5] 范围内 */
    if (delta > 0.5f)
        delta = 0.5f;
    if (delta < -0.5f)
        delta = -0.5f;

    return (float)peak_idx + delta;
}

/**
 * @brief 执行 GCC-PHAT
 */
void gcc_phat_process(const float *x1, const float *x2, gcc_phat_result_t *result)
{
    /* 初始化结果 */
    result->valid = false;
    result->lag_sub = 0.0f;
    result->dt = 0.0f;
    result->theta_deg = 0.0f;
    result->peak = 0.0f;
    result->ratio = 0.0f;

    /* 1. 预处理：去直流 + 加窗 */
    preprocess(x1, temp_x1);
    preprocess(x2, temp_x2);

    /* 2. 零填充到 FFT_L */
    memset(fft_buf1, 0, sizeof(fft_buf1));
    memset(fft_buf2, 0, sizeof(fft_buf2));
    memcpy(fft_buf1, temp_x1, FRAME_N * sizeof(float));
    memcpy(fft_buf2, temp_x2, FRAME_N * sizeof(float));

    /* 3. FFT */
    arm_rfft_fast_f32(&fft_inst, fft_buf1, cross_spectrum, 0); /* 暂存到 cross_spectrum */
    memcpy(fft_buf1, cross_spectrum, FFT_L * sizeof(float));   /* 复制回 fft_buf1 */

    arm_rfft_fast_f32(&fft_inst, fft_buf2, cross_spectrum, 0); /* X2 的 FFT */
    memcpy(fft_buf2, cross_spectrum, FFT_L * sizeof(float));   /* 复制回 fft_buf2 */

    /* 4. 互功率谱: G(k) = X1(k) * conj(X2(k)) */
    complex_mult_conj(fft_buf1, fft_buf2, cross_spectrum, FFT_L);

    /* 5. PHAT 加权 */
    phat_weighting(cross_spectrum, FFT_L);

    /* 6. IFFT */
    arm_rfft_fast_f32(&fft_inst, cross_spectrum, gcc_output, 1);

    /* 7. FFT shift */
    fftshift(gcc_output, FFT_L);

    /* 8. 峰值搜索（物理约束） */
    int32_t peak_idx;
    float peak_val, second_peak;
    find_peak_constrained(gcc_output, FFT_L, &peak_idx, &peak_val, &second_peak);

    result->peak = peak_val;
    result->ratio = peak_val / (second_peak + EPS_PHAT);

    /* 9. 可信度判决 */
    if (peak_val < PEAK_MIN)
    {
        return; /* 峰值太小，放弃 */
    }

    if (result->ratio < RATIO_MIN)
    {
        return; /* 主峰/次峰比太小，放弃 */
    }

    /* 10. 亚采样插值 */
    float sub_idx = parabolic_interp(gcc_output, peak_idx, FFT_L);

    /* 转换为相对于中心的延迟 */
    float lag = sub_idx - (float)(FFT_L / 2);
    result->lag_sub = lag;

    /* 11. 计算时间差 */
    result->dt = lag / (float)FS_HZ;

    /* 12. 计算角度 */
    float sin_theta = (SOUND_SPEED * result->dt) / MIC_DIST_M;

    /* clamp 到 [-1, 1] */
    if (sin_theta > 1.0f)
        sin_theta = 1.0f;
    if (sin_theta < -1.0f)
        sin_theta = -1.0f;

    result->theta_deg = asinf(sin_theta) * 180.0f / PI;
    result->valid = true;
}
