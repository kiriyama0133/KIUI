# KIUI

一个现代化的跨平台 UI 组件库

## 项目简介

KIUI 旨在构建一个高性能、跨平台的现代化 UI 组件库。项目采用 Monorepo 架构，提供基础库和组件系统，支持 Windows、macOS 和 Linux 平台。

## 技术栈

### 核心依赖

| 组件 | vcpkg 包名 |
|------|-----------|
| **窗口管理** | `glfw3` |
| **图形 API** | `angle` | 
| **布局引擎** | `yoga` | 
| **字体加载** | `freetype` | 
| **文字排版** | `harfbuzz` |
| **图片加载** | `stb` | 

### 构建工具

- **CMake** (>= 3.10.0) - 跨平台构建系统
- **vcpkg** - C++ 包管理器
- **C++20** - 现代 C++ 标准

### 测试框架

- **Google Test (GTest)** - 单元测试框架

## 项目结构

```
WindowsAPI/
├── foundation/          # Foundation 基础库
│   ├── src/            # 源代码
│   │   ├── hpps/       # 头文件
│   │   └── *.cpp       # 实现文件
│   ├── tests/          # 单元测试
│   └── CMakeLists.txt  # 构建配置
├── SDK/                # 安装目录（构建后生成）
│   ├── include/        # 公共头文件
│   └── lib/            # 静态库文件
├── CMakeLists.txt      # 根 CMake 配置
├── CMakePresets.json   # CMake 预设配置
└── build.ps1           # PowerShell 构建脚本
```

## 快速开始

### 前置要求

1. **安装 vcpkg**
   ```powershell
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

2. **设置环境变量**
   ```powershell
   $env:VCPKG_ROOT = "E:\vcpkg\vcpkg"  # 替换为你的 vcpkg 路径
   ```

3. **安装依赖**
   ```powershell
   vcpkg install glfw3 angle yoga freetype harfbuzz stb --triplet x64-windows
   ```

### 构建项目

#### Windows (PowerShell)

```powershell
# Debug 版本
.\build.ps1 -Debug

# Release 版本
.\build.ps1 -Release

# 清理并构建 Release 版本
.\build.ps1 -Release -Clean

# 构建并安装到 SDK 目录
.\build.ps1 -Release -Install
```

#### 使用 CMake 直接构建

```powershell
# 配置
cmake --preset classic-vcpkg

# 构建
cmake --build out/build/classic-vcpkg --config Release

# 安装
cmake --install out/build/classic-vcpkg --config Release
```

### 运行测试

```powershell
cd out/build/classic-vcpkg/foundation/Release
.\FoundationTests.exe
```

## Foundation 基础库

Foundation 是 KIUI 的核心基础库，提供以下功能：

- ✅ **窗口管理** - 基于 GLFW 的多窗口管理
- ✅ **图形上下文** - ANGLE 集成（OpenGL ES 3.0）
- 🔄 **交换链管理** - 渲染目标管理（规划中）
- 🔄 **输入服务** - 标准化输入处理（规划中）
- 🔄 **高分屏支持** - DPI 感知和高分辨率适配（规划中）
- 🔄 **环境感知** - 平台特性检测（规划中）

### 使用示例

```cpp
#include <foundation/window.hpp>

using namespace KiUI::foundation;

// 获取 WindowManager 单例
auto& wm = WindowManager::GetSharedInstance();

// 初始化平台子系统
wm.InitializePlatformSubsystems();

// 创建窗口
auto window = wm.CreateNativeWindow("My Window", 800, 600);

// 进入主消息循环
wm.EnterMainMessageLoop();

// 清理
wm.ShutdownPlatformSubsystems();
```

## 开发计划

- [x] Foundation 基础库架构
- [x] WindowManager 窗口管理
- [x] GLFW 集成
- [ ] ANGLE 图形上下文初始化
- [ ] 交换链管理
- [ ] 输入服务标准化
- [ ] 高分屏支持
- [ ] Yoga 布局引擎集成
- [ ] Freetype/HarfBuzz 文字渲染
- [ ] STB 图片加载
- [ ] UI 组件系统

## 相关链接

- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [ANGLE Project](https://github.com/google/angle)
- [Yoga Layout](https://yogalayout.com/)
- [FreeType](https://www.freetype.org/)
- [HarfBuzz](https://harfbuzz.github.io/)
- [stb](https://github.com/nothings/stb)
