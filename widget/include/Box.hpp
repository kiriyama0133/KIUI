#ifndef BOX_HPP
#define BOX_HPP
#pragma once

#include "VisualElement.hpp"
#include <include/core/SkCanvas.h>

namespace KiUI {
namespace widget {

/**
 * @brief Box 组件，用于绘制矩形区域
 * 这是一个基础的容器组件，继承自 VisualElement，提供布局和渲染功能
 */
class Box : public VisualElement {
public:
    Box();
    virtual ~Box();
    
    /**
     * @brief 渲染 Box
     * @param canvas 画布
     */
    virtual void Render(SkCanvas* canvas) override;
    
    /**
     * @brief 命中测试
     * @param x 父级局部 x 坐标
     * @param y 父级局部 y 坐标
     * @return 被命中的最深层 VisualElement，如果没有命中则返回 nullptr
     */
    virtual boost::shared_ptr<VisualElement> HitTest(float x, float y) override;
};

} // namespace widget
} // namespace KiUI

#endif // BOX_HPP

