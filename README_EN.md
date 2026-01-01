# KIUI

A Modern Cross-Platform UI Components Library

## Project Overview

KIUI aims to build a high-performance, cross-platform modern UI components library. The project adopts a Monorepo architecture, providing foundational libraries and component systems that support Windows, macOS, and Linux platforms.

## Technology Stack

### Core Dependencies

| Component | vcpkg Package |
|-----------|--------------|
| **Window Management** | `glfw3` |
| **Graphics API** | `angle` |
| **Layout Engine** | `yoga` | 
| **Font Loading** | `freetype` | 
| **Text Shaping** | `harfbuzz` |
| **Image Loading** | `stb` | 

### Build Tools

- **CMake** (>= 3.10.0) - Cross-platform build system
- **vcpkg** - C++ package manager
- **C++20** - Modern C++ standard

### Testing Framework

- **Google Test (GTest)** - Unit testing framework

## Project Structure

```
WindowsAPI/
├── foundation/          # Foundation base library
│   ├── src/            # Source code
│   │   ├── hpps/       # Header files
│   │   └── *.cpp       # Implementation files
│   ├── tests/          # Unit tests
│   └── CMakeLists.txt  # Build configuration
├── graphics/           # Graphics rendering layer
│   ├── src/            # Source code
│   ├── include/        # Header files
│   └── CMakeLists.txt  # Build configuration
├── widget/             # Widget UI component library
│   ├── src/            # Source code
│   ├── include/        # Header files
│   ├── examples/       # Example code
│   ├── tests/          # Unit tests
│   └── CMakeLists.txt  # Build configuration
├── SDK/                # Installation directory (generated after build)
│   ├── include/        # Public headers
│   └── lib/            # Static library files
├── CMakeLists.txt      # Root CMake configuration
├── CMakePresets.json   # CMake preset configuration
└── build.ps1           # PowerShell build script
```

## Quick Start

### Prerequisites

1. **Install vcpkg**
   ```powershell
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

2. **Set Environment Variable**
   ```powershell
   $env:VCPKG_ROOT = "E:\vcpkg\vcpkg"  # Replace with your vcpkg path
   ```

3. **Install Dependencies**
   ```powershell
   vcpkg install glfw3 angle yoga freetype harfbuzz stb --triplet x64-windows
   ```

### Building the Project

#### Windows (PowerShell)

```powershell
# Debug build
.\build.ps1 -Debug

# Release build
.\build.ps1 -Release

# Clean and build Release version
.\build.ps1 -Release -Clean

# Build and install to SDK directory
.\build.ps1 -Release -Install
```

#### Using CMake Directly

```powershell
# Configure
cmake --preset classic-vcpkg

# Build
cmake --build out/build/classic-vcpkg --config Release

# Install
cmake --install out/build/classic-vcpkg --config Release
```

### Running Tests

```powershell
# Foundation tests
cd out/build/classic-vcpkg/foundation/Release
.\FoundationTests.exe

# Widget tests
cd out/build/classic-vcpkg/widget/Release
.\WidgetTests.exe
```

## Foundation Base Library

Foundation is the core base library of KIUI, providing the following features:

- ✅ **Window Management** - Multi-window management based on GLFW
- ✅ **Graphics Context** - ANGLE integration (OpenGL ES 3.0)
- 🔄 **Swapchain Management** - Render target management (Planned)
- 🔄 **Input Service** - Standardized input handling (Planned)
- 🔄 **High-DPI Support** - DPI awareness and high-resolution adaptation (Planned)
- 🔄 **Environment Awareness** - Platform feature detection (Planned)

### Usage Example

```cpp
#include <foundation/window.hpp>

using namespace KiUI::foundation;

// Get WindowManager singleton
auto& wm = WindowManager::GetSharedInstance();

// Initialize platform subsystems
wm.InitializePlatformSubsystems();

// Create window
auto window = wm.CreateNativeWindow("My Window", 800, 600);

// Enter main message loop
wm.EnterMainMessageLoop();

// Cleanup
wm.ShutdownPlatformSubsystems();
```

## Widget UI Component Library

Widget is the UI component library of KIUI, built on top of Yoga layout engine and Skia graphics library

### Core Features

#### High-Performance Hit Testing
- Uses virtual function `AsVisualElement()` to avoid expensive `dynamic_cast`
- Supports coordinate transformation with transform matrices (rotation, scaling)
- Supports recursive search for multi-level nested elements
- Supports layer handling for overlapping elements (later drawn on top)

#### Flexible Layout System
- Flexbox layout based on Yoga
- Supports style properties like Margin, Padding, Border
- Supports alignment and distribution configuration

### Usage Example

```cpp
#include <widget/Box.hpp>
#include <widget/SceneRenderer.hpp>

using namespace KiUI::widget;

// Create root container
auto root = boost::make_shared<Box>();
root->SetWidth(800.0f);
root->SetHeight(600.0f);

// Create child container
auto child = boost::make_shared<Box>();
child->SetLeft(100.0f);
child->SetTop(100.0f);
child->SetWidth(200.0f);
child->SetHeight(200.0f);
child->SetBackgroundColor(SK_ColorBLUE);
root->AddChild(child);

// Calculate layout
root->CalculateLayout(800.0f, 600.0f);

// Hit testing
auto hit = root->HitTest(150.0f, 150.0f);
if (hit) {
    // Handle click event
}
```

## Development Roadmap

### Foundation Base Library
- [x] Foundation base library architecture
- [x] WindowManager window management
- [x] GLFW integration
- [ ] ANGLE graphics context initialization
- [ ] Swapchain management
- [ ] Standardized input service
- [ ] High-DPI support

### Graphics Rendering Layer
- [x] Graphics rendering layer architecture
- [x] Skia graphics library integration
- [x] Render context management
- [x] Basic shape drawing (rectangle, rounded rectangle)

### Widget UI Component Library
- [x] Widget component library architecture
- [x] Yoga layout engine integration
- [x] VisualElement base component
- [x] Box container component
- [x] Recursive hit testing system
- [x] Transform matrix support (rotation, scaling)
- [x] Scene renderer
- [x] Event routing system
- [x] Unit testing framework integration
- [ ] More basic components (Text, Image, etc.)
- [ ] Style system
- [ ] Animation system

### Text and Image
- [ ] Freetype/HarfBuzz text rendering
- [ ] STB image loading

## Related Links

- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [ANGLE Project](https://github.com/google/angle)
- [Yoga Layout](https://yogalayout.com/)
- [FreeType](https://www.freetype.org/)
- [HarfBuzz](https://harfbuzz.github.io/)
- [stb](https://github.com/nothings/stb)

