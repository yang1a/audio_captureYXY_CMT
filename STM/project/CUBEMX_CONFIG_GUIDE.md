# STM32CubeMX 配置步骤详解

## 1. 基础配置

### Project Manager
- Project Name: `music_find` (或你的项目名)
- Toolchain: `CMake`
- MCU: `STM32H743xx` (或你使用的型号)

## 2. 时钟配置 (Clock Configuration)

### RCC Configuration
```
HSI: 64 MHz
PLL Source: HSI
PLLM: 4
PLLN: 30
PLLP: 2
System Clock: 240 MHz (或根据需要)
```

### PLL2 (用于 ADC)
```
PLL2M: 4
PLL2N: 10
PLL2P: 2
PLL2 Output: ~25 MHz
ADC Clock Source: PLL2P
```

## 3. ADC 配置

### ADC1 基本配置

**Mode**:
- Independent mode
- Scan Conversion Mode: Enabled
- Continuous Conversion Mode: Disabled
- Number of Conversion: 2

**Configuration**:
```
Clock Prescaler: Asynchronous clock mode divided by 2
Resolution: 16 bits (ADC_RESOLUTION_16B)
Data Alignment: Right alignment
External Trigger: Timer 6 Trigger Out event (或其他定时器)
External Trigger Edge: Rising edge
Conversion Data Management: DMA Circular Mode
Overrun: Data preserved
```

**Regular Channel 1** (Mic 1):
```
Channel: ADC1_INP0 (PA0_C)
Rank: 1
Sampling Time: 64.5 cycles
Single Ended
```

**Regular Channel 2** (Mic 2):
```
Channel: ADC1_INP1 (PA1_C)
Rank: 2
Sampling Time: 64.5 cycles
Single Ended
```

### ADC DMA 配置

**DMA Request**: ADC1
```
DMA1 Stream X
Direction: Peripheral to Memory
Priority: High
Mode: Circular
Increment Address:
  - Peripheral: No
  - Memory: Yes
Data Width:
  - Peripheral: Half Word (16 bits)
  - Memory: Half Word (16 bits)
```

**DMA Interrupts**:
- Transfer Complete
- Half Transfer

## 4. 定时器配置 (用于 ADC 触发)

### TIM6 配置 (或 TIM7)

**Basic Parameters**:
```
Prescaler: 计算得到合适的时基
Counter Period (ARR): 计算得到 48 kHz 采样率
Auto-reload preload: Enable
Trigger Output (TRGO): Update Event
```

**计算示例** (假设 Timer Clock = 240 MHz):
```
采样率 = 48000 Hz
Timer_Freq = 240,000,000 Hz

Prescaler = 0 (不分频)
ARR = (240,000,000 / 48,000) - 1 = 4999

或者:
Prescaler = 4 - 1 = 3
ARR = (240,000,000 / 4 / 48,000) - 1 = 1249
```

## 5. 定时器配置 (用于舵机 PWM)

### TIM2 配置 (或其他通用定时器)

**Mode**:
- Channel 1: PWM Generation CH1

**Configuration**:
```
Prescaler: 计算得到合适的时基
Counter Period (ARR): 计算得到 20ms (50Hz)
Counter Mode: Up
Auto-reload preload: Enable

PWM Mode: PWM mode 1
Pulse: 1500 (初始中位)
Output Compare Preload: Enable
Fast Mode: Disable
CH Polarity: High
```

**计算示例** (假设 APB1 Timer Clock = 120 MHz):
```
PWM 频率 = 50 Hz
周期 = 20 ms

方案 1:
Prescaler = 120 - 1 = 119 (1 MHz timer clock)
ARR = 20000 - 1 = 19999
此时 CCR 步进 = 1μs

脉宽范围:
0.5ms = 500
1.0ms = 1000
1.5ms = 1500 (中位)
2.0ms = 2000
2.5ms = 2500
```

**GPIO Pin**: 配置为 TIM2_CH1 (例如 PA5)

## 6. UART 配置 (用于调试,可选)

### USART1 配置

**Mode**: Asynchronous

**Configuration**:
```
Baud Rate: 115200
Word Length: 8 Bits
Stop Bits: 1
Parity: None
Mode: TX/RX
Hardware Flow Control: None
```

**GPIO Pins**:
- TX: PA9 (或对应引脚)
- RX: PA10 (或对应引脚)

## 7. GPIO 配置

### ADC 输入引脚
```
PA0_C: ADC1_INP0 (Mic 1)
PA1_C: ADC1_INP1 (Mic 2)
Mode: Analog
```

### PWM 输出引脚
```
PA5: TIM2_CH1 (Servo)
Mode: Alternate Function
Pull: No pull-up/pull-down
Speed: Low
```

### UART 引脚
```
PA9: USART1_TX
PA10: USART1_RX
Mode: Alternate Function
Pull: Pull-up
Speed: Very High
```

## 8. NVIC 配置

### 中断优先级

```
DMA1 Stream X global interrupt: Priority 5, Sub-Priority 0
TIM6 global interrupt: Priority 6, Sub-Priority 0
USART1 global interrupt: Priority 7, Sub-Priority 0
```

**注意**: 确保 ADC/DMA 中断优先级高于其他中断

## 9. 其他配置

### FPU 设置
在 Project Manager → Project → Code Generator:
```
☑ Use FPU: Hardware single precision
```

### MPU 配置
```
☑ MPU Control Mode: Background Region Privileged Access Only
```

### DCache 注意事项

如果启用 DCache,需要注意 DMA 缓冲区的 Cache 一致性:

**方案 1**: 将 DMA 缓冲区放到非 Cache 区域
在链接脚本中定义 D2 RAM section:
```ld
.dma_buffer (NOLOAD) :
{
    . = ALIGN(4);
    *(.dma_buffer)
    . = ALIGN(4);
} >RAM_D2
```

**方案 2**: 使用 Cache 管理函数
```c
// 在读取 DMA 数据前
SCB_InvalidateDCache_by_Addr((uint32_t*)buffer, size);
```

## 10. 生成代码

### Project Manager → Code Generator

```
☑ Copy all used libraries into project folder
☑ Generate peripheral initialization as a pair of '.c/.h' files per peripheral
☐ Keep User Code when re-generating (推荐取消勾选第一次生成时)
☑ Delete previously generated files when not re-generated
```

### 生成方式

1. 点击 `GENERATE CODE`
2. 选择工具链: CMake
3. 确认生成

## 验证清单

生成代码后,检查以下文件:

- [ ] `Core/Src/adc.c` - ADC 配置正确
- [ ] `Core/Src/tim.c` - 定时器配置正确 (TIM6 和 TIM2)
- [ ] `Core/Src/usart.c` - UART 配置正确 (如果使用)
- [ ] `Core/Src/main.c` - 初始化顺序正确
- [ ] `CMakeLists.txt` - 项目配置正确

## 常见配置错误

### 错误 1: ADC 采样率不对
**原因**: 定时器频率计算错误
**解决**: 重新计算 TIM6 的 Prescaler 和 ARR

### 错误 2: DMA 数据异常
**原因**: DMA 配置错误或 Cache 问题
**解决**: 检查 DMA 方向、数据宽度、循环模式;处理 Cache

### 错误 3: 舵机不工作
**原因**: PWM 频率或脉宽范围错误
**解决**: 用示波器测量 PWM 输出,确认 20ms 周期

### 错误 4: printf 无输出
**原因**: UART 未配置或未重定向
**解决**: 配置 UART 并添加 `_write` 重定向

## 参考时钟树示例

```
HSI (64 MHz)
  ↓
PLL (PLLM=4, PLLN=30, PLLP=2)
  ↓
SYSCLK (240 MHz)
  ↓
AHB (120 MHz)
  ↓
APB1 (60 MHz) → TIM2 Clock (120 MHz after doubling)
APB2 (60 MHz)
APB3 (60 MHz) → TIM6 Clock (120 MHz after doubling)

PLL2 (PLLM=4, PLLN=10, PLLP=2)
  ↓
ADC Clock (20-25 MHz)
```

## 下一步

配置完成后:
1. 生成代码
2. 添加本项目的所有模块文件
3. 修改 CMakeLists.txt 添加源文件
4. 编译测试
5. 根据 README_DOA.md 调试系统

## 技术支持

如遇到问题:
1. 检查 STM32CubeMX 生成的代码
2. 参考 STM32H7 Reference Manual
3. 使用调试器检查寄存器配置
4. 参考本项目的 README_DOA.md
