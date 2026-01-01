# Widget 子项目
# UI 组件库，提供各种 UI 控件
cmake_minimum_required(VERSION 3.10.0)

project(Widget VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(unofficial-angle CONFIG QUIET)
find_package(skia CONFIG QUIET)
if(NOT skia_FOUND)
    find_package(Skia CONFIG QUIET)
endif()
if(NOT skia_FOUND AND NOT Skia_FOUND)
    find_package(unofficial-skia CONFIG QUIET)
endif()

# Widget 库（静态库，供其他模块链接）
add_library(Widget STATIC
    src/clock.cpp
)

# 公共头文件目录
target_include_directories(Widget PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/widget>
)

# 私有头文件目录（用于编译源文件）
target_include_directories(Widget PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Widget PUBLIC
    Graphics
    Foundation
)

if(unofficial-angle_FOUND)
    target_link_libraries(Widget PUBLIC unofficial::angle::libEGL unofficial::angle::libGLESv2)
    target_compile_definitions(Widget PUBLIC WIDGET_USE_ANGLE)
    message(STATUS "Widget: Using ANGLE")
endif()

if(unofficial-skia_FOUND)
    target_link_libraries(Widget PUBLIC unofficial::skia::skia)
    target_compile_definitions(Widget PUBLIC WIDGET_USE_SKIA)
    if(DEFINED ENV{VCPKG_ROOT})
        set(VCPKG_INSTALLED_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows")
        if(EXISTS "${VCPKG_INSTALLED_DIR}/include/skia/include")
            target_include_directories(Widget PUBLIC "${VCPKG_INSTALLED_DIR}/include/skia/include")
            message(STATUS "Widget: Added Skia include directory: ${VCPKG_INSTALLED_DIR}/include/skia/include")
        endif()
    endif()
    message(STATUS "Widget: Using Skia from vcpkg (unofficial-skia)")
elseif(skia_FOUND)
    target_link_libraries(Widget PUBLIC skia::skia)
    target_compile_definitions(Widget PUBLIC WIDGET_USE_SKIA)
    message(STATUS "Widget: Using Skia from vcpkg (skia)")
elseif(Skia_FOUND)
    target_link_libraries(Widget PUBLIC Skia::Skia)
    target_compile_definitions(Widget PUBLIC WIDGET_USE_SKIA)
    message(STATUS "Widget: Using Skia from vcpkg (Skia)")
elseif(DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_INSTALLED_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows")
    find_library(SKIA_LIB 
        NAMES skia skia_shared
        PATHS "${VCPKG_INSTALLED_DIR}/lib"
        NO_DEFAULT_PATH
    )
    if(SKIA_LIB)
        target_link_libraries(Widget PUBLIC ${SKIA_LIB})
        target_compile_definitions(Widget PUBLIC WIDGET_USE_SKIA)
        if(EXISTS "${VCPKG_INSTALLED_DIR}/include/skia/include")
            target_include_directories(Widget PUBLIC "${VCPKG_INSTALLED_DIR}/include/skia/include")
        endif()
        message(STATUS "Widget: Linked Skia library directly: ${SKIA_LIB}")
    else()
        message(WARNING "Widget: Skia not found. Please install skia via vcpkg: vcpkg install skia")
    endif()
endif()

if(WIN32)
    target_link_libraries(Widget PUBLIC opengl32)
endif()

target_compile_definitions(Widget PRIVATE
    WIDGET_VERSION_MAJOR=${Widget_VERSION_MAJOR}
    WIDGET_VERSION_MINOR=${Widget_VERSION_MINOR}
    WIDGET_VERSION_PATCH=${Widget_VERSION_PATCH}
    NOMINMAX
)

install(TARGETS Widget
    EXPORT WidgetTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/widget
)

install(DIRECTORY include/
    DESTINATION include/widget
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl"
)

install(EXPORT WidgetTargets
    FILE WidgetTargets.cmake
    NAMESPACE Widget::
    DESTINATION lib/cmake/Widget
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/WidgetConfigVersion.cmake"
    VERSION ${Widget_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/WidgetConfigVersion.cmake"
    DESTINATION lib/cmake/Widget
)

