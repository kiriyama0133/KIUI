#include "hpps/window_class.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

namespace KiUI {
namespace foundation {

Window::Window(GLFWwindow* handle) 
    : handle_(handle) {
    if (!handle_) {
        std::cerr << "Warning: Window created with null GLFW handle" << std::endl;
    }
}

Window::~Window() {
    if (handle_) {
        glfwDestroyWindow(handle_);
        handle_ = nullptr;
    }
}

bool Window::ShouldClose() const {
    if (!handle_) {
        return true;
    }
    return glfwWindowShouldClose(handle_) != 0;
}

void Window::OnUpdate() {
    if (!handle_) {
        return;
    }
    // 更新窗口状态，例如处理输入事件等
    // GLFW 事件已经在 WindowManager::EnterMainMessageLoop() 中通过 glfwPollEvents() 处理
}

void Window::OnRender() {
    if (!handle_) {
        return;
    }
    // 渲染窗口内容
    // 这里可以调用交换链的 present 操作
    // 具体实现取决于使用的渲染后端（ANGLE/DirectX/Vulkan等）
}

} // namespace foundation
} // namespace KiUI

