#ifndef RENDER_CONTEXT_HPP
#define RENDER_CONTEXT_HPP
#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

namespace KiUI {
namespace foundation {
    class Window;
}

namespace graphics {

/**
 * @brief RenderContext: 渲染上下文管理类
 * 负责管理图形渲染相关的资源，包括渲染上下文创建、销毁和状态管理
 */
class RenderContext {
public:
    /**
     * @brief 构造函数
     * @param window 关联的窗口对象
     */
    explicit RenderContext(boost::shared_ptr<foundation::Window> window);
    
    /**
     * @brief 析构函数
     */
    ~RenderContext();
    
    /**
     * @brief 初始化渲染上下文
     * @return 是否成功初始化
     */
    bool Initialize();
    
    /**
     * @brief 销毁渲染上下文
     */
    void Shutdown();
    
    /**
     * @brief 使渲染上下文成为当前上下文
     * @return 是否成功
     */
    bool MakeCurrent();
    
    /**
     * @brief 交换前后缓冲区
     */
    void SwapBuffers();
    
    /**
     * @brief 获取关联的窗口
     * @return 窗口对象的共享指针
     */
    boost::shared_ptr<foundation::Window> GetWindow() const { return window_; }
    
    /**
     * @brief 检查渲染上下文是否已初始化
     * @return 是否已初始化
     */
    bool IsInitialized() const { return initialized_; }
    
    /**
     * @brief 上下文创建完成信号
     */
    boost::signals2::signal<void()> OnContextCreated;
    
    /**
     * @brief 上下文销毁信号
     */
    boost::signals2::signal<void()> OnContextDestroyed;

private:
    boost::shared_ptr<foundation::Window> window_;
    bool initialized_;
    void* context_handle_;  // 平台相关的上下文句柄（EGLContext, HGLRC等）
    
    // 禁止拷贝和赋值
    RenderContext(const RenderContext&) = delete;
    RenderContext& operator=(const RenderContext&) = delete;
};

} // namespace graphics
} // namespace KiUI

#endif // RENDER_CONTEXT_HPP

