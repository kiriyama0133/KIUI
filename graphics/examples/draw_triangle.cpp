#include "RenderContext.hpp"
#include "RenderSurface.hpp"
#include <window.hpp>
#include <window_class.hpp>
#include <GLFW/glfw3.h>
#include <core/SkCanvas.h>
#include <core/SkPaint.h>
#include <core/SkColor.h>
#include <iostream>

using namespace KiUI::foundation;
using namespace KiUI::graphics;

int main() {
    // 初始化窗口管理器
    auto& windowManager = WindowManager::GetSharedInstance();
    if (!windowManager.InitializePlatformSubsystems()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }
    
    // 创建窗口
    auto window = windowManager.CreateNativeWindow("Circle Test", 800, 600, false, true);
    if (!window) {
        windowManager.ShutdownPlatformSubsystems();
        return 1;
    }
    
    // 创建并初始化渲染上下文
    auto renderContext = boost::make_shared<RenderContext>();
    if (!renderContext->Initialize()) {
        windowManager.ShutdownPlatformSubsystems();
        return 1;
    }
    
    // 创建并初始化渲染表面
    auto renderSurface = boost::make_shared<RenderSurface>(renderContext, window);
    if (!renderSurface->Initialize()) {
        renderContext->Shutdown();
        windowManager.ShutdownPlatformSubsystems();
        return 1;
    }
    
    // 主循环：绘制圆
    while (!window->ShouldClose()) {
        // 开始绘制
        auto canvasOpt = renderSurface->BeginFrame();
        if (canvasOpt) {
            SkCanvas& canvas = canvasOpt->get();
            int width = canvas.getBaseLayerSize().width();
            int height = canvas.getBaseLayerSize().height();
            
            // 绘制圆（在窗口中心）
            float centerX = width / 2.0f;
            float centerY = height / 2.0f;
            float radius = std::min(width, height) * 0.3f;
            
            SkPaint paint;
            paint.setColor(SK_ColorBLUE);
            paint.setAntiAlias(true);
            
            canvas.drawCircle(centerX, centerY, radius, paint);
        }
        
        // 结束绘制并显示
        renderSurface->EndFrame();
        
        // 处理事件
        glfwPollEvents();
        windowManager.PollMainThreadTasks();
    }
    
    // 清理资源
    renderSurface->Destroy();
    renderContext->Shutdown();
    windowManager.ShutdownPlatformSubsystems();
    
    return 0;
}

