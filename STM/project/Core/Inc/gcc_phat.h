/**
 ******************************************************************************
 * @file           : gcc_phat.h
 * @brief          : 用于 TDOA 估计的 GCC-PHAT 算法头文件
 ******************************************************************************
 */

#ifndef GCC_PHAT_H
#define GCC_PHAT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "config.h"

    /**
     * @brief GCC-PHAT 结果结构体
     */
    typedef struct
    {
        float lag_sub;    // 次采样级时延（可为小数，单位采样点）
        float dt;         // 时间差 (秒)
        float theta_deg;  // 角度 (度，-90 到 +90)
        float peak_value; // 相关峰值
        float peak_ratio; // 主峰与次峰的比值
        uint8_t is_valid; // 置信标志 (1=有效, 0=无效)
    } GccPhat_Result_t;

    /**
     * @brief 初始化 GCC-PHAT 模块
     * 处理前需调用一次
     * @return 成功返回 0，失败返回 -1
     */
    int GccPhat_Init(void);

    /**
     * @brief 使用 GCC-PHAT 算法处理一帧
     * @param x1: 通道 1 浮点数据（长度 FRAME_N）
     * @param x2: 通道 2 浮点数据（长度 FRAME_N）
     * @param result: 输出结果结构体
     * @return 成功返回 0，失败返回 -1
     */
    int GccPhat_Process(const float *x1, const float *x2, GccPhat_Result_t *result);

    /**
     * @brief 反初始化 GCC-PHAT 模块（如需）
     */
    void GccPhat_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* GCC_PHAT_H */
