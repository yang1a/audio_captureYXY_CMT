# CMSIS-DSP 集成指南

本项目需要 CMSIS-DSP 库来执行 FFT 运算。以下是集成步骤。

## 方案 1: 使用预编译库 (推荐)

### 步骤

1. **确认库文件存在**

检查路径:
```
Drivers/CMSIS/Lib/GCC/
```

应包含:
- `libarm_cortexM7lfdp_math.a` (单精度浮点,硬件 FPU)

2. **修改 CMakeLists.txt**

在主 `CMakeLists.txt` 中添加:

```cmake
# 添加 CMSIS-DSP 包含路径
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include
)

# 链接预编译库
target_link_libraries(${CMAKE_PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Lib/GCC/libarm_cortexM7lfdp_math.a
)

# 添加 ARM_MATH_CM7 宏定义
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ARM_MATH_CM7
    ARM_MATH_MATRIX_CHECK
    ARM_MATH_ROUNDING
)
```

3. **编译选项**

确保在 `gcc-arm-none-eabi.cmake` 或 CMakeLists.txt 中有:

```cmake
set(FPU_FLAGS "-mfpu=fpv5-d16 -mfloat-abi=hard")
set(CPU_FLAGS "-mcpu=cortex-m7 ${FPU_FLAGS}")
```

## 方案 2: 从源码编译 (完全控制)

### 步骤

1. **添加需要的源文件**

在 CMakeLists.txt 中:

```cmake
# CMSIS-DSP 源文件
set(CMSIS_DSP_SRC
    # FFT Functions
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_rfft_fast_f32.c
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_cfft_f32.c
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_rfft_fast_init_f32.c
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_cfft_init_f32.c
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix8_f32.c
    
    # Common Tables
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/CommonTables/arm_common_tables.c
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/CommonTables/arm_const_structs.c
    
    # Support Functions (if needed)
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/SupportFunctions/arm_copy_f32.c
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Source/SupportFunctions/arm_fill_f32.c
)

# 添加到项目
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${CMSIS_DSP_SRC})

# 添加包含路径
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/PrivateInclude
)

# 添加宏定义
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ARM_MATH_CM7
)
```

2. **优点**
- 只编译需要的函数
- 可以调试 DSP 库内部
- 减小最终二进制大小

3. **缺点**
- 编译时间稍长
- 需要手动管理依赖

## 验证 CMSIS-DSP 集成

### 测试代码

在 `main.cpp` 的 `USER CODE BEGIN 2` 中添加:

```cpp
#include "arm_math.h"

// 测试 FFT 初始化
arm_rfft_fast_instance_f32 test_fft;
arm_status status = arm_rfft_fast_init_f32(&test_fft, 1024);

if (status == ARM_MATH_SUCCESS)
{
    printf("CMSIS-DSP FFT init successful!\r\n");
}
else
{
    printf("CMSIS-DSP FFT init failed!\r\n");
}
```

### 编译测试

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

如果编译通过且测试输出成功,说明 CMSIS-DSP 已正确集成。

## 常见问题

### Q1: "undefined reference to 'arm_rfft_fast_f32'"

**原因**: CMSIS-DSP 库未正确链接

**解决**:
1. 检查库路径是否正确
2. 确认 `target_link_libraries` 有库文件
3. 确认编译选项有 FPU 支持

### Q2: FFT 结果异常

**原因**: 数据对齐或 FPU 配置问题

**解决**:
1. 确保缓冲区对齐: `__attribute__((aligned(4)))`
2. 检查 FPU 启用: `-mfpu=fpv5-d16 -mfloat-abi=hard`
3. 确认定义了 `ARM_MATH_CM7`

### Q3: 编译时出现 "arm_math.h: No such file"

**原因**: 包含路径未设置

**解决**:
在 CMakeLists.txt 添加:
```cmake
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include
)
```

### Q4: 链接时库文件找不到

**原因**: 库路径错误或库文件不存在

**解决**:
1. 检查库文件是否存在于 `Drivers/CMSIS/Lib/GCC/`
2. 如果没有,需要从 ARM CMSIS-DSP 仓库获取或从源码编译
3. 确认库文件名正确 (M7 单精度硬件 FPU 应该是 `libarm_cortexM7lfdp_math.a`)

## CMSIS-DSP 版本

本项目兼容 CMSIS-DSP 1.10.0 及以上版本。

ARM 官方仓库:
https://github.com/ARM-software/CMSIS-DSP

## 性能优化建议

### 编译优化

```cmake
# Release 模式下使用 -O3
set(CMAKE_C_FLAGS_RELEASE "-O3 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")

# 启用链接时优化 (LTO)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
```

### 内存对齐

确保所有 FFT 输入输出缓冲区对齐:

```c
static float32_t input[FFT_SIZE] __attribute__((aligned(8)));
static float32_t output[FFT_SIZE * 2] __attribute__((aligned(8)));
```

### Cache 优化 (STM32H7)

如果启用 DCache:

```c
// 在使用 FFT 数据前
SCB_CleanDCache_by_Addr((uint32_t*)input, sizeof(input));

// 在 FFT 完成后
SCB_InvalidateDCache_by_Addr((uint32_t*)output, sizeof(output));
```

## 库文件选择参考

根据 MCU 和 FPU 配置选择合适的库:

| MCU Core | FPU | 库文件 |
|----------|-----|--------|
| Cortex-M7 | 单精度硬件 | libarm_cortexM7lfdp_math.a |
| Cortex-M7 | 双精度硬件 | libarm_cortexM7lfsp_math.a |
| Cortex-M7 | 软浮点 | libarm_cortexM7l_math.a |
| Cortex-M4 | 单精度硬件 | libarm_cortexM4lf_math.a |
| Cortex-M4 | 软浮点 | libarm_cortexM4l_math.a |

**本项目使用**: `libarm_cortexM7lfdp_math.a` (Cortex-M7 + 单精度硬件 FPU)

## 完整 CMakeLists.txt 示例

```cmake
cmake_minimum_required(VERSION 3.22)

# ... 其他配置 ...

# CMSIS-DSP 配置
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    ARM_MATH_CM7
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Lib/GCC/libarm_cortexM7lfdp_math.a
)

# FPU 编译选项
target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
    -mfpu=fpv5-d16
    -mfloat-abi=hard
)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -mfpu=fpv5-d16
    -mfloat-abi=hard
)
```

## 下载 CMSIS-DSP 库

如果项目中缺少 CMSIS-DSP 库文件:

1. **下载完整 CMSIS Pack**
   - 访问: https://github.com/ARM-software/CMSIS_5
   - 下载最新 release
   - 解压并复制 `CMSIS/DSP` 到项目 `Drivers/CMSIS/` 目录

2. **使用 STM32CubeMX**
   - 在 STM32CubeMX 项目中
   - Additional Software → 选择 CMSIS DSP
   - 重新生成代码

## 总结

CMSIS-DSP 是本项目的核心依赖,务必正确集成。推荐使用预编译库以简化集成过程。
