#include "RenderContext.hpp"
#include "RenderSurface.hpp"
#include <window.hpp>
#include <window_class.hpp>
#include <GLFW/glfw3.h>
#include <core/SkCanvas.h>
#include <core/SkPaint.h>
#include <core/SkColor.h>
#include <iostream>
#include <tracy/Tracy.hpp>
#include <cstdlib>

// 启用 Tracy 内存追踪
// 这会重载全局的 new/delete 操作符，自动追踪所有内存分配
// 所有通过 new/delete 分配的内存都会在 Tracy 中显示堆栈信息
#ifdef TRACY_ENABLE
void* operator new(std::size_t count) {
    auto ptr = std::malloc(count);
    TracyAlloc(ptr, count);
    return ptr;
}

void operator delete(void* ptr) noexcept {
    TracyFree(ptr);
    std::free(ptr);
}

void* operator new[](std::size_t count) {
    auto ptr = std::malloc(count);
    TracyAlloc(ptr, count);
    return ptr;
}

void operator delete[](void* ptr) noexcept {
    TracyFree(ptr);
    std::free(ptr);
}

#if defined(__cpp_aligned_new) && __cpp_aligned_new >= 201606L
// C++17 对齐分配（如果编译器支持）
#ifdef _WIN32
// Windows/MSVC 使用 _aligned_malloc
#include <malloc.h>
void* operator new(std::size_t count, std::align_val_t al) {
    auto ptr = _aligned_malloc(count, static_cast<std::size_t>(al));
    TracyAlloc(ptr, count);
    return ptr;
}

void operator delete(void* ptr, std::align_val_t al) noexcept {
    TracyFree(ptr);
    _aligned_free(ptr);
}

void* operator new[](std::size_t count, std::align_val_t al) {
    auto ptr = _aligned_malloc(count, static_cast<std::size_t>(al));
    TracyAlloc(ptr, count);
    return ptr;
}

void operator delete[](void* ptr, std::align_val_t al) noexcept {
    TracyFree(ptr);
    _aligned_free(ptr);
}
#else
// Linux/其他平台使用 std::aligned_alloc
#include <cstdlib>
void* operator new(std::size_t count, std::align_val_t al) {
    auto ptr = std::aligned_alloc(static_cast<std::size_t>(al), count);
    TracyAlloc(ptr, count);
    return ptr;
}

void operator delete(void* ptr, std::align_val_t al) noexcept {
    TracyFree(ptr);
    std::free(ptr);
}

void* operator new[](std::size_t count, std::align_val_t al) {
    auto ptr = std::aligned_alloc(static_cast<std::size_t>(al), count);
    TracyAlloc(ptr, count);
    return ptr;
}

void operator delete[](void* ptr, std::align_val_t al) noexcept {
    TracyFree(ptr);
    std::free(ptr);
}
#endif
#endif
#endif

using namespace KiUI::foundation;
using namespace KiUI::graphics;

int main() {
    ZoneScoped;  // 追踪整个 main 函数的执行时间
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
        FrameMark;  // 标记帧边界，用于显示帧率
        ZoneScopedN("Main Loop");  // 追踪主循环的执行时间
        
        // 开始绘制
        {
            ZoneScopedN("BeginFrame");
            auto canvasOpt = renderSurface->BeginFrame();
            if (canvasOpt) {
                ZoneScopedN("Draw Circle");
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
        }
        
        // 结束绘制并显示
        {
            ZoneScopedN("EndFrame");
            renderSurface->EndFrame();
        }
        
        // 处理事件
        {
            ZoneScopedN("Event Processing");
            glfwPollEvents();
            windowManager.PollMainThreadTasks();
        }
    }
    
    // 清理资源
    renderSurface->Destroy();
    renderContext->Shutdown();
    windowManager.ShutdownPlatformSubsystems();
    
    return 0;
}

