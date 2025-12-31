// 禁用 Windows 的 min/max 宏，避免与 C++ 标准库和 Skia 冲突
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "RenderSurface.hpp"
#include "RenderContext.hpp"
#include <window_class.hpp>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglext_angle.h>
#include <gpu/ganesh/GrDirectContext.h>
#include <gpu/ganesh/GrRecordingContext.h>  // GrRecordingContext (base class for GrDirectContext)
#include <gpu/ganesh/gl/GrGLDirectContext.h>
#include <gpu/ganesh/SkSurfaceGanesh.h>  // WrapBackendRenderTarget
#include <gpu/ganesh/gl/GrGLBackendSurface.h>  // GrBackendRenderTargets::MakeGL
#include <gpu/ganesh/gl/GrGLTypes.h>  // GrGLFramebufferInfo, GrGLenum
#include <core/SkSurface.h>
#include <core/SkCanvas.h>
#include <core/SkColorSpace.h>  // SkColorSpace (needed by SkSurface)
#include <gpu/ganesh/GrBackendSurface.h>
#include <gpu/ganesh/GrTypes.h>
#include <iostream>

namespace KiUI {
namespace graphics {

struct RenderSurface::Impl {
    boost::shared_ptr<RenderContext> context_;
    EGLSurface eglSurface_ = EGL_NO_SURFACE;
    sk_sp<SkSurface> skSurface_;
    int width_ = 0;
    int height_ = 0;
    bool initialized_ = false;
    SkCanvas* currentCanvas_ = nullptr;
};

RenderSurface::RenderSurface(boost::shared_ptr<RenderContext> context, 
                             boost::shared_ptr<foundation::Window> window)
    : impl_(new Impl())
    , targetWindow_(window)
{
    impl_->context_ = context;
}

RenderSurface::~RenderSurface() {
    Destroy();
}

bool RenderSurface::Initialize() {
    if (impl_->initialized_) {
        std::cerr << "RenderSurface: Already initialized" << std::endl;
        return false;
    }
    
    if (!impl_->context_ || !impl_->context_->IsInitialized()) {
        std::cerr << "RenderSurface: RenderContext is not initialized" << std::endl;
        return false;
    }
    
    auto window = targetWindow_.lock();
    if (!window) {
        std::cerr << "RenderSurface: Window is null or expired" << std::endl;
        return false;
    }
    
    // 获取 GLFW 窗口句柄
    GLFWwindow* glfwWindow = window->GetHandle();
    if (!glfwWindow) {
        std::cerr << "RenderSurface: GLFW window handle is null" << std::endl;
        return false;
    }
    
    // 获取窗口句柄
    #ifdef _WIN32
    HWND windowHandle = glfwGetWin32Window(glfwWindow);
    if (!windowHandle) {
        std::cerr << "RenderSurface: Failed to get Win32 window handle" << std::endl;
        return false;
    }
    #endif
    #ifdef __APPLE__
    NSWindow* windowHandle = glfwGetCocoaWindow(glfwWindow);
    if (!windowHandle) {
        std::cerr << "RenderSurface: Failed to get NSWindow handle" << std::endl;
        return false;
    }
    #endif
    #ifdef __linux__
    XWindow* windowHandle = glfwGetX11Window(glfwWindow);
    if (!windowHandle) {
        std::cerr << "RenderSurface: Failed to get XWindow handle" << std::endl;
        return false;
    }
    #endif
    
    // 获取 EGL 原生句柄
    auto nativeHandles = impl_->context_->GetNativeHandles();
    EGLDisplay display = static_cast<EGLDisplay>(nativeHandles.display);
    EGLConfig config = static_cast<EGLConfig>(nativeHandles.config);
    
    if (display == EGL_NO_DISPLAY || config == nullptr) {
        std::cerr << "RenderSurface: Invalid EGL handles from RenderContext" << std::endl;
        return false;
    }
    
    // 获取窗口大小（考虑 DPI 缩放）
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);
    impl_->width_ = width;
    impl_->height_ = height;
    
    if (impl_->width_ <= 0 || impl_->height_ <= 0) {
        std::cerr << "RenderSurface: Invalid window size: " << width << "x" << height << std::endl;
        return false;
    }
    
    // 创建 EGLSurface（将窗口句柄转换为 EGL 表面）
    EGLint surfaceAttributes[] = {
        EGL_NONE
    };
    
    // 创建 EGLSurface
    impl_->eglSurface_ = eglCreateWindowSurface(display, config, windowHandle, surfaceAttributes);
    
    if (impl_->eglSurface_ == EGL_NO_SURFACE) {
        EGLint error = eglGetError();
        std::cerr << "RenderSurface: Failed to create EGL surface, error: 0x" 
                  << std::hex << error << std::dec << std::endl;
        return false;
    }
    
    // 使 EGL 上下文成为当前上下文
    EGLContext context = static_cast<EGLContext>(nativeHandles.context);
    if (!eglMakeCurrent(display, impl_->eglSurface_, impl_->eglSurface_, context)) {
        EGLint error = eglGetError();
        std::cerr << "RenderSurface: Failed to make EGL context current, error: 0x" 
                  << std::hex << error << std::dec << std::endl;
        eglDestroySurface(display, impl_->eglSurface_);
        impl_->eglSurface_ = EGL_NO_SURFACE;
        return false;
    }
    
    // 创建 Skia 表面（SkSurface）
    GrDirectContext* skiaContext = impl_->context_->GetSkiaContext();
    if (!skiaContext) {
        std::cerr << "RenderSurface: Skia context is null" << std::endl;
        eglDestroySurface(display, impl_->eglSurface_);
        impl_->eglSurface_ = EGL_NO_SURFACE;
        return false;
    }
    
    // 获取默认的 framebuffer
    GrGLFramebufferInfo framebufferInfo;
    framebufferInfo.fFBOID = 0;
    
    // 获取颜色格式
    EGLint redSize, greenSize, blueSize, alphaSize;
    eglGetConfigAttrib(display, config, EGL_RED_SIZE, &redSize);
    eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &greenSize);
    eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blueSize);
    eglGetConfigAttrib(display, config, EGL_ALPHA_SIZE, &alphaSize);
    
    // 确定 Skia 颜色类型
    GrGLenum format = GL_RGBA8;
    if (alphaSize == 0) {
        format = GL_RGB8;
    }
    
    framebufferInfo.fFormat = format;
    
    // 创建 GrBackendRenderTarget
    GrBackendRenderTarget backendRenderTarget = GrBackendRenderTargets::MakeGL(
        impl_->width_, 
        impl_->height_,
        0,
        0,
        framebufferInfo
    );
    
    GrRecordingContext* recordingContext = reinterpret_cast<GrRecordingContext*>(skiaContext);
    impl_->skSurface_ = SkSurfaces::WrapBackendRenderTarget(
        recordingContext,
        backendRenderTarget,
        kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType,
        nullptr,
        nullptr
    );
    
    if (!impl_->skSurface_) {
        std::cerr << "RenderSurface: Failed to create Skia surface" << std::endl;
        eglDestroySurface(display, impl_->eglSurface_);
        impl_->eglSurface_ = EGL_NO_SURFACE;
        return false;
    }
    
    impl_->initialized_ = true;
    std::cout << "RenderSurface: Initialized successfully, size: " 
              << impl_->width_ << "x" << impl_->height_ << std::endl;
    
    return true;
}

bool RenderSurface::Destroy() {
    if (!impl_->initialized_) {
        return true;
    }
    
    // 释放 Skia 表面
    if (impl_->skSurface_) {
        impl_->skSurface_.reset();
    }
    
    // 获取 EGL 句柄
    auto nativeHandles = impl_->context_->GetNativeHandles();
    EGLDisplay display = static_cast<EGLDisplay>(nativeHandles.display);
    
    // 如果当前表面是活动的，先取消绑定
    if (display != EGL_NO_DISPLAY && impl_->eglSurface_ != EGL_NO_SURFACE) {
        EGLContext currentContext = eglGetCurrentContext();
        EGLSurface currentDraw = eglGetCurrentSurface(EGL_DRAW);
        EGLSurface currentRead = eglGetCurrentSurface(EGL_READ);
        
        // 如果当前表面是我们的，先取消绑定
        if (currentDraw == impl_->eglSurface_ || currentRead == impl_->eglSurface_) {
            eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }
        
        // 销毁 EGL 表面
        eglDestroySurface(display, impl_->eglSurface_);
        impl_->eglSurface_ = EGL_NO_SURFACE;
    }
    
    impl_->initialized_ = false;
    impl_->width_ = 0;
    impl_->height_ = 0;
    impl_->currentCanvas_ = nullptr;
    
    return true;
}

boost::optional<boost::reference_wrapper<SkCanvas>> RenderSurface::BeginFrame() {
    if (!impl_->initialized_) {
        std::cerr << "RenderSurface: Not initialized" << std::endl;
        return boost::none;
    }
    
    auto window = targetWindow_.lock();
    if (!window) {
        std::cerr << "RenderSurface: Window expired" << std::endl;
        return boost::none;
    }
    
    // 获取当前窗口大小（可能已经改变）
    GLFWwindow* glfwWindow = window->GetHandle();
    if (!glfwWindow) {
        return boost::none;
    }
    
    int newWidth, newHeight;
    glfwGetFramebufferSize(glfwWindow, &newWidth, &newHeight);
    
    // 如果窗口大小改变了，需要重新创建 Skia 表面
    if (newWidth != impl_->width_ || newHeight != impl_->height_) {
        if (newWidth > 0 && newHeight > 0) {
            impl_->width_ = newWidth;
            impl_->height_ = newHeight;
            
            // 重新创建 Skia 表面（EGL 表面会自动适应窗口大小）
            auto nativeHandles = impl_->context_->GetNativeHandles();
            EGLDisplay display = static_cast<EGLDisplay>(nativeHandles.display);
            EGLConfig config = static_cast<EGLConfig>(nativeHandles.config);
            EGLContext context = static_cast<EGLContext>(nativeHandles.context);
            
            // 确保上下文是当前的
            if (!eglMakeCurrent(display, impl_->eglSurface_, impl_->eglSurface_, context)) {
                std::cerr << "RenderSurface: Failed to make context current in BeginFrame" << std::endl;
                return boost::none;
            }
            
            // 重新创建 Skia 表面
            GrDirectContext* skiaContext = impl_->context_->GetSkiaContext();
            if (skiaContext) {
                GrGLFramebufferInfo framebufferInfo;
                framebufferInfo.fFBOID = 0;
                
                EGLint alphaSize;
                eglGetConfigAttrib(display, config, EGL_ALPHA_SIZE, &alphaSize);
                framebufferInfo.fFormat = (alphaSize > 0) ? GL_RGBA8 : GL_RGB8;
                
                GrBackendRenderTarget backendRenderTarget = GrBackendRenderTargets::MakeGL(
                    impl_->width_,
                    impl_->height_,
                    0,  // sample count
                    0,  // stencil bits
                    framebufferInfo
                );
                
                impl_->skSurface_ = SkSurfaces::WrapBackendRenderTarget(
                    reinterpret_cast<GrRecordingContext*>(skiaContext),  // 隐式转换为 GrRecordingContext*
                    backendRenderTarget,
                    kBottomLeft_GrSurfaceOrigin,
                    kRGBA_8888_SkColorType,
                    nullptr,  // 颜色空间
                    nullptr   // SurfaceProps
                );
                
                if (!impl_->skSurface_) {
                    std::cerr << "RenderSurface: Failed to recreate Skia surface after resize" << std::endl;
                    return boost::none;
                }
            }
        }
    }
    
    // 确保 EGL 上下文是当前的（上下文切换）
    // 注意：如果有多个窗口，每一帧都需要切换上下文
    auto nativeHandles = impl_->context_->GetNativeHandles();
    EGLDisplay display = static_cast<EGLDisplay>(nativeHandles.display);
    EGLContext context = static_cast<EGLContext>(nativeHandles.context);
    
    if (!eglMakeCurrent(display, impl_->eglSurface_, impl_->eglSurface_, context)) {
        EGLint error = eglGetError();
        std::cerr << "RenderSurface: Failed to make context current, error: 0x" 
                  << std::hex << error << std::dec << std::endl;
        return boost::none;
    }
    
    // 获取 Skia 画布
    SkCanvas* canvas = impl_->skSurface_->getCanvas();
    if (!canvas) {
        std::cerr << "RenderSurface: Failed to get Skia canvas" << std::endl;
        return boost::none;
    }
    
    // 保存当前画布引用（用于 EndFrame）
    impl_->currentCanvas_ = canvas;
    
    // 清除画布（准备新的一帧）
    canvas->clear(SK_ColorTRANSPARENT);
    
    return boost::make_optional(boost::ref(*canvas));
}

void RenderSurface::EndFrame() {
    if (!impl_->initialized_ || !impl_->currentCanvas_) {
        return; // 没有活动的帧
    }
    
    // 刷新 Skia 画布，确保所有绘制命令都提交到 GPU
    // 注意：SkSurface 没有 flush() 方法，应该使用 GrDirectContext::flush(SkSurface*)
    GrDirectContext* skiaContext = impl_->context_->GetSkiaContext();
    if (skiaContext && impl_->skSurface_) {
        skiaContext->flush(impl_->skSurface_.get());
    }
    
    // 获取 EGL 句柄
    auto nativeHandles = impl_->context_->GetNativeHandles();
    EGLDisplay display = static_cast<EGLDisplay>(nativeHandles.display);
    
    if (display == EGL_NO_DISPLAY || impl_->eglSurface_ == EGL_NO_SURFACE) {
        std::cerr << "RenderSurface: Invalid EGL handles in EndFrame" << std::endl;
        return;
    }
    
    // 双缓冲机制的核心：eglSwapBuffers
    // 这行代码将后台缓冲区（我们刚刚画完的）和前台缓冲区（用户看到的）交换
    // 用户永远看不到正在绘制的过程，只能看到画好的完整帧
    // 这比原生 GDI 绘图要平滑得多，避免了闪烁
    if (!eglSwapBuffers(display, impl_->eglSurface_)) {
        EGLint error = eglGetError();
        std::cerr << "RenderSurface: Failed to swap buffers, error: 0x" 
                  << std::hex << error << std::dec << std::endl;
    }
    
    // 清除当前画布引用
    impl_->currentCanvas_ = nullptr;
}

} // namespace graphics
} // namespace KiUI

