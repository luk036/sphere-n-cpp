# iFlow 项目上下文 - sphere-n-cpp

## 项目概述

`sphere-n-cpp` 是一个现代 C++ 库，用于生成 n 维球面上的低差异序列 (Low Discrepancy Sequences)。该库的主要目的是在不同维度的球面上生成均匀分布的点，这在计算机图形学、数值积分和蒙特卡洛模拟等领域非常有用。

该项目基于 ModernCppStarter 模板构建，采用 C++17 标准（实际使用 C++20），利用 CPM.cmake 进行依赖管理，并包含完整的测试套件、文档生成、代码格式化和持续集成支持。

## 核心技术与架构

- **C++17/20**: 使用现代 C++ 特性
- **CMake**: 构建系统，遵循现代 CMake 最佳实践
- **CPM.cmake**: 依赖管理
- **doctest**: 单元测试框架
- **fmt**: 格式化库
- **Variant/Unique_ptr**: 使用 C++17 的 `std::variant` 和 `std::unique_ptr` 实现递归结构

### 主要组件

1. **VdCorput 序列生成器**: 生成 [0,1] 之间的均匀分布数字
2. **Sphere3 类**: 生成 3 维球面上的点（特殊处理 3 维情况）
3. **SphereN 类**: 生成 n 维球面上的点（递归结构）
4. **CylindN 类**: 使用柱面坐标方法生成点
5. **递归结构**: 通过 `Sphere3` 和 `NSphere` 从低维构建高维实体

## 构建和运行

### 构建独立可执行文件

```bash
cmake -S. -B build
cmake --build build
./build/standalone/SphereN --help
```

### 运行测试套件

```bash
cmake -S. -B build
cmake --build build
cd build/test
CTEST_OUTPUT_ON_FAILURE=1 ctest

# 或者直接运行测试可执行文件
./build/test/SphereNTests
```

启用代码覆盖率：
```bash
cmake -S. -B build -DENABLE_TEST_COVERAGE=1
```

### 代码格式化

```bash
cmake -S . -B build/test

# 查看格式化更改
cmake --build build --target format

# 应用格式化更改
cmake --build build --target fix-format
```

### 构建文档

```bash
cmake -S . -B build
cmake --build build --target GenerateDocs
open build/documentation/doxygen/html/index.html
```

## 开发约定

- 使用 clang-format 和 cmake-format 进行代码格式化
- 使用 doctest 框架编写单元测试
- 遵循现代 CMake 实践
- 使用 C++20 标准特性
- 通过 GitHub Actions 实现持续集成
- 通过 codecov 进行代码覆盖率分析

## 项目结构

- `include/sphere_n/`: 头文件 (sphere_n.hpp, cylind_n.hpp)
- `source/`: 源文件实现
- `test/`: 测试代码
- `standalone/`: 独立可执行文件示例
- `cmake/`: CMake 模块和工具
- `documentation/`: 文档配置

## 依赖项

- `ldsgen/lds.hpp`: 依赖于 `ldsgen` 库提供基础的 VdCorput 和 Sphere 类
- `doctest`: 用于单元测试
- `fmt`: 用于格式化
- `CPM.cmake`: 用于依赖管理

## 功能特点

- 支持 N 维球面 (SphereN)
- 现代 CMake 实践
- 集成测试套件
- 持续集成支持
- 代码格式化和静态分析工具支持
- 可安装的目标库
- 自动生成文档