#ifndef WINDOW_CLASS_HPP
#define WINDOW_CLASS_HPP
#pragma once

#include <GLFW/glfw3.h>
#include <memory>
#include <string>

struct GLFWwindow;

namespace KiUI {
namespace foundation {

/**
 * @brief Window: 封装原生窗口的类
 */
class Window {
public:
    Window(GLFWwindow* handle);
    ~Window();

    // 检查窗口是否应该关闭
    bool ShouldClose() const;
    
    // 更新窗口状态
    void OnUpdate();
    
    // 渲染窗口内容
    void OnRender();

    // 获取 GLFW 窗口句柄
    GLFWwindow* GetHandle() const { return handle_; }

private:
    GLFWwindow* handle_;
};

} // namespace foundation
} // namespace KiUI

#endif // WINDOW_CLASS_HPP

