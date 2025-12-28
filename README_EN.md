# KIUI

A Modern Cross-Platform UI Components Library

## Project Overview

KIUI aims to build a high-performance, cross-platform modern UI components library. The project adopts a Monorepo architecture, providing foundational libraries and component systems that support Windows, macOS, and Linux platforms.

## Technology Stack

### Core Dependencies

| Component | vcpkg Package | Cross-Platform Support | Notes |
|-----------|--------------|------------------------|-------|
| **Window Management** | `glfw3` | ⭐⭐⭐⭐⭐ Excellent | The preferred choice for modern OpenGL/GLES window creation |
| **Graphics API** | `angle` | ⭐⭐⭐⭐⭐ Excellent | Provides GLES 3.0 support on Windows |
| **Layout Engine** | `yoga` | ⭐⭐⭐⭐⭐ Excellent | Facebook open-source, mature C++ bindings |
| **Font Loading** | `freetype` | ⭐⭐⭐⭐⭐ Excellent | The underlying font parser used by almost all UI libraries |
| **Text Shaping** | `harfbuzz` | ⭐⭐⭐⭐⭐ Excellent | Handles character spacing, ligatures, and complex scripts |
| **Image Loading** | `stb` | ⭐⭐⭐⭐⭐ Excellent | Lightweight single-header library, recommended over heavyweight libpng/jpeg |

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
cd out/build/classic-vcpkg/foundation/Release
.\FoundationTests.exe
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

## Development Roadmap

- [x] Foundation base library architecture
- [x] WindowManager window management
- [x] GLFW integration
- [ ] ANGLE graphics context initialization
- [ ] Swapchain management
- [ ] Standardized input service
- [ ] High-DPI support
- [ ] Yoga layout engine integration
- [ ] Freetype/HarfBuzz text rendering
- [ ] STB image loading
- [ ] UI component system

## License

[To be determined]

## Contributing

Issues and Pull Requests are welcome!

## Related Links

- [GLFW Documentation](https://www.glfw.org/documentation.html)
- [ANGLE Project](https://github.com/google/angle)
- [Yoga Layout](https://yogalayout.com/)
- [FreeType](https://www.freetype.org/)
- [HarfBuzz](https://harfbuzz.github.io/)
- [stb](https://github.com/nothings/stb)

