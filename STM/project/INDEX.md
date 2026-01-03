# STM32H7 双麦克风声源定位系统 - 文档索引

## 📚 快速导航

### 🚀 新手入门
1. **[QUICK_START.md](QUICK_START.md)** ⭐ **从这里开始!**
   - 5 步快速部署指南
   - 硬件连接说明
   - 第一次运行教程
   - 常见问题快速解决

### 📖 详细文档
2. **[README_DOA.md](README_DOA.md)** - 完整系统说明
   - 系统架构详解
   - 硬件要求清单
   - 参数调优指南
   - 性能指标与优化

3. **[CUBEMX_CONFIG_GUIDE.md](CUBEMX_CONFIG_GUIDE.md)** - STM32CubeMX 配置
   - 逐步配置说明
   - 时钟树配置
   - 外设参数详解
   - 验证清单

4. **[CMSIS_DSP_INTEGRATION.md](CMSIS_DSP_INTEGRATION.md)** - DSP 库集成
   - CMSIS-DSP 配置方案
   - 库文件选择指南
   - 常见集成问题

5. **[PROJECT_FILES.md](PROJECT_FILES.md)** - 项目文件清单
   - 完整文件列表
   - 模块依赖关系
   - 内存需求分析
   - 版本历史

## 🗂️ 项目结构

```
project/
├── Core/
│   ├── Inc/                    # 头文件
│   │   ├── config.h           ⭐ 系统配置参数
│   │   ├── utils.h            - 工具函数
│   │   ├── audio_capture.h    - 音频采集
│   │   ├── gcc_phat.h         - GCC-PHAT 算法
│   │   ├── servo.h            - 舵机控制
│   │   ├── app.h              - 应用主流程
│   │   └── retarget.h         - Printf 重定向
│   │
│   └── Src/                    # 源文件
│       ├── utils.c
│       ├── audio_capture.c
│       ├── gcc_phat.c
│       ├── servo.c
│       ├── app.c
│       ├── retarget.c
│       └── main.cpp           ⭐ 主程序入口
│
├── Drivers/                    # STM32 驱动
│   ├── CMSIS/                 - ARM CMSIS 库
│   └── STM32H7xx_HAL_Driver/  - STM32 HAL 库
│
├── CMakeLists.txt             ⭐ 构建配置
│
└── 文档/
    ├── QUICK_START.md         ⭐ 快速入门
    ├── README_DOA.md          - 系统说明
    ├── CUBEMX_CONFIG_GUIDE.md - CubeMX 配置
    ├── CMSIS_DSP_INTEGRATION.md - DSP 集成
    ├── PROJECT_FILES.md       - 文件清单
    └── INDEX.md               - 本文档
```

## 📋 按任务查找文档

### 🎯 我想...

#### 快速开始项目
→ **[QUICK_START.md](QUICK_START.md)**

#### 理解系统原理
→ **[README_DOA.md](README_DOA.md)** 第 1-4 章

#### 配置 STM32CubeMX
→ **[CUBEMX_CONFIG_GUIDE.md](CUBEMX_CONFIG_GUIDE.md)**

#### 解决编译错误
→ **[CMSIS_DSP_INTEGRATION.md](CMSIS_DSP_INTEGRATION.md)** 常见问题章节

#### 调整系统参数
→ **[README_DOA.md](README_DOA.md)** 参数调优章节  
→ `Core/Inc/config.h` 文件

#### 调试系统问题
→ **[QUICK_START.md](QUICK_START.md)** 调试技巧章节  
→ **[README_DOA.md](README_DOA.md)** 常见问题章节

#### 了解代码结构
→ **[PROJECT_FILES.md](PROJECT_FILES.md)**

#### 优化系统性能
→ **[README_DOA.md](README_DOA.md)** 性能优化章节

## 🔧 核心配置文件

| 文件 | 用途 | 修改频率 |
|------|------|----------|
| `Core/Inc/config.h` | 系统参数 (采样率、阈值等) | ⭐⭐⭐ 经常 |
| `CMakeLists.txt` | 编译配置 | ⭐⭐ 偶尔 |
| `Core/Src/main.cpp` | 主程序逻辑 | ⭐ 很少 |
| `.ioc` 文件 | CubeMX 配置 | ⭐ 初始设置 |

## 📊 关键算法说明

### GCC-PHAT 流程
```
音频帧 (1024 samples)
    ↓
去直流 + 汉宁窗
    ↓
FFT (2048 点)
    ↓
互功率谱 G = X1 * conj(X2)
    ↓
PHAT 加权 G /= |G|
    ↓
IFFT
    ↓
物理窗口峰值搜索 (±17 samples)
    ↓
三点抛物线插值
    ↓
可信度判决
    ↓
角度计算 θ = asin(c*Δt/d)
    ↓
指数平滑
    ↓
舵机控制
```

详见: **[README_DOA.md](README_DOA.md)** 第 7 章

## 🎓 学习路径

### 初级 (1-2 小时)
1. 阅读 [QUICK_START.md](QUICK_START.md)
2. 配置硬件并运行系统
3. 观察串口输出理解参数

### 中级 (3-5 小时)
1. 阅读 [README_DOA.md](README_DOA.md) 前 10 章
2. 理解 GCC-PHAT 算法原理
3. 调整参数适应不同环境
4. 阅读 `gcc_phat.c` 源码

### 高级 (5-10 小时)
1. 深入研究 CMSIS-DSP 使用
2. 优化性能 (降低延迟、提高精度)
3. 扩展功能 (2D 定位、多声源)
4. 修改算法实现

## 🐛 故障排除流程图

```
遇到问题?
    ↓
【是编译问题?】
    Yes → CMSIS_DSP_INTEGRATION.md 常见问题
    No  ↓
【是配置问题?】
    Yes → CUBEMX_CONFIG_GUIDE.md 验证清单
    No  ↓
【是硬件问题?】
    Yes → QUICK_START.md 硬件连接
    No  ↓
【是参数问题?】
    Yes → README_DOA.md 参数调优
    No  ↓
【其他问题】
    → QUICK_START.md 调试技巧
```

## 📞 技术支持资源

### 内部资源
- 📄 所有 .md 文档
- 💻 代码注释 (每个函数都有详细说明)
- 📊 串口调试输出

### 外部资源
- [STM32H7 参考手册](https://www.st.com/resource/en/reference_manual/rm0433-stm32h742-stm32h743753-and-stm32h750-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [CMSIS-DSP 文档](https://arm-software.github.io/CMSIS_5/DSP/html/index.html)
- [GCC-PHAT 算法论文](https://ieeexplore.ieee.org/document/1162830)
- STM32 官方论坛
- Stack Overflow

## ✅ 快速检查清单

### 开始前
- [ ] 已安装所有必需软件
- [ ] 已准备好硬件 (MCU、麦克风、舵机)
- [ ] 已阅读 [QUICK_START.md](QUICK_START.md)

### 配置阶段
- [ ] STM32CubeMX 配置完成
- [ ] 代码已生成
- [ ] 项目文件已集成
- [ ] CMakeLists.txt 已更新

### 编译阶段
- [ ] CMSIS-DSP 已集成
- [ ] 编译无错误
- [ ] 生成 .elf/.bin 文件

### 运行阶段
- [ ] 硬件连接正确
- [ ] 程序已烧录
- [ ] 串口有输出
- [ ] 舵机能响应声音

## 📈 项目状态

| 模块 | 状态 | 文档完整度 |
|------|------|-----------|
| 配置系统 | ✅ 完成 | 100% |
| 工具函数 | ✅ 完成 | 100% |
| 音频采集 | ✅ 完成 | 100% |
| GCC-PHAT | ✅ 完成 | 100% |
| 舵机控制 | ✅ 完成 | 100% |
| 应用层 | ✅ 完成 | 100% |
| 文档 | ✅ 完成 | 100% |
| 测试 | ⏳ 待硬件 | - |

## 🎯 下一步行动

1. **立即开始**: 打开 [QUICK_START.md](QUICK_START.md)
2. **深入学习**: 阅读 [README_DOA.md](README_DOA.md)
3. **实践调试**: 参考各种配置指南
4. **分享经验**: 记录你的调优结果

## 📝 版本信息

- **创建日期**: 2026-01-03
- **文档版本**: 1.0
- **项目状态**: 代码完成,待测试
- **兼容性**: STM32H7xx 系列

## 🙏 致谢

本项目基于:
- STMicroelectronics HAL 库
- ARM CMSIS-DSP 库
- GCC-PHAT 算法 (Knapp & Carter, 1976)

---

**开始你的声源定位之旅吧!** 🎵🎯🤖

如有任何问题,请参考相应文档或检查代码注释。
