#ifndef RENDER_CONTEXT_HPP
#define RENDER_CONTEXT_HPP
#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

class GrDirectContext;
namespace KiUI {
namespace foundation {
    class Window;
}

namespace graphics {

/**
 * @brief RenderContext: 渲染上下文管理类
 * 负责管理图形渲染相关的资源，包括渲染上下文创建、销毁和状态管理
 */
class RenderContext : boost::noncopyable {
public:
    RenderContext();
    ~RenderContext();
    
    /**
     * @brief 初始化渲染上下文
     * @return 是否成功初始化
     */
    bool Initialize();
    /**
     * @brief 资源释放
     */
    void Shutdown();

    /**
     * @brief 获取 Skia 的 GPU 上下文。
     * 所有的渲染表面（RenderSurface）都需要通过这个上下文来创建画布。
     * @return Skia GPU 上下文指针，如果未初始化则返回 nullptr
     */
    GrDirectContext* GetSkiaContext() const;
    
    /**
     * @brief 检查渲染上下文是否已初始化
     * @return 如果已初始化返回 true，否则返回 false
     */
    bool IsInitialized() const;
    
    // 内部接口：仅供 RenderSurface 调用，用于 EGL 握手
    struct NativeHandles {
        void* display;
        void* context;
        void* config;
    };
    
    /**
     * @brief 获取原生 EGL 句柄（仅供 RenderSurface 使用）
     * @return NativeHandles 结构体，包含 display、context 和 config
     */
    NativeHandles GetNativeHandles() const;

private:
    struct Impl;
    boost::scoped_ptr<Impl> impl_;
};

} // namespace graphics
} // namespace KiUI

#endif // RENDER_CONTEXT_HPP

