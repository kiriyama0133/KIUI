#ifndef SCENE_RENDERER_HPP
#define SCENE_RENDERER_HPP
#pragma once

#include "VisualElement.hpp"
#include <include/core/SkCanvas.h>
#include <boost/shared_ptr.hpp>

namespace KiUI {
namespace foundation {
class Window;
class WindowManager;
}
namespace graphics {
class RenderSurface;
}
namespace widget {

/**
 * @brief SceneRenderer - 场景渲染器
 * 统一管理所有组件的渲染逻辑，让开发者无需关心渲染细节
 */
class SceneRenderer {
public:
    /**
     * @brief 构造函数
     */
    SceneRenderer();
    
    /**
     * @brief 析构函数
     */
    ~SceneRenderer();
    
    /**
     * @brief 设置根组件
     * @param root 根组件（通常是占满整个窗口的容器）
     */
    void SetRoot(boost::shared_ptr<VisualElement> root);
    
    /**
     * @brief 获取根组件
     * @return 根组件的共享指针
     */
    boost::shared_ptr<VisualElement> GetRoot() const { return root_; }
    
    /**
     * @brief 计算布局
     * 递归计算所有组件的布局位置和大小
     * @param viewportWidth 视口宽度（通常是窗口宽度）
     * @param viewportHeight 视口高度（通常是窗口高度）
     */
    void CalculateLayout(float viewportWidth, float viewportHeight);
    
    /**
     * @brief 渲染场景
     * 递归渲染所有可见组件（内部包含 Tracy 性能追踪）
     * @param canvas Skia 画布
     */
    void Render(SkCanvas* canvas);
    
    /**
     * @brief 运行渲染循环
     * 管理整个渲染循环，包括 BeginFrame、Render、EndFrame 和事件处理
     * 开发者无需自己管理 while 循环
     * @param renderSurface 渲染表面
     * @param window 窗口
     * @param windowManager 窗口管理器
     */
    void Run(boost::shared_ptr<KiUI::graphics::RenderSurface> renderSurface,
             boost::shared_ptr<KiUI::foundation::Window> window,
             KiUI::foundation::WindowManager& windowManager);
    
    /**
     * @brief 清除场景
     * 移除根组件并清理资源
     */
    void Clear();

private:
    /**
     * @brief 递归渲染组件
     * @param element 要渲染的组件
     * @param canvas Skia 画布
     * @param offsetX 父组件的 x 偏移
     * @param offsetY 父组件的 y 偏移
     */
    void RenderElement(boost::shared_ptr<VisualElement> element, SkCanvas* canvas, float offsetX, float offsetY);
    
    boost::shared_ptr<VisualElement> root_;
};

} // namespace widget
} // namespace KiUI

#endif // SCENE_RENDERER_HPP

