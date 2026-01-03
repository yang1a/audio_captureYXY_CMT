# 快速入门指南

## 1. 前置要求

### 硬件
- [x] STM32H743 开发板
- [x] 2个模拟麦克风 (例如 MAX9814)
- [x] 1个舵机 (SG90 或类似)
- [x] USB-UART 适配器 (用于调试,可选)
- [x] 连接线若干

### 软件
- [x] STM32CubeMX (最新版)
- [x] CMake (3.22+)
- [x] ARM GCC 工具链
- [x] VSCode + CMake Tools 扩展
- [x] 串口终端 (PuTTY, Tera Term 等)

## 2. 五步快速部署

### 步骤 1: 配置 STM32CubeMX (15 分钟)

1. 打开 STM32CubeMX
2. 创建新项目,选择 STM32H743
3. 按照 `CUBEMX_CONFIG_GUIDE.md` 配置:
   - ✅ ADC1: 2通道扫描,DMA circular
   - ✅ TIM6: 48kHz 触发 ADC
   - ✅ TIM2: 50Hz PWM (舵机)
   - ✅ UART1: 115200 (调试)
4. 生成代码 (选择 CMake 工具链)

### 步骤 2: 集成项目代码 (5 分钟)

复制本项目的所有文件到 STM32CubeMX 生成的项目:

```bash
# 复制头文件
cp Core/Inc/*.h <你的项目>/Core/Inc/

# 复制源文件
cp Core/Src/utils.c <你的项目>/Core/Src/
cp Core/Src/audio_capture.c <你的项目>/Core/Src/
cp Core/Src/gcc_phat.c <你的项目>/Core/Src/
cp Core/Src/servo.c <你的项目>/Core/Src/
cp Core/Src/app.c <你的项目>/Core/Src/
cp Core/Src/retarget.c <你的项目>/Core/Src/

# 复制文档
cp *.md <你的项目>/
```

### 步骤 3: 更新 CMakeLists.txt (2 分钟)

在主 `CMakeLists.txt` 中:

```cmake
# 1. 添加源文件
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Src/utils.c
    Core/Src/audio_capture.c
    Core/Src/gcc_phat.c
    Core/Src/servo.c
    Core/Src/app.c
    Core/Src/retarget.c
)

# 2. 添加 CMSIS-DSP
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ARM_MATH_CM7
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Lib/GCC/libarm_cortexM7lfdp_math.a
)
```

### 步骤 4: 修改 main.cpp (3 分钟)

在 `Core/Src/main.cpp` 的 `USER CODE BEGIN Includes` 中:

```cpp
#include "app.h"
#include "retarget.h"
#include <stdio.h>
```

在 `USER CODE BEGIN 2` 中:

```cpp
// 1. 初始化 printf 重定向
extern UART_HandleTypeDef huart1;
RetargetInit(&huart1);

// 2. 初始化应用
extern TIM_HandleTypeDef htim2;  // 舵机定时器
if (App_Init(&hadc1, &htim2, TIM_CHANNEL_1) != 0)
{
    printf("App initialization failed!\r\n");
    Error_Handler();
}

// 3. 启动应用
if (App_Start() != 0)
{
    printf("App start failed!\r\n");
    Error_Handler();
}

printf("System started!\r\n");
```

在 `USER CODE BEGIN WHILE` 的 while 循环中:

```cpp
App_Loop();
```

### 步骤 5: 编译和烧录 (5 分钟)

```bash
# 1. 配置
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# 2. 编译
cmake --build .

# 3. 烧录 (使用你的工具,例如 OpenOCD、ST-Link 等)
# openocd -f interface/stlink.cfg -f target/stm32h7x.cfg -c "program project.elf verify reset exit"
# 或使用 STM32CubeProgrammer
```

## 3. 硬件连接

### 麦克风连接
```
Mic 1 OUT → PA0 (ADC1_IN0)
Mic 1 GND → GND
Mic 1 VCC → 3.3V

Mic 2 OUT → PA1 (ADC1_IN1)
Mic 2 GND → GND
Mic 2 VCC → 3.3V
```

**重要**: 两个麦克风必须:
- 水平放置,间距 12cm
- 朝向相同方向
- 固定牢固不晃动

### 舵机连接
```
Servo Signal → PA5 (TIM2_CH1)
Servo GND → GND
Servo VCC → 5V (外部电源,不要用 MCU 供电!)
```

### UART 连接 (调试)
```
UART TX (PA9) → USB-UART RX
UART RX (PA10) → USB-UART TX
GND → GND
```

## 4. 第一次运行

### 打开串口监视器

- 波特率: 115200
- 数据位: 8
- 停止位: 1
- 无校验

### 预期输出

```
STM32H7 Dual-Mic GCC-PHAT DOA System
Initializing...
App initialized successfully
App started
System started!

Lag: 0.00, dt: 0.000000 s, theta: 0.0 deg, peak: 0.050, ratio: 1.2, valid: 0
Low confidence - servo not updated
...
```

### 测试声源定位

1. **拍手测试**
   - 在麦克风左侧拍手
   - 观察输出: theta 应为负值
   - 舵机应向左转

2. **语音测试**
   - 在麦克风右侧说话
   - 观察输出: theta 应为正值
   - 舵机应向右转

3. **正前方测试**
   - 在麦克风正前方拍手
   - theta 应接近 0

## 5. 调试技巧

### 问题: 无串口输出

**检查**:
1. UART 是否正确配置
2. `RetargetInit` 是否调用
3. 串口波特率是否匹配
4. TX/RX 是否接反

### 问题: 舵机不动

**检查**:
1. PWM 是否有输出 (用示波器)
2. 舵机供电是否充足 (需要 5V 外部电源)
3. 定时器配置是否正确 (50Hz, 20ms)
4. 查看串口输出的 `valid` 字段

### 问题: 角度不准确

**检查**:
1. 麦克风间距是否为 12cm (修改 `config.h` 的 `MIC_DIST_M`)
2. ADC 采样率是否为 48kHz (检查定时器配置)
3. 麦克风是否对齐

### 问题: 舵机抖动严重

**调整** `config.h`:
```c
#define PEAK_MIN      0.25f    // 提高阈值
#define RATIO_MIN     2.0f     // 提高比值
#define ALPHA_SMOOTH  0.1f     // 降低平滑系数 (更平滑)
```

### 问题: 编译错误 "undefined reference to arm_rfft_fast_f32"

**解决**: 参考 `CMSIS_DSP_INTEGRATION.md` 集成 CMSIS-DSP 库

## 6. 参数调优

### 环境适应

| 环境 | PEAK_MIN | RATIO_MIN | ALPHA_SMOOTH |
|------|----------|-----------|--------------|
| 安静室内 | 0.10 | 1.3 | 0.3 |
| 正常室内 | 0.15 | 1.5 | 0.2 |
| 嘈杂环境 | 0.25 | 2.0 | 0.15 |
| 混响严重 | 0.30 | 2.5 | 0.1 |

### 响应速度 vs 稳定性

- **响应快** → 提高 `ALPHA_SMOOTH` (0.3-0.5)
- **稳定性好** → 降低 `ALPHA_SMOOTH` (0.1-0.2)

## 7. 性能验证

### 基准测试

运行以下测试确认系统正常:

1. **延迟测试**
   - 拍手 → 舵机响应
   - 预期: < 100ms

2. **精度测试**
   - 放置声源在已知角度
   - 记录估计角度
   - 误差应 < ±10°

3. **稳定性测试**
   - 持续发声 10 秒
   - 观察舵机是否稳定指向

## 8. 进阶功能

### 增加采样率

在 `config.h` 修改:
```c
#define FS_HZ     96000  // 更高精度
```

**注意**: 需要重新计算 TIM6 配置

### 更改麦克风间距

```c
#define MIC_DIST_M    0.15f  // 15cm 间距
```

更大间距 → 更高角度分辨率

### 添加 LCD 显示

在 `app.c` 的 `App_Loop` 中添加:
```c
char buf[32];
sprintf(buf, "Angle: %.1f", theta_smooth);
LCD_Print(buf);
```

## 9. 故障排除清单

- [ ] STM32CubeMX 配置正确
- [ ] 所有源文件已添加到 CMakeLists.txt
- [ ] CMSIS-DSP 库已正确链接
- [ ] FPU 已启用 (-mfpu=fpv5-d16 -mfloat-abi=hard)
- [ ] 硬件连接正确
- [ ] 舵机有独立供电
- [ ] ADC 采样率正确 (48kHz)
- [ ] 麦克风间距准确

## 10. 获取帮助

遇到问题时:

1. **查阅文档**
   - `README_DOA.md` - 系统说明
   - `CUBEMX_CONFIG_GUIDE.md` - 配置指南
   - `CMSIS_DSP_INTEGRATION.md` - DSP 集成

2. **检查日志**
   - 串口输出的调试信息
   - 重点关注 `valid` 字段

3. **调试工具**
   - 串口监视器
   - 示波器 (检查 PWM 和 ADC)
   - ST-Link 调试器

## 恭喜!

如果你看到舵机能够跟随声源移动,说明系统已经成功运行! 🎉

接下来可以:
- 调优参数以适应你的环境
- 添加更多功能 (LCD 显示、LED 指示等)
- 尝试更复杂的声源定位算法

祝你玩得开心! 🚀
