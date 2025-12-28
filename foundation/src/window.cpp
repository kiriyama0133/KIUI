#include "hpps/window_class.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

namespace KiUI {
namespace foundation {

Window::Window(GLFWwindow* handle, bool isFrameless) 
    : handle_(handle), isFrameless_(isFrameless) {
    if (!handle_) {
        std::cerr << "Warning: Window created with null GLFW handle" << std::endl;
        return;
    }
    
    // 初始化 DPI 缩放因子
    glfwGetWindowContentScale(handle_, &contentScaleX_, &contentScaleY_);
    // 设置窗口用户指针，用于回调中获取 Window 实例
    glfwSetWindowUserPointer(handle_, this);
    // 设置 GLFW 回调
    SetupCallbacks();
}

Window::~Window() {
    if (handle_) {
        glfwSetWindowUserPointer(handle_, nullptr);
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

void Window::SetupCallbacks() {
    if (!handle_) {
        return;
    }
    
    // 设置内容缩放回调
    glfwSetWindowContentScaleCallback(handle_, ContentScaleCallback);
    
    // 设置焦点回调
    glfwSetWindowFocusCallback(handle_, FocusCallback);
}

void Window::ContentScaleCallback(GLFWwindow* window, float xScale, float yScale) {
    // 从窗口用户指针获取 Window 实例
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win) {
        win->UpdateContentScale(xScale, yScale);
    }
}

void Window::UpdateContentScale(float xScale, float yScale) {
    // 只有当缩放因子真正改变时才更新和发出信号
    if (contentScaleX_ != xScale || contentScaleY_ != yScale) {
        contentScaleX_ = xScale;
        contentScaleY_ = yScale;
        
        // 发出 DPI 变化信号
        OnContentScaleChanged(xScale, yScale);
    }
}

void Window::FocusCallback(GLFWwindow* window, int focused) {
    // 从窗口用户指针获取 Window 实例
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win) {
        // 发出焦点变化信号（focused 为 1 表示获得焦点，0 表示失去焦点）
        win->OnFocusChanged(focused != 0);
    }
}

} // namespace foundation
} // namespace KiUI

