/**
 * @file gcc_phat.h
 * @brief GCC-PHAT 时延估计算法模块
 *
 * 实现基于广义互相关-相位变换的时延估计算法
 */
#ifndef __GCC_PHAT_H__
#define __GCC_PHAT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "config.h"
#include <stdbool.h>

    /**
     * @brief GCC-PHAT 计算结果结构体
     */
    typedef struct
    {
        float lag_sub;   /* 亚采样精度延迟（采样点） */
        float dt;        /* 时间差 (s) */
        float theta_deg; /* 角度 (度) */
        float peak;      /* 主峰值 */
        float ratio;     /* 主峰/次峰比 */
        bool valid;      /* 结果是否有效 */
    } gcc_phat_result_t;

    /**
     * @brief 初始化 GCC-PHAT 模块
     * @note 初始化 FFT 实例和汉宁窗
     */
    void gcc_phat_init(void);

    /**
     * @brief 执行 GCC-PHAT 时延估计
     * @param x1 麦克风1数据 (长度 FRAME_N)
     * @param x2 麦克风2数据 (长度 FRAME_N)
     * @param result 输出结果结构体
     */
    void gcc_phat_process(const float *x1, const float *x2, gcc_phat_result_t *result);

#ifdef __cplusplus
}
#endif

#endif /* __GCC_PHAT_H__ */
