# Graphics 子项目
# 渲染层，提供渲染上下文管理
cmake_minimum_required(VERSION 3.10.0)

project(Graphics VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找依赖
# ANGLE (Almost Native Graphics Layer Engine) - 用于 OpenGL ES 到 DirectX 的桥接
find_package(unofficial-angle CONFIG QUIET)

# Skia 配置
# 设置 Skia 目录路径（相对于项目根目录）
set(SKIA_DIR "${CMAKE_SOURCE_DIR}/skia" CACHE PATH "Path to Skia directory")
set(SKIA_INCLUDE_DIR "${SKIA_DIR}/include")
set(SKIA_LIB_DIR "${SKIA_DIR}/out")

# 检查 Skia 头文件是否存在
if(EXISTS "${SKIA_INCLUDE_DIR}/core/SkCanvas.h")
    set(SKIA_FOUND TRUE)
    message(STATUS "Graphics: Found Skia headers at ${SKIA_INCLUDE_DIR}")
else()
    set(SKIA_FOUND FALSE)
    message(WARNING "Graphics: Skia headers not found at ${SKIA_INCLUDE_DIR}")
endif()

# 查找 Skia 库文件（支持多种可能的命名和位置）
if(SKIA_FOUND)
    # 可能的库文件名
    set(SKIA_LIB_NAMES skia skia_shared)
    
    # 可能的库文件位置
    set(SKIA_LIB_SEARCH_PATHS
        "${SKIA_DIR}/out/Release"
        "${SKIA_DIR}/out/Debug"
        "${SKIA_DIR}/out/Release-x64"
        "${SKIA_DIR}/out/Debug-x64"
        "${SKIA_DIR}/out/Release-x86"
        "${SKIA_DIR}/out/Debug-x86"
        "${SKIA_DIR}/lib"
        "${SKIA_DIR}/out/${CMAKE_BUILD_TYPE}"
        "${SKIA_DIR}/out/${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_PROCESSOR}"
    )
    
    # 查找静态库
    find_library(SKIA_LIB
        NAMES ${SKIA_LIB_NAMES}
        PATHS ${SKIA_LIB_SEARCH_PATHS}
        PATH_SUFFIXES lib lib/${CMAKE_SYSTEM_PROCESSOR}
        NO_DEFAULT_PATH
    )
    
    if(SKIA_LIB)
        message(STATUS "Graphics: Found Skia library at ${SKIA_LIB}")
    else()
        message(STATUS "Graphics: Skia library not found, will use header-only or link manually")
    endif()
endif()

# Graphics 库（静态库，供其他模块链接）
add_library(Graphics STATIC
    src/RenderContext.cpp
)

# 公共头文件目录
target_include_directories(Graphics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/graphics>
)

# 添加 Skia 头文件目录
if(SKIA_FOUND)
    target_include_directories(Graphics PUBLIC
        $<BUILD_INTERFACE:${SKIA_INCLUDE_DIR}>
    )
endif()

# 链接 Foundation 库
target_link_libraries(Graphics PUBLIC
    Foundation
)

# 如果找到 ANGLE，链接它
if(unofficial-angle_FOUND)
    target_link_libraries(Graphics PUBLIC unofficial::angle::libEGL unofficial::angle::libGLESv2)
    target_compile_definitions(Graphics PUBLIC GRAPHICS_USE_ANGLE)
    message(STATUS "Graphics: Using ANGLE for graphics context")
else()
    message(STATUS "Graphics: ANGLE not found, using native graphics APIs")
endif()

# 如果找到 Skia，链接它
if(SKIA_FOUND)
    if(SKIA_LIB)
        target_link_libraries(Graphics PUBLIC ${SKIA_LIB})
        message(STATUS "Graphics: Linking Skia library")
    else()
        # 如果没有找到预编译库，可以手动指定库路径
        # 或者使用 header-only 模式（如果 Skia 支持）
        message(STATUS "Graphics: Skia headers found but library not found, please link manually")
    endif()
    target_compile_definitions(Graphics PUBLIC GRAPHICS_USE_SKIA)
    message(STATUS "Graphics: Using Skia for rendering")
endif()

# 编译定义
target_compile_definitions(Graphics PRIVATE
    GRAPHICS_VERSION_MAJOR=${Graphics_VERSION_MAJOR}
    GRAPHICS_VERSION_MINOR=${Graphics_VERSION_MINOR}
    GRAPHICS_VERSION_PATCH=${Graphics_VERSION_PATCH}
)

# ==================== 安装配置 ====================
# 安装 Graphics 库
install(TARGETS Graphics
    EXPORT GraphicsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/graphics
)

# 安装头文件到 SDK/include/graphics
install(DIRECTORY include/
    DESTINATION include/graphics
    FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)

# 安装导出文件（供其他项目使用）
install(EXPORT GraphicsTargets
    FILE GraphicsTargets.cmake
    NAMESPACE Graphics::
    DESTINATION lib/cmake/Graphics
)

# 创建配置文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/GraphicsConfigVersion.cmake"
    VERSION ${Graphics_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/GraphicsConfigVersion.cmake"
    DESTINATION lib/cmake/Graphics
)

