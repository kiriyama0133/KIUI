#ifndef WINDOW_CLASS_HPP
#define WINDOW_CLASS_HPP
#pragma once

#include <GLFW/glfw3.h>
#include <array>
#include <cmath>
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
    static constexpr std::array<float, 5> PresetStepSizes = {1.0f, 1.25f, 1.5f, 1.75f, 2.0f};
    static constexpr float MinScale = 1.0f;
    static constexpr float MaxScale = 2.0f;
    
    /**
     * @brief 将缩放值对齐到最近的预设步长，并限制在 1.0f - 2.0f 范围内
     * @param value 输入的缩放值
     * @return 对齐后的缩放值，保证在 [MinScale, MaxScale] 范围内
     */
    static float SnapToNearestStep(float value) noexcept {
        float clampedValue = std::max(MinScale, std::min(MaxScale, value));
        float bestStep = PresetStepSizes[0];
        float minDiff = std::abs(clampedValue - bestStep);
        for (std::size_t i = 0; i < PresetStepSizes.size(); ++i) {
            float step = PresetStepSizes[i];
            float diff = std::abs(clampedValue - step);  // 计算绝对差值
            if (diff < minDiff) {  // 如果找到更接近的值
                minDiff = diff;
                bestStep = step;
            }
        }
        return bestStep;  // 返回最接近的预设步长（保证在 [MinScale, MaxScale] 范围内）
    }
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
    
    // 窗口需要重绘信号（当窗口内容需要重新渲染时触发，如 DPI 变化）
    boost::signals2::signal<void()> OnInvalidate;

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

