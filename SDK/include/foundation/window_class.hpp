#ifndef WINDOW_CLASS_HPP
#define WINDOW_CLASS_HPP
#pragma once

#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <boost/signals2.hpp>

struct GLFWwindow;

namespace KiUI {
namespace foundation {

/**
 * @brief Window: 封装原生窗口的类
 * 每个窗口独立管理自己的 DPI 缩放因子
 */
class Window {
public:
    Window(GLFWwindow* handle, bool isFrameless = false);
    ~Window();

    // 检查窗口是否应该关闭
    bool ShouldClose() const;
    
    // 更新窗口状态
    void OnUpdate();
    
    // 渲染窗口内容
    void OnRender();

    // 获取 GLFW 窗口句柄
    GLFWwindow* GetHandle() const { return handle_; }

    // DPI 相关方法
    float GetContentScaleX() const { return contentScaleX_; }
    float GetContentScaleY() const { return contentScaleY_; }
    float GetContentScale() const { return contentScaleX_; } // 返回 X 缩放因子作为主要缩放值

    // DPI 变化信号（当窗口所在的屏幕 DPI 改变时触发）
    boost::signals2::signal<void(float xScale, float yScale)> OnContentScaleChanged;
    
    // 焦点变化信号（当窗口获得或失去焦点时触发）
    boost::signals2::signal<void(bool focused)> OnFocusChanged;

private:
    GLFWwindow* handle_;
    bool isFrameless_;
    
    // DPI 缩放因子（窗口所在屏幕的缩放）
    float contentScaleX_ = 1.0f;
    float contentScaleY_ = 1.0f;
    
    // 更新 DPI 缩放因子（由 GLFW 回调调用）
    void UpdateContentScale(float xScale, float yScale);
    
    // 设置 GLFW 窗口回调
    void SetupCallbacks();
    
    // GLFW 内容缩放回调的静态包装函数
    static void ContentScaleCallback(GLFWwindow* window, float xScale, float yScale);
    
    // GLFW 焦点回调的静态包装函数
    static void FocusCallback(GLFWwindow* window, int focused);
};

} // namespace foundation
} // namespace KiUI

#endif // WINDOW_CLASS_HPP

