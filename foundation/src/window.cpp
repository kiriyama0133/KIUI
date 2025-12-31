#include "hpps/window_class.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include "hpps/logger.hpp"

namespace KiUI {
namespace foundation {

Window::Window(GLFWwindow* handle, bool isFrameless) 
    : handle_(handle), isFrameless_(isFrameless) {
    if (!handle_) {
        std::cerr << "Warning: Window created with null GLFW handle" << std::endl;
        return;
    }
    
    // 初始化 DPI 缩放因子（从 GLFW 获取系统值）
    float systemXScale, systemYScale;
    glfwGetWindowContentScale(handle_, &systemXScale, &systemYScale);
    
    // 应用限制和对齐（确保初始值也在预期范围内）
    contentScaleX_ = SnapToNearestStep(systemXScale);
    contentScaleY_ = SnapToNearestStep(systemYScale);
    
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
}

void Window::OnRender() {
    if (!handle_) {
        return;
    }
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
    float snappedX = Window::SnapToNearestStep(xScale);
    float snappedY = Window::SnapToNearestStep(yScale);
    if (contentScaleX_ != snappedX || contentScaleY_ != snappedY) {
        contentScaleX_ = snappedX;
        contentScaleY_ = snappedY;
        #ifndef NDEBUG
            Logger::Debug("Window::UpdateContentScale: xScale = {0}, yScale = {1}, snappedX = {2}, snappedY = {3}", 
                          xScale, yScale, snappedX, snappedY);
        #endif
        // 发出 DPI 变化信号，通知 WindowManager
        OnContentScaleChanged(contentScaleX_, contentScaleY_);
        // 通知窗口需要重新绘制（DPI 变化后需要重新渲染以适应新的缩放）
        OnInvalidate();
    }
}

void Window::FocusCallback(GLFWwindow* window, int focused) {
    // 从窗口用户指针获取 Window 实例
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (win) {
        // 发出焦点变化信号（focused 为 1 表示获得焦点，0 表示失去焦点）
        #ifdef __DEBUG__
            Logger::Debug("Window::FocusCallback: focused = {}", focused);
        #endif
        win->OnFocusChanged(focused != 0);
    }
}

} // namespace foundation
} // namespace KiUI

