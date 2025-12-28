# Foundation 子项目
# 提供窗口管理、上下文初始化（ANGLE）、交换链管理、输入服务、高分屏和环境感知
cmake_minimum_required(VERSION 3.10.0)

project(Foundation VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找依赖
# ANGLE (Almost Native Graphics Layer Engine) - 用于 OpenGL ES 到 DirectX 的桥接
find_package(unofficial-angle CONFIG QUIET)

# GLFW - 跨平台窗口和输入管理库（可选，如果 ANGLE 不提供窗口管理）
find_package(glfw3 CONFIG QUIET)

# Windows API
if(WIN32)
    # Windows 特定的库
    set(WINDOWS_LIBS
        user32
        gdi32
        winmm
        dwmapi
        shcore  # 高分屏支持
    )
endif()

# Foundation 库（静态库，供其他模块链接）
add_library(Foundation STATIC
    src/window_manager.cpp
    src/window.cpp
)

# 公共头文件目录（源文件中的 hpps 目录）
target_include_directories(Foundation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/hpps>
    $<INSTALL_INTERFACE:include>
)

# 链接库
target_link_libraries(Foundation PUBLIC
    Boost::boost
    Boost::system
    Boost::filesystem
    Boost::thread
    Boost::date_time
    ${WINDOWS_LIBS}
)

# 如果找到 ANGLE，链接它
if(unofficial-angle_FOUND)
    target_link_libraries(Foundation PUBLIC unofficial::angle::libEGL unofficial::angle::libGLESv2)
    target_compile_definitions(Foundation PUBLIC FOUNDATION_USE_ANGLE)
    message(STATUS "Foundation: Using ANGLE for graphics context")
else()
    message(STATUS "Foundation: ANGLE not found, using native graphics APIs")
endif()

# 如果找到 GLFW，链接它（用于跨平台窗口管理）
if(glfw3_FOUND)
    target_link_libraries(Foundation PUBLIC glfw)
    target_compile_definitions(Foundation PUBLIC FOUNDATION_USE_GLFW)
    message(STATUS "Foundation: Using GLFW for window management")
endif()

# 编译定义
target_compile_definitions(Foundation PRIVATE
    FOUNDATION_VERSION_MAJOR=${Foundation_VERSION_MAJOR}
    FOUNDATION_VERSION_MINOR=${Foundation_VERSION_MINOR}
    FOUNDATION_VERSION_PATCH=${Foundation_VERSION_PATCH}
    _WIN32_WINNT=0x0A00
)

# ==================== 安装配置 ====================
# 安装 Foundation 库
install(TARGETS Foundation
    EXPORT FoundationTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# 安装头文件到 SDK/include
# 如果存在 include/ 目录，安装其中的头文件
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
    install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING 
            PATTERN "*.h"
            PATTERN "*.hpp"
            PATTERN "*.inl"
    )
endif()

# 安装 src/hpps/ 目录中的头文件到 SDK/include/foundation
install(DIRECTORY src/hpps/
    DESTINATION include/foundation
    FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)

# 安装导出文件（供其他项目使用）
install(EXPORT FoundationTargets
    FILE FoundationTargets.cmake
    NAMESPACE Foundation::
    DESTINATION lib/cmake/Foundation
)

# 创建配置文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/FoundationConfigVersion.cmake"
    VERSION ${Foundation_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/FoundationConfigVersion.cmake"
    DESTINATION lib/cmake/Foundation
)

# ==================== 测试配置 ====================

# 查找 Google Test
find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND)
    # 如果找不到 GTest，尝试使用 FetchContent 下载
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    # 对于 Windows: 防止覆盖父项目的编译选项
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# 启用测试
enable_testing()

# 测试可执行文件
add_executable(FoundationTests
    tests/main.cpp
    tests/test_window_manager.cpp
    tests/test_window_focus.cpp
)

target_include_directories(FoundationTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hpps
)

target_link_libraries(FoundationTests PRIVATE
    Foundation
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    ${WINDOWS_LIBS}
)

# 如果找到 ANGLE，测试也需要链接
if(unofficial-angle_FOUND)
    target_link_libraries(FoundationTests PRIVATE unofficial::angle::libEGL unofficial::angle::libGLESv2)
endif()

# 如果找到 GLFW，测试也需要链接
if(glfw3_FOUND)
    target_link_libraries(FoundationTests PRIVATE glfw)
endif()

# 添加测试到 CTest
add_test(NAME FoundationTests COMMAND FoundationTests)

# 设置测试属性
set_tests_properties(FoundationTests PROPERTIES
    TIMEOUT 60
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
