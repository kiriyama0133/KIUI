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
├── graphics/           # Graphics 渲染层
│   ├── src/            # 源代码
│   ├── include/        # 头文件
│   └── CMakeLists.txt  # 构建配置
├── widget/             # Widget UI 组件库
│   ├── src/            # 源代码
│   ├── include/        # 头文件
│   ├── examples/       # 示例代码
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
# Foundation 测试
cd out/build/classic-vcpkg/foundation/Release
.\FoundationTests.exe

# Widget 测试
cd out/build/classic-vcpkg/widget/Release
.\WidgetTests.exe
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

## Widget UI 组件库

Widget 是 KIUI 的 UI 组件库，基于 Yoga 布局引擎和 Skia 图形库构建


### 核心特性

#### 高性能命中测试
- 使用虚函数 `AsVisualElement()` 避免昂贵的 `dynamic_cast`
- 支持变换矩阵（旋转、缩放）的坐标转换
- 支持多层嵌套元素的递归查找
- 支持重叠元素的层级处理（后画的在上层）

#### 灵活的布局系统
- 基于 Yoga 的 Flexbox 布局
- 支持 Margin、Padding、Border 等样式属性
- 支持对齐和分布方式配置

### 使用示例

```cpp
#include <widget/Box.hpp>
#include <widget/SceneRenderer.hpp>

using namespace KiUI::widget;

// 创建根容器
auto root = boost::make_shared<Box>();
root->SetWidth(800.0f);
root->SetHeight(600.0f);

// 创建子容器
auto child = boost::make_shared<Box>();
child->SetLeft(100.0f);
child->SetTop(100.0f);
child->SetWidth(200.0f);
child->SetHeight(200.0f);
child->SetBackgroundColor(SK_ColorBLUE);
root->AddChild(child);

// 计算布局
root->CalculateLayout(800.0f, 600.0f);

// 命中测试
auto hit = root->HitTest(150.0f, 150.0f);
if (hit) {
    // 处理点击事件
}
```

## 开发计划

### Foundation 基础库
- [x] Foundation 基础库架构
- [x] WindowManager 窗口管理
- [x] GLFW 集成
- [ ] ANGLE 图形上下文初始化
- [ ] 交换链管理
- [ ] 输入服务标准化
- [ ] 高分屏支持

### Graphics 渲染层
- [x] Graphics 渲染层架构
- [x] Skia 图形库集成
- [x] 渲染上下文管理
- [x] 基础图形绘制（矩形、圆角矩形）

### Widget UI 组件库
- [x] Widget 组件库架构
- [x] Yoga 布局引擎集成
- [x] VisualElement 基础组件
- [x] Box 容器组件
- [x] 递归命中测试系统
- [x] 变换矩阵支持（旋转、缩放）
- [x] 场景渲染器
- [x] 事件路由系统
- [x] 单元测试框架集成
- [ ] 更多基础组件（Text、Image 等）
- [ ] 样式系统
- [ ] 动画系统

### 文字和图片
- [ ] Freetype/HarfBuzz 文字渲染
- [ ] STB 图片加载

## 相关链接

- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [ANGLE Project](https://github.com/google/angle)
- [Yoga Layout](https://yogalayout.com/)
- [FreeType](https://www.freetype.org/)
- [HarfBuzz](https://harfbuzz.github.io/)
- [stb](https://github.com/nothings/stb)
