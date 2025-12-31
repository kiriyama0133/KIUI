// 禁用 Windows 的 min/max 宏，避免与 C++ 标准库和 Skia 冲突
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "RenderContext.hpp"
#include "EGL/eglplatform.h"
#include <iostream>
#include <vector>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglext_angle.h>  // ANGLE 扩展头文件，包含 EGL_PLATFORM_ANGLE_TYPE_* 宏
#include <GLES3/gl3.h>
#include <core/SkRefCnt.h>  // Skia 引用计数，包含 sk_sp 模板
#include <gpu/ganesh/GrDirectContext.h>
#include <gpu/ganesh/gl/GrGLDirectContext.h>  // GrDirectContexts::MakeGL
#include <gpu/ganesh/gl/GrGLInterface.h>
#include <gpu/ganesh/gl/GrGLAssembleInterface.h>

namespace KiUI {
namespace graphics {
    struct RenderContext::Impl {
        EGLDisplay display_ = EGL_NO_DISPLAY; // EGL display
        EGLConfig config_ = nullptr; // EGL config
        EGLContext context_ = EGL_NO_CONTEXT; // EGL context
        sk_sp<GrDirectContext> skiaContext_ = nullptr;
    };  

    RenderContext::RenderContext() : impl_(new Impl()) {}
    RenderContext::~RenderContext() {
        Shutdown();
    }

    bool RenderContext::Initialize() {
        auto eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
        EGLint backendType = EGL_PLATFORM_ANGLE_TYPE_DEFAULT_ANGLE;
        bool enableAutomaticTrim = false;
        
        #if defined(_WIN32)
            backendType = EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE;
            enableAutomaticTrim = true;  // 只在 D3D11 后端启用自动 trim
        #elif defined(__APPLE__)
            backendType = EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE;
        #elif defined(__linux__)
            backendType = EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE;
        #endif
        
        // 构建显示属性数组
        std::vector<EGLint> displayAttributes;
        displayAttributes.push_back(EGL_PLATFORM_ANGLE_TYPE_ANGLE);
        displayAttributes.push_back(backendType);
        
        // 只在 D3D11 后端启用自动 trim（这是 ANGLE 的要求）
        if (enableAutomaticTrim) {
            displayAttributes.push_back(EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE);
            displayAttributes.push_back(EGL_TRUE);
        }
        
        displayAttributes.push_back(EGL_NONE);

        impl_->display_ = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, 
            EGL_DEFAULT_DISPLAY, 
            displayAttributes.data());

        if (impl_->display_ == EGL_NO_DISPLAY) return false;

        // Initialize EGL
        EGLint major, minor;
        if (!eglInitialize(impl_->display_, &major, &minor)) {
            std::cerr << "RenderContext: Failed to initialize EGL" << std::endl;
            impl_->display_ = EGL_NO_DISPLAY;
            return false;
        }
        
        std::cout << "RenderContext: EGL initialized, version " << major << "." << minor << std::endl;
        
        // Configure surface attributes
        const EGLint configAttributes[] = {
            EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 8, EGL_STENCIL_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_NONE
        };

        // Choose EGL configuration
        EGLint numConfigs;
        if (!eglChooseConfig(impl_->display_, configAttributes, &impl_->config_, 1, &numConfigs) || numConfigs == 0) {
            std::cerr << "RenderContext: Failed to choose EGL config" << std::endl;
            eglTerminate(impl_->display_);
            impl_->display_ = EGL_NO_DISPLAY;
            return false;
        }

        // Bind OpenGL ES API
        if (!eglBindAPI(EGL_OPENGL_ES_API)) {
            std::cerr << "RenderContext: Failed to bind OpenGL ES API" << std::endl;
            eglTerminate(impl_->display_);
            impl_->display_ = EGL_NO_DISPLAY;
            return false;
        }
        
        // Create EGL context
        const EGLint contextAttributes[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3, // 使用 OpenGL ES 3.0
            EGL_NONE
        };
        impl_->context_ = eglCreateContext(impl_->display_, impl_->config_, EGL_NO_CONTEXT, contextAttributes);
        if (impl_->context_ == EGL_NO_CONTEXT) {
            EGLint error = eglGetError();
            std::cerr << "RenderContext: Failed to create EGL context, error: 0x" 
                      << std::hex << error << std::dec << std::endl;
            eglTerminate(impl_->display_);
            impl_->display_ = EGL_NO_DISPLAY;
            return false;
        }

        // Make context current (required before creating Skia context)
        if (!eglMakeCurrent(impl_->display_, EGL_NO_SURFACE, EGL_NO_SURFACE, impl_->context_)) {
            EGLint error = eglGetError();
            std::cerr << "RenderContext: Failed to make EGL context current, error: 0x" 
                      << std::hex << error << std::dec << std::endl;
            eglDestroyContext(impl_->display_, impl_->context_);
            impl_->context_ = EGL_NO_CONTEXT;
            eglTerminate(impl_->display_);
            impl_->display_ = EGL_NO_DISPLAY;
            return false;
        }

        // connect the context to the skia context
        auto interface = GrGLMakeAssembledInterface(nullptr, [](void* ctx, const char name[]) {
            return eglGetProcAddress(name);
        });
        
        if (!interface) {
            std::cerr << "RenderContext: Failed to create GL interface" << std::endl;
            return false;
        }
        
        impl_->skiaContext_ = GrDirectContexts::MakeGL(interface);
        if (!impl_->skiaContext_) {
            std::cerr << "RenderContext: Failed to create Skia GL context" << std::endl;
            return false;
        }
        
        return true;
    }

    void RenderContext::Shutdown() {
        if (!impl_) return;
        
        // Release Skia context first
        if (impl_->skiaContext_) {
            impl_->skiaContext_.reset();
        }
        
        // Make context not current
        if (impl_->display_ != EGL_NO_DISPLAY) {
            eglMakeCurrent(impl_->display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }
        
        // Destroy EGL context
        if (impl_->context_ != EGL_NO_CONTEXT && impl_->display_ != EGL_NO_DISPLAY) {
            eglDestroyContext(impl_->display_, impl_->context_);
            impl_->context_ = EGL_NO_CONTEXT;
        }
        
        // Terminate EGL display
        if (impl_->display_ != EGL_NO_DISPLAY) {
            eglTerminate(impl_->display_);
            impl_->display_ = EGL_NO_DISPLAY;
        }
        
        impl_->config_ = nullptr;
    }

    GrDirectContext* RenderContext::GetSkiaContext() const {
        if (!impl_) return nullptr;
        return impl_->skiaContext_.get();
    }

    bool RenderContext::IsInitialized() const {
        if (!impl_) return false;
        return impl_->display_ != EGL_NO_DISPLAY && 
               impl_->context_ != EGL_NO_CONTEXT && 
               impl_->skiaContext_ != nullptr;
    }

    RenderContext::NativeHandles RenderContext::GetNativeHandles() const {
        RenderContext::NativeHandles handles;
        handles.display = nullptr;
        handles.context = nullptr;
        handles.config = nullptr;
        
        if (impl_) {
            handles.display = impl_->display_;
            handles.context = impl_->context_;
            handles.config = impl_->config_;
        }
        return handles;
    }

} // namespace graphics
} // namespace KiUI
