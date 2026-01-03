# 项目文件清单

## 核心模块文件

### 配置文件
- ✅ `Core/Inc/config.h` - 系统配置参数 (采样率、FFT长度、麦克风参数等)

### 工具函数模块
- ✅ `Core/Inc/utils.h` - 工具函数声明
- ✅ `Core/Src/utils.c` - 工具函数实现 (窗函数、均值、clamp等)

### 音频采集模块
- ✅ `Core/Inc/audio_capture.h` - 音频采集接口
- ✅ `Core/Src/audio_capture.c` - ADC+DMA 双通道采集实现

### GCC-PHAT 算法模块
- ✅ `Core/Inc/gcc_phat.h` - GCC-PHAT 算法接口
- ✅ `Core/Src/gcc_phat.c` - GCC-PHAT 完整实现

### 舵机控制模块
- ✅ `Core/Inc/servo.h` - 舵机控制接口
- ✅ `Core/Src/servo.c` - PWM 舵机控制实现

### 应用层模块
- ✅ `Core/Inc/app.h` - 应用主流程接口
- ✅ `Core/Src/app.c` - 主流程实现 (帧处理、角度平滑、舵机更新)

### 辅助模块
- ✅ `Core/Inc/retarget.h` - Printf 重定向接口
- ✅ `Core/Src/retarget.c` - Printf 到 UART 重定向实现

### 主程序
- ✅ `Core/Src/main.cpp` - 主程序 (已更新集成所有模块)

### 构建配置
- ✅ `CMakeLists.txt` - 已更新包含所有新增源文件

## 文档文件

- ✅ `README_DOA.md` - 完整系统说明文档
- ✅ `CUBEMX_CONFIG_GUIDE.md` - STM32CubeMX 配置详细步骤
- ✅ `CMSIS_DSP_INTEGRATION.md` - CMSIS-DSP 库集成指南
- ✅ `PROJECT_FILES.md` - 本文件清单

## 项目状态

### ✅ 已完成
1. 所有核心模块代码实现
2. 完整的 GCC-PHAT 算法 (含物理窗口、可信度判决)
3. ADC+DMA 双缓冲采集
4. 舵机 PWM 控制
5. 角度平滑滤波
6. CMakeLists.txt 配置
7. 完整文档

### ⚠️ 需要用户配置

以下项需要在 STM32CubeMX 中配置:

1. **ADC 配置**
   - 2通道扫描模式
   - 定时器触发
   - DMA circular mode
   - 参考: `CUBEMX_CONFIG_GUIDE.md`

2. **定时器配置 (TIM6 - ADC 触发)**
   - 配置为 48kHz 触发频率
   - TRGO: Update Event

3. **定时器配置 (TIM2 - 舵机 PWM)**
   - PWM 输出 50Hz
   - Channel 1 输出

4. **UART 配置 (可选 - 调试输出)**
   - 波特率 115200
   - 用于 printf 输出

5. **更新 main.cpp**
   - 生成代码后,取消注释 UART 初始化
   - 取消注释定时器初始化
   - 更新 App_Init 调用

### 📝 待完成步骤

用户需要:

1. 在 STM32CubeMX 中按照 `CUBEMX_CONFIG_GUIDE.md` 配置外设
2. 生成代码
3. 集成 CMSIS-DSP 库 (按照 `CMSIS_DSP_INTEGRATION.md`)
4. 在 main.cpp 中启用 UART 和 Timer (取消相关注释)
5. 编译项目
6. 下载到硬件
7. 调试和参数调优

## 代码统计

| 模块 | 文件 | 代码行数 (估算) |
|------|------|----------------|
| config.h | 1 | ~70 |
| utils | 2 | ~150 |
| audio_capture | 2 | ~200 |
| gcc_phat | 2 | ~300 |
| servo | 2 | ~120 |
| app | 2 | ~150 |
| retarget | 2 | ~50 |
| **总计** | **13** | **~1040** |

## 依赖关系

```
main.cpp
  ↓
app.c
  ├→ audio_capture.c
  ├→ gcc_phat.c
  │   └→ utils.c
  └→ servo.c
       └→ utils.c

retarget.c (独立)
```

## 内存需求估算

### Flash (代码)
- 用户代码: ~20 KB
- CMSIS-DSP: ~50 KB
- HAL库: ~100 KB
- **总计**: ~170 KB

### RAM (数据)
- GCC-PHAT 缓冲区: ~48 KB (FFT_L=2048, 多个float32数组)
- Audio 缓冲区: ~8 KB (双缓冲)
- 栈: ~8 KB
- 其他: ~4 KB
- **总计**: ~68 KB

**STM32H743 (512KB Flash, 512KB RAM) 完全满足需求**

## 编译命令

```bash
# 清理
rm -rf build

# 配置
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build .

# 输出文件
# build/project.elf
# build/project.bin
# build/project.hex
```

## 调试建议

### 1. 串口调试
- 波特率: 115200
- 输出信息包含: lag, dt, theta, peak, ratio, valid

### 2. 示波器
- 检查 ADC 采样信号
- 检查 PWM 输出 (20ms 周期)

### 3. 逻辑分析仪
- 监测 DMA 传输
- 验证采样率

## 测试计划

### 单元测试
1. ✅ utils 函数 (窗函数、均值)
2. ⚠️ audio_capture (需硬件)
3. ⚠️ gcc_phat (可用仿真数据测试)
4. ⚠️ servo (需硬件)

### 集成测试
1. ⚠️ ADC + DMA 数据流
2. ⚠️ 完整 DOA 流程
3. ⚠️ 舵机响应

### 系统测试
1. ⚠️ 单声源定位精度
2. ⚠️ 响应时间
3. ⚠️ 抗噪声性能
4. ⚠️ 回声鲁棒性

## 版本历史

### v1.0 (2026-01-03)
- ✅ 初始版本
- ✅ 所有核心模块实现
- ✅ 完整文档

## 许可证

本项目基于 STM32 HAL 库,遵循 ST 软件许可协议。
用户代码部分可自由使用和修改。

## 技术支持

遇到问题请:
1. 查阅 `README_DOA.md` 常见问题章节
2. 检查 `CUBEMX_CONFIG_GUIDE.md` 配置是否正确
3. 使用调试器检查变量值
4. 参考 STM32H7 参考手册

## 贡献

欢迎提交改进建议:
- 算法优化
- 文档完善
- Bug 修复
- 新功能添加

## 相关资源

- [STM32H7 参考手册](https://www.st.com/resource/en/reference_manual/rm0433-stm32h742-stm32h743753-and-stm32h750-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [CMSIS-DSP 文档](https://arm-software.github.io/CMSIS_5/DSP/html/index.html)
- [GCC-PHAT 论文](https://ieeexplore.ieee.org/document/1162830)

---

**项目创建日期**: 2026-01-03  
**最后更新**: 2026-01-03  
**状态**: 代码完成,等待硬件测试
