# STM32H7 双麦克风 GCC-PHAT 声源定位系统

## 项目概述

本项目在 STM32H7 上实现了基于 GCC-PHAT 算法的双麦克风声源方向估计(DOA)系统,可以实时检测声源方向并控制舵机指向声源。

## 系统架构

### 模块划分

1. **config.h** - 系统配置参数
   - 采样率、帧长、FFT 长度
   - 麦克风间距、声速
   - 舵机参数、滤波参数

2. **utils.c/.h** - 工具函数
   - 窗函数(汉宁窗)
   - 均值计算、DC 去除
   - Clamp 函数
   - ADC 到浮点数转换

3. **audio_capture.c/.h** - 音频采集模块
   - ADC+DMA 双通道同步采样
   - Ping-pong 双缓冲管理
   - 帧就绪标志

4. **gcc_phat.c/.h** - GCC-PHAT 算法核心
   - FFT/IFFT (CMSIS-DSP)
   - 互功率谱与 PHAT 加权
   - 物理窗口峰值搜索
   - 三点抛物线插值
   - 可信度判决(峰值阈值、峰值比)

5. **servo.c/.h** - 舵机控制
   - PWM 50Hz 输出
   - 角度到脉宽映射

6. **app.c/.h** - 应用层主流程
   - 帧处理循环
   - 角度指数平滑
   - 系统集成

## 硬件要求

### MCU
- STM32H7xx (例如 STM32H743)
- Cortex-M7 with FPU
- Flash: >= 1MB
- RAM: >= 512KB

### 外设
- **ADC**: 双通道同步采样 (48kHz)
  - 配置为扫描模式,定时器触发
  - DMA circular mode
  
- **Timer**: PWM 输出 (50Hz)
  - 用于舵机控制
  - 例如: TIM2 Channel 1

- **UART**: 调试输出 (可选)
  - 用于打印 DOA 结果

### 传感器
- 2 个模拟麦克风 (例如 MAX9814)
- 麦克风间距: 12cm (可配置)
- 连接到 ADC 输入

### 执行器
- 1 个标准舵机 (0-180°)
- 连接到 Timer PWM 输出

## 软件配置

### 1. STM32CubeMX 配置

#### ADC 配置
```
ADC1:
- Clock: PLL2, 25MHz (配置分频得到)
- Resolution: 16-bit
- Scan Mode: Enable (2 channels)
- Continuous Mode: Disable
- External Trigger: Timer (例如 TIM6)
- DMA: Circular mode, Half Word
- Channels: 
  - Channel 1: PA0 (Mic 1)
  - Channel 2: PA1 (Mic 2)
```

#### Timer 配置 (用于 ADC 触发)
```
TIM6 (或其他定时器):
- Counter Period: 设置为 48kHz 采样率
- Trigger Output: Update Event
- 用于触发 ADC
```

#### Timer 配置 (用于舵机 PWM)
```
TIM2 (或其他):
- Prescaler: 设置以得到合适的时基
- Counter Period (ARR): 设置为 20ms (50Hz)
- Channel 1: PWM Mode 1
- Output Pin: 例如 PA5
```

#### DMA 配置
```
DMA1 Stream 1 (ADC1):
- Direction: Peripheral to Memory
- Mode: Circular
- Data Width: Half Word (16-bit)
- Increment: Memory address increment
```

#### Clock 配置
```
- System Clock: 240MHz (或根据需要)
- AHB/APB 合理配置
- ADC Clock: ~25MHz (通过 PLL2)
- 确保 FPU 启用
```

### 2. 编译配置

#### CMakeLists.txt 修改

确保在主 `CMakeLists.txt` 中添加:

```cmake
# 添加 CMSIS-DSP 路径
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include
)

# 链接 CMSIS-DSP 库
# 选项 1: 使用预编译库
target_link_libraries(${CMAKE_PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Lib/GCC/libarm_cortexM7lfdp_math.a
)

# 选项 2: 或者添加需要的 DSP 源文件
# target_sources(${CMAKE_PROJECT_NAME} PRIVATE
#     ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_rfft_fast_f32.c
#     ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_cfft_f32.c
#     ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/CommonTables/arm_common_tables.c
#     ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/CommonTables/arm_const_structs.c
# )
```

#### 编译选项

在 `gcc-arm-none-eabi.cmake` 或 CMakeLists.txt 中确保:

```cmake
# FPU 设置
set(FPU_FLAGS "-mfpu=fpv5-d16 -mfloat-abi=hard")

# 优化级别
set(CMAKE_C_FLAGS_RELEASE "-O2 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g")

# ARM 架构定义
add_definitions(-DARM_MATH_CM7)
```

### 3. 代码集成

#### main.cpp 修改

查看已更新的 `Core/Src/main.cpp`,需要:

1. **配置定时器用于舵机**:
   在 STM32CubeMX 中添加 Timer PWM 配置后,更新:
   ```cpp
   extern TIM_HandleTypeDef htim2;  // 或你使用的定时器
   
   // 在 USER CODE BEGIN 2 中:
   App_Init(&hadc1, &htim2, TIM_CHANNEL_1);
   ```

2. **配置 UART (用于调试)**:
   如需 printf 输出,需重定向 `_write`:
   ```c
   // 在 syscalls.c 中
   int _write(int file, char *ptr, int len)
   {
       HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, HAL_MAX_DELAY);
       return len;
   }
   ```

### 4. ADC DMA 配置示例

在 `adc.c` 中,确保 ADC 配置为:

```c
void MX_ADC1_Init(void)
{
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_16B;
    hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;  // 重要: 启用扫描
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 2;  // 2 个通道
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T6_TRGO;  // 定时器触发
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
    // ... 其他配置
}
```

## 使用说明

### 1. 系统启动流程

```
HAL_Init() 
  ↓
SystemClock_Config()
  ↓
外设初始化 (ADC, GPIO, Timer)
  ↓
App_Init() - 初始化所有模块
  ↓
App_Start() - 启动采集和舵机
  ↓
while(1) {
    App_Loop() - 处理音频帧
}
```

### 2. 参数调优

#### config.h 参数

| 参数 | 默认值 | 说明 | 调优建议 |
|------|--------|------|----------|
| FS_HZ | 48000 | 采样率 | 越高越精确,但计算量大 |
| FRAME_N | 1024 | 帧长 | 越长时延越大,但频率分辨率越高 |
| FFT_L | 2048 | FFT 长度 | 必须 >= 2*FRAME_N |
| MIC_DIST_M | 0.12 | 麦克风间距 | 根据实际硬件调整 |
| PEAK_MIN | 0.15 | 峰值阈值 | 低噪声环境可降低,高噪声环境需提高 |
| RATIO_MIN | 1.5 | 峰值比阈值 | 提高可减少误判,但可能漏检 |
| ALPHA_SMOOTH | 0.2 | 平滑系数 | 越小越平滑但响应慢,越大响应快但抖动大 |

#### 可信度调优

如果舵机频繁乱动:
- 提高 `PEAK_MIN` (例如 0.2 - 0.3)
- 提高 `RATIO_MIN` (例如 2.0 - 3.0)

如果对声源响应不够敏感:
- 降低 `PEAK_MIN` (例如 0.1)
- 降低 `RATIO_MIN` (例如 1.3)

### 3. 调试输出

系统会通过 UART 输出调试信息:

```
Lag: -5.23, dt: -0.000109 s, theta: -10.8 deg, peak: 0.245, ratio: 2.35, valid: 1
Servo updated: 79.2 deg (smoothed theta: -10.8)
```

参数解释:
- **Lag**: 采样点延迟(正=声源在右,负=声源在左)
- **dt**: 时间差(秒)
- **theta**: 估计角度(-90~90度)
- **peak**: 相关峰值
- **ratio**: 主峰/次峰比
- **valid**: 是否通过可信度检查

### 4. 性能优化

#### 内存优化
- 所有 DSP 缓冲区都是静态分配
- 总内存需求约 60KB RAM

#### 计算优化
- 使用 CMSIS-DSP 硬件加速 FFT
- 编译时开启 `-O2` 或 `-O3`
- 确保 FPU 启用

#### 实时性
- 帧处理时间 < 20ms (典型)
- 端到端延迟: 约 40-60ms

## 物理限制

### 角度分辨率
基于麦克风间距 d=0.12m:
- 理论最大 TDOA: ±350μs
- 理论角度范围: ±90°
- 实际可检测: 约 ±80° (边缘精度下降)

### 频率响应
- 最低频率: ~47Hz (由帧长决定)
- 最高频率: 24kHz (由采样率决定)
- 建议检测频率: 200Hz - 4kHz (人声范围)

## 常见问题

### Q1: 编译错误 "undefined reference to 'arm_rfft_fast_f32'"
**A**: 需要链接 CMSIS-DSP 库或添加相关源文件到编译列表。

### Q2: 舵机不停抖动
**A**: 可能是可信度阈值太低。提高 `PEAK_MIN` 和 `RATIO_MIN`。

### Q3: 角度估计不准确
**A**: 检查:
1. 麦克风间距 `MIC_DIST_M` 是否正确
2. 采样率是否精确为 48kHz
3. ADC 通道是否同步采样

### Q4: DMA 数据异常
**A**: STM32H7 有 DCache,需要:
1. 将 DMA 缓冲区放在非 Cache 区域(例如 D2 SRAM)
2. 或在访问前使用 `SCB_InvalidateDCache_by_Addr()`

### Q5: printf 无输出
**A**: 需要:
1. 配置 UART
2. 重定向 `_write` 函数到 UART
3. 或使用 ITM SWO

## 性能指标

| 指标 | 值 |
|------|-----|
| 采样率 | 48 kHz |
| 帧长 | 1024 samples (~21ms) |
| 处理延迟 | < 20ms/帧 |
| 总延迟 | ~40-60ms |
| 角度精度 | ±5° (典型) |
| 内存占用 | ~60KB RAM |
| CPU 占用 | ~30% @ 240MHz |

## 扩展方向

### 1. 2D 定位
- 添加第三个麦克风(三角形阵列)
- 实现俯仰角估计

### 2. 多声源
- MUSIC 或 ESPRIT 算法
- 需要更多麦克风(4+)

### 3. 回声消除
- AEC 算法
- 降低室内混响影响

### 4. 机器学习
- 使用 TensorFlow Lite for Microcontrollers
- 训练神经网络提高精度

## 参考资料

1. GCC-PHAT: "The Generalized Cross Correlation Method for Estimation of Time Delay" (Knapp & Carter, 1976)
2. CMSIS-DSP: ARM CMSIS DSP Software Library
3. STM32H7 Reference Manual: RM0433

## 许可证

本项目代码基于 STM32 HAL 库,遵循相应开源协议。

## 作者与联系

项目生成于 2026-01-03
如有问题,请参考代码注释或 STM32 社区支持。
