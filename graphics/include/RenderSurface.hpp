#ifndef RENDER_SURFACE_HPP
#define RENDER_SURFACE_HPP
#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include "RenderContext.hpp"
#include "window.hpp"

class SkCanvas;

namespace KiUI {
namespace graphics {

class RenderSurface : boost::noncopyable {
public:
    /**
     * @brief 构造函数
     * @param context 渲染上下文
     * @param window 窗口
     */
    RenderSurface(boost::shared_ptr<RenderContext> context, boost::shared_ptr<foundation::Window> window);
    ~RenderSurface();  // 需要在 .cpp 中实现，因为 Impl 是前向声明
    bool Initialize(); // 生命周期  
    /**
     * @brief 销毁渲染表面
     */
    bool Destroy();
    /**
     * @brief 开始绘制一帧
     * 确保画布大小正确，切换上下文，返回 Skia 画布供绘制
     * @return Skia 画布引用，如果失败返回 boost::none
     */
    boost::optional<boost::reference_wrapper<SkCanvas>> BeginFrame();
    
    /**
     * @brief 结束绘制一帧并显示
     * 执行双缓冲交换（eglSwapBuffers），将后台缓冲区的内容推送到屏幕
     * 这是"翻页显示"的关键步骤，保证用户看不到绘制过程，只看到完整帧
     */
    void EndFrame();

private:
    struct Impl;
    boost::scoped_ptr<Impl> impl_;
    boost::weak_ptr<foundation::Window> targetWindow_;
};

} // namespace graphics
} // namespace KiUI
#endif