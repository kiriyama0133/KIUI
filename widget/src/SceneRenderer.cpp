#include "SceneRenderer.hpp"
#include <RenderSurface.hpp>
#include <window.hpp>
#include <window_class.hpp>
#include <GLFW/glfw3.h>
#include <logger.hpp>
#include <boost/optional.hpp>

#ifdef TRACY_ENABLE
#include <tracy/Tracy.hpp>
#endif

namespace KiUI {
namespace widget {

SceneRenderer::SceneRenderer() {
}

SceneRenderer::~SceneRenderer() {
}

void SceneRenderer::SetRoot(boost::shared_ptr<VisualElement> root) {
    root_ = root;
}

void SceneRenderer::CalculateLayout(float viewportWidth, float viewportHeight) {
    if (!root_) {
        return;
    }
    
    // 从根组件开始递归计算布局
    root_->CalculateLayout(viewportWidth, viewportHeight, 0.0f, 0.0f);
}

void SceneRenderer::Render(SkCanvas* canvas) {
#ifdef TRACY_ENABLE
    ZoneScopedN("SceneRenderer::Render");
#endif
    
    if (!canvas || !root_) {
        return;
    }
    
    // 从根组件开始递归渲染
    RenderElement(root_, canvas, 0.0f, 0.0f);
}

void SceneRenderer::RenderElement(boost::shared_ptr<VisualElement> element, SkCanvas* canvas, float offsetX, float offsetY) {
#ifdef TRACY_ENABLE
    ZoneScopedN("SceneRenderer::RenderElement");
#endif
    
    if (!element || !element->GetVisibility()) {
        return;
    }
    
    // 保存画布状态
    canvas->save();
    
    // 移动到元素位置
    float x = offsetX + element->GetLeft();
    float y = offsetY + element->GetTop();
    canvas->translate(x, y);
    
    // 渲染元素本身
    {
#ifdef TRACY_ENABLE
        ZoneScopedN("VisualElement::Render");
#endif
        element->Render(canvas);
    }
    
    // 递归渲染子元素
    const auto& children = element->GetChildren();
    if (!children.empty()) {
#ifdef TRACY_ENABLE
        ZoneScopedN("Render Children");
#endif
        for (const auto& child : children) {
            auto visualChild = boost::dynamic_pointer_cast<VisualElement>(child);
            if (visualChild) {
                // 子元素的位置已经是相对于父元素的，所以 offset 为 0
                RenderElement(visualChild, canvas, 0.0f, 0.0f);
            }
        }
    }
    
    // 恢复画布状态
    canvas->restore();
}

void SceneRenderer::Run(boost::shared_ptr<KiUI::graphics::RenderSurface> renderSurface,
                        boost::shared_ptr<KiUI::foundation::Window> window,
                        KiUI::foundation::WindowManager& windowManager) {
    if (!renderSurface || !window) {
        return;
    }
    
    // 主渲染循环（内部管理所有细节，包括性能追踪）
    while (!window->ShouldClose()) {
#ifdef TRACY_ENABLE
        FrameMark;
#endif
        
        // 开始绘制
        auto canvasOpt = renderSurface->BeginFrame();
        if (canvasOpt) {
            SkCanvas& canvas = canvasOpt->get();
            // 渲染场景（内部已包含性能追踪）
            Render(&canvas);
        }
        
        // 结束绘制并显示
        renderSurface->EndFrame();
        
        // 处理事件
        glfwPollEvents();
        windowManager.PollMainThreadTasks();
    }
}

void SceneRenderer::Clear() {
    root_.reset();
}

} // namespace widget
} // namespace KiUI

