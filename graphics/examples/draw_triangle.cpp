#include "RenderContext.hpp"
#include "RenderSurface.hpp"
#include <window.hpp>  // WindowManager
#include <window_class.hpp>
#include <GLFW/glfw3.h>
#include <core/SkCanvas.h>
#include <core/SkPath.h>
#include <core/SkPaint.h>
#include <core/SkColor.h>
#include <thread>
#include <chrono>
#include <iostream>
#include <algorithm>

using namespace KiUI::foundation;
using namespace KiUI::graphics;

int main() {
    std::cout << "Initializing WindowManager..." << std::endl;
    auto& windowManager = WindowManager::GetSharedInstance();
    
    if (!windowManager.InitializePlatformSubsystems()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }
    
    std::cout << "Creating window..." << std::endl;
    // 创建窗口
    auto window = windowManager.CreateNativeWindow("Triangle Test", 800, 600, false, true);
    if (!window || !window->GetHandle()) {
        std::cerr << "Failed to create window" << std::endl;
        windowManager.ShutdownPlatformSubsystems();
        return 1;
    }
    
    // 等待窗口完全初始化
    glfwPollEvents();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Creating RenderContext..." << std::endl;
    // 创建渲染上下文
    auto renderContext = boost::make_shared<RenderContext>();
    if (!renderContext) {
        std::cerr << "Failed to create RenderContext" << std::endl;
        windowManager.CloseAndReleaseWindow(window);
        windowManager.ShutdownPlatformSubsystems();
        return 1;
    }
    
    // 初始化渲染上下文
    if (!renderContext->Initialize()) {
        std::cerr << "Failed to initialize RenderContext" << std::endl;
        windowManager.CloseAndReleaseWindow(window);
        windowManager.ShutdownPlatformSubsystems();
        return 1;
    }
    
    if (!renderContext->IsInitialized()) {
        std::cerr << "RenderContext is not initialized" << std::endl;
        renderContext->Shutdown();
        windowManager.CloseAndReleaseWindow(window);
        windowManager.ShutdownPlatformSubsystems();
        return 1;
    }
    
    std::cout << "Creating RenderSurface..." << std::endl;
    // 创建渲染表面
    auto renderSurface = boost::make_shared<RenderSurface>(renderContext, window);
    if (!renderSurface) {
        std::cerr << "Failed to create RenderSurface" << std::endl;
        renderContext->Shutdown();
        windowManager.CloseAndReleaseWindow(window);
        windowManager.ShutdownPlatformSubsystems();
        return 1;
    }
    
    // 初始化渲染表面
    if (!renderSurface->Initialize()) {
        std::cerr << "Failed to initialize RenderSurface" << std::endl;
        renderContext->Shutdown();
        windowManager.CloseAndReleaseWindow(window);
        windowManager.ShutdownPlatformSubsystems();
        return 1;
    }
    
    std::cout << "Window created, RenderContext and RenderSurface initialized" << std::endl;
    std::cout << "Drawing triangle... (window will stay open for 5 seconds)" << std::endl;
    std::cout << "Close the window or wait for 5 seconds to exit" << std::endl;
    
    // 绘制三角形（循环几帧以确保显示）
    const int totalFrames = 300; // 约 5 秒（60 FPS）
    for (int frame = 0; frame < totalFrames; ++frame) {
        // 开始绘制一帧
        auto canvasOpt = renderSurface->BeginFrame();
        if (!canvasOpt) {
            std::cerr << "Failed to begin frame" << std::endl;
            break;
        }
        
        SkCanvas& canvas = canvasOpt->get();
        
        // 获取画布大小
        int width = canvas.getBaseLayerSize().width();
        int height = canvas.getBaseLayerSize().height();
        
        if (width > 0 && height > 0) {
            // 创建路径来绘制三角形
            SkPath trianglePath;
            
            // 定义三角形的三个顶点（在窗口中心绘制）
            float centerX = width / 2.0f;
            float centerY = height / 2.0f;
            float size = std::min(width, height) * 0.3f;  // 三角形大小为窗口的30%
            
            // 等边三角形的三个顶点
            trianglePath.moveTo(centerX, centerY - size);           // 顶点（上）
            trianglePath.lineTo(centerX - size * 0.866f, centerY + size * 0.5f);  // 左下
            trianglePath.lineTo(centerX + size * 0.866f, centerY + size * 0.5f);  // 右下
            trianglePath.close();  // 闭合路径形成三角形
            
            // 创建画笔
            SkPaint paint;
            paint.setColor(SK_ColorBLUE);      // 填充颜色：蓝色
            paint.setStyle(SkPaint::kFill_Style);
            paint.setAntiAlias(true);          // 抗锯齿
            
            // 绘制填充的三角形
            canvas.drawPath(trianglePath, paint);
            
            // 绘制三角形边框
            SkPaint strokePaint;
            strokePaint.setColor(SK_ColorRED);  // 边框颜色：红色
            strokePaint.setStyle(SkPaint::kStroke_Style);
            strokePaint.setStrokeWidth(3.0f);
            strokePaint.setAntiAlias(true);
            
            canvas.drawPath(trianglePath, strokePaint);
        }
        
        // 结束绘制并显示
        renderSurface->EndFrame();
        
        // 处理窗口事件
        glfwPollEvents();
        windowManager.PollMainThreadTasks();
        
        // 检查窗口是否关闭
        if (window->ShouldClose()) {
            std::cout << "Window was closed by user" << std::endl;
            break;
        }
        
        // 每帧等待约 16ms（约 60 FPS）
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    std::cout << "Triangle drawing completed" << std::endl;
    
    // 清理资源
    renderSurface->Destroy();
    renderContext->Shutdown();
    windowManager.CloseAndReleaseWindow(window);
    windowManager.ShutdownPlatformSubsystems();
    
    return 0;
}

