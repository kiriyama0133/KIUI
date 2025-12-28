cmake_minimum_required(VERSION 3.10.0)
project(WindowsApiSolutions VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置安装前缀为项目根目录下的 SDK 文件夹
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/SDK" CACHE PATH "Installation directory" FORCE)
endif()

# 查找公共依赖
find_package(Boost REQUIRED COMPONENTS system filesystem thread date_time)

# 添加 Foundation 基础库子项目
add_subdirectory(foundation)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
