/**
 * @file app_doa.h
 * @brief DOA 主业务逻辑模块
 *
 * 整合音频采集、GCC-PHAT 算法、舵机控制的主业务流程
 */
#ifndef __APP_DOA_H__
#define __APP_DOA_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include <stdbool.h>

    /**
     * @brief 初始化 DOA 系统
     * @retval HAL_StatusTypeDef
     */
    HAL_StatusTypeDef app_doa_init(void);

    /**
     * @brief 处理一帧音频数据
     * @note 执行 GCC-PHAT 算法和可信度判决
     */
    void app_doa_process_frame(void);

    /**
     * @brief 检查是否有新帧需要处理
     * @retval true: 有帧可处理; false: 无
     */
    bool app_doa_frame_ready(void);

    /**
     * @brief 打印调试信息
     */
    void app_doa_debug_print(void);

    /**
     * @brief 更新舵机位置
     */
    void app_doa_servo_update(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_DOA_H__ */
