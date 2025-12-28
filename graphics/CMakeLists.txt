# Graphics 子项目
# 渲染层，提供渲染上下文管理
cmake_minimum_required(VERSION 3.10.0)

project(Graphics VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(unofficial-angle CONFIG QUIET)

# 查找 Skia（尝试多种可能的包名）
find_package(skia CONFIG QUIET)
if(NOT skia_FOUND)
    find_package(Skia CONFIG QUIET)
endif()
if(NOT skia_FOUND AND NOT Skia_FOUND)
    find_package(unofficial-skia CONFIG QUIET)
endif()

# 如果通过 vcpkg 找不到，尝试手动查找
if(NOT unofficial-skia_FOUND AND NOT skia_FOUND AND NOT Skia_FOUND)
    # 尝试从 vcpkg 安装目录查找 Skia 头文件
    if(DEFINED ENV{VCPKG_ROOT})
        set(VCPKG_INSTALLED_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows")
        # 检查多种可能的路径
        if(EXISTS "${VCPKG_INSTALLED_DIR}/include/gpu/GrDirectContext.h")
            set(SKIA_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/include")
            set(SKIA_FOUND TRUE)
            message(STATUS "Graphics: Found Skia headers at ${SKIA_INCLUDE_DIR}")
        elseif(EXISTS "${VCPKG_INSTALLED_DIR}/include/skia/gpu/GrDirectContext.h")
            set(SKIA_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/include/skia")
            set(SKIA_FOUND TRUE)
            message(STATUS "Graphics: Found Skia headers at ${SKIA_INCLUDE_DIR}")
        elseif(EXISTS "${VCPKG_INSTALLED_DIR}/include/skia/include/gpu/GrDirectContext.h")
            set(SKIA_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/include/skia/include")
            set(SKIA_FOUND TRUE)
            message(STATUS "Graphics: Found Skia headers at ${SKIA_INCLUDE_DIR}")
        endif()
    endif()
endif()

add_library(Graphics STATIC
    src/RenderContext.cpp
    src/RenderSurface.cpp
)

target_include_directories(Graphics PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/graphics>
)

# 添加 Skia 包含目录（vcpkg 的 Skia 包将头文件放在 include/skia/include 目录下）
if(DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_INSTALLED_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows")
    # vcpkg 的 Skia 包的头文件在 include/skia/include 目录下
    if(EXISTS "${VCPKG_INSTALLED_DIR}/include/skia/include")
        target_include_directories(Graphics PUBLIC "${VCPKG_INSTALLED_DIR}/include/skia/include")
        message(STATUS "Graphics: Added Skia include directory: ${VCPKG_INSTALLED_DIR}/include/skia/include")
    elseif(EXISTS "${VCPKG_INSTALLED_DIR}/include")
        # 备用：如果结构不同，添加标准 include 目录
        target_include_directories(Graphics PUBLIC "${VCPKG_INSTALLED_DIR}/include")
        message(STATUS "Graphics: Added vcpkg include directory: ${VCPKG_INSTALLED_DIR}/include")
    endif()
endif()

# 如果手动找到了 Skia，也添加其包含目录
if(SKIA_FOUND AND SKIA_INCLUDE_DIR)
    target_include_directories(Graphics PUBLIC ${SKIA_INCLUDE_DIR})
endif()

target_link_libraries(Graphics PUBLIC Foundation)

if(unofficial-angle_FOUND)
    target_link_libraries(Graphics PUBLIC unofficial::angle::libEGL unofficial::angle::libGLESv2)
    target_compile_definitions(Graphics PUBLIC GRAPHICS_USE_ANGLE)
    message(STATUS "Graphics: Using ANGLE")
endif()

# 链接 Skia（vcpkg 会自动处理依赖库）
if(unofficial-skia_FOUND)
    # vcpkg 推荐使用 unofficial::skia::skia
    target_link_libraries(Graphics PUBLIC unofficial::skia::skia)
    target_compile_definitions(Graphics PUBLIC GRAPHICS_USE_SKIA)
    # vcpkg 的 unofficial-skia 包设置的包含目录是 include/skia，但头文件实际在 include/skia/include
    # 需要额外添加 include/skia/include 到包含目录
    if(DEFINED ENV{VCPKG_ROOT})
        set(VCPKG_INSTALLED_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows")
        if(EXISTS "${VCPKG_INSTALLED_DIR}/include/skia/include")
            target_include_directories(Graphics PUBLIC "${VCPKG_INSTALLED_DIR}/include/skia/include")
            message(STATUS "Graphics: Added Skia include directory: ${VCPKG_INSTALLED_DIR}/include/skia/include")
        endif()
    endif()
    message(STATUS "Graphics: Using Skia from vcpkg (unofficial-skia)")
elseif(skia_FOUND)
    target_link_libraries(Graphics PUBLIC skia::skia)
    target_compile_definitions(Graphics PUBLIC GRAPHICS_USE_SKIA)
    message(STATUS "Graphics: Using Skia from vcpkg (skia)")
elseif(Skia_FOUND)
    target_link_libraries(Graphics PUBLIC Skia::Skia)
    target_compile_definitions(Graphics PUBLIC GRAPHICS_USE_SKIA)
    message(STATUS "Graphics: Using Skia from vcpkg (Skia)")
elseif(SKIA_FOUND)
    # 手动找到的 Skia，需要手动链接库
    find_library(SKIA_LIB skia PATHS "${VCPKG_INSTALLED_DIR}/lib" NO_DEFAULT_PATH)
    if(SKIA_LIB)
        target_link_libraries(Graphics PUBLIC ${SKIA_LIB})
        target_compile_definitions(Graphics PUBLIC GRAPHICS_USE_SKIA)
        message(STATUS "Graphics: Using Skia from ${SKIA_INCLUDE_DIR}")
    else()
        message(WARNING "Graphics: Skia headers found but library not found")
    endif()
else()
    # 如果找不到 Skia 包，尝试直接链接库文件
    if(DEFINED ENV{VCPKG_ROOT})
        set(VCPKG_INSTALLED_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows")
        find_library(SKIA_LIB 
            NAMES skia skia_shared
            PATHS "${VCPKG_INSTALLED_DIR}/lib"
            NO_DEFAULT_PATH
        )
        if(SKIA_LIB)
            target_link_libraries(Graphics PUBLIC ${SKIA_LIB})
            target_compile_definitions(Graphics PUBLIC GRAPHICS_USE_SKIA)
            message(STATUS "Graphics: Linked Skia library directly: ${SKIA_LIB}")
        else()
            message(WARNING "Graphics: Skia not found. Please install skia via vcpkg: vcpkg install skia")
        endif()
    else()
        message(WARNING "Graphics: Skia not found. Please install skia via vcpkg: vcpkg install skia")
    endif()
endif()

target_compile_definitions(Graphics PRIVATE
    GRAPHICS_VERSION_MAJOR=${Graphics_VERSION_MAJOR}
    GRAPHICS_VERSION_MINOR=${Graphics_VERSION_MINOR}
    GRAPHICS_VERSION_PATCH=${Graphics_VERSION_PATCH}
    NOMINMAX
)

# ==================== 示例程序 ====================
add_executable(DrawTriangle examples/draw_triangle.cpp)

target_include_directories(DrawTriangle PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/foundation/src/hpps
)

target_link_libraries(DrawTriangle PRIVATE Graphics Foundation)

if(WIN32)
    target_link_libraries(DrawTriangle PRIVATE opengl32)
endif()

# 注意：Skia 及其依赖库已通过 Graphics 库 PUBLIC 链接，无需重复链接

# ==================== 安装配置 ====================
install(TARGETS Graphics
    EXPORT GraphicsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/graphics
)

install(DIRECTORY include/
    DESTINATION include/graphics
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl"
)

install(EXPORT GraphicsTargets
    FILE GraphicsTargets.cmake
    NAMESPACE Graphics::
    DESTINATION lib/cmake/Graphics
)

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

