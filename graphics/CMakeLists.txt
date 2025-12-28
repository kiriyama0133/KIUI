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

# 检查 Skia 头文件是否存在
if(EXISTS "${SKIA_INCLUDE_DIR}/core/SkCanvas.h")
    set(SKIA_FOUND TRUE)
    message(STATUS "Graphics: Found Skia headers at ${SKIA_INCLUDE_DIR}")
else()
    set(SKIA_FOUND FALSE)
    message(WARNING "Graphics: Skia headers not found at ${SKIA_INCLUDE_DIR}")
endif()

# 查找 Skia 库文件（支持多种构建配置）
if(SKIA_FOUND)
    # 根据构建类型和架构确定库文件路径
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SKIA_ARCH_SUFFIX "x64")
    else()
        set(SKIA_ARCH_SUFFIX "x86")
    endif()
    
    # 根据构建类型确定目录
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_CONFIGURATION_TYPES)
        set(SKIA_BUILD_DIR "${SKIA_DIR}/out/Release-${SKIA_ARCH_SUFFIX}")
    else()
        set(SKIA_BUILD_DIR "${SKIA_DIR}/out/Debug-${SKIA_ARCH_SUFFIX}")
    endif()
    
    # 主要的 Skia 库文件
    set(SKIA_LIB_PATH "${SKIA_BUILD_DIR}/skia.lib")
    
    # 检查库文件是否存在
    if(EXISTS "${SKIA_LIB_PATH}")
        set(SKIA_LIB "${SKIA_LIB_PATH}")
        set(SKIA_LIB_DIR "${SKIA_BUILD_DIR}")
        message(STATUS "Graphics: Found Skia library at ${SKIA_LIB}")
        
        # Skia 的依赖库（根据实际构建配置可能需要调整）
        set(SKIA_DEPENDENCY_LIBS
            "${SKIA_BUILD_DIR}/skcms.lib"
            "${SKIA_BUILD_DIR}/freetype2.lib"
            "${SKIA_BUILD_DIR}/harfbuzz.lib"
            "${SKIA_BUILD_DIR}/libpng.lib"
            "${SKIA_BUILD_DIR}/libjpeg.lib"
            "${SKIA_BUILD_DIR}/libwebp.lib"
            "${SKIA_BUILD_DIR}/zlib.lib"
            "${SKIA_BUILD_DIR}/expat.lib"
            "${SKIA_BUILD_DIR}/icu.lib"
        )
        
        # 只添加存在的依赖库
        set(SKIA_LINK_LIBS "${SKIA_LIB}")
        foreach(dep_lib ${SKIA_DEPENDENCY_LIBS})
            if(EXISTS "${dep_lib}")
                list(APPEND SKIA_LINK_LIBS "${dep_lib}")
            endif()
        endforeach()
        
        message(STATUS "Graphics: Skia library directory: ${SKIA_LIB_DIR}")
    else()
        # 尝试其他可能的路径
        set(SKIA_LIB_SEARCH_PATHS
            "${SKIA_DIR}/out/Release"
            "${SKIA_DIR}/out/Debug"
            "${SKIA_DIR}/out/Release-x64"
            "${SKIA_DIR}/out/Debug-x64"
            "${SKIA_DIR}/lib"
        )
        
        find_library(SKIA_LIB
            NAMES skia skia_shared
            PATHS ${SKIA_LIB_SEARCH_PATHS}
            NO_DEFAULT_PATH
        )
        
        if(SKIA_LIB)
            message(STATUS "Graphics: Found Skia library at ${SKIA_LIB}")
            get_filename_component(SKIA_LIB_DIR "${SKIA_LIB}" DIRECTORY)
        else()
            message(STATUS "Graphics: Skia library not found, will use header-only or link manually")
            set(SKIA_LIB "")
        endif()
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
        # 链接 Skia 主库和依赖库（使用完整路径，不需要 target_link_directories）
        if(SKIA_LINK_LIBS)
            target_link_libraries(Graphics PUBLIC ${SKIA_LINK_LIBS})
        else()
            target_link_libraries(Graphics PUBLIC ${SKIA_LIB})
        endif()
        
        message(STATUS "Graphics: Linking Skia library and dependencies")
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

