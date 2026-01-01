#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP
#pragma once

#include <include/core/SkCanvas.h>
#include <include/core/SkColor.h>

namespace KiUI {
namespace graphics {

/**
 * @brief 矩形绘制类，提供矩形的绘制功能
 */
class Rectangle {
public:
    /**
     * @brief 构造函数
     * @param x 左上角 x 坐标
     * @param y 左上角 y 坐标
     * @param width 宽度
     * @param height 高度
     */
    Rectangle(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f);
    
    /**
     * @brief 析构函数
     */
    ~Rectangle();
    
    /**
     * @brief 绘制矩形
     * @param canvas Skia 画布
     */
    void Draw(SkCanvas* canvas) const;
    
    // Getters and Setters
    void SetX(float x) { x_ = x; }
    float GetX() const { return x_; }
    
    void SetY(float y) { y_ = y; }
    float GetY() const { return y_; }
    
    void SetWidth(float width) { width_ = width; }
    float GetWidth() const { return width_; }
    
    void SetHeight(float height) { height_ = height; }
    float GetHeight() const { return height_; }
    
    void SetFillColor(SkColor color) { fillColor_ = color; }
    SkColor GetFillColor() const { return fillColor_; }
    
    void SetStrokeColor(SkColor color) { strokeColor_ = color; }
    SkColor GetStrokeColor() const { return strokeColor_; }
    
    void SetStrokeWidth(float width) { strokeWidth_ = width; }
    float GetStrokeWidth() const { return strokeWidth_; }
    
    void SetOpacity(float opacity) { opacity_ = opacity; }
    float GetOpacity() const { return opacity_; }

private:
    float x_;
    float y_;
    float width_;
    float height_;
    SkColor fillColor_ = SK_ColorTRANSPARENT;
    SkColor strokeColor_ = SK_ColorTRANSPARENT;
    float strokeWidth_ = 0.0f;
    float opacity_ = 1.0f;
};

} // namespace graphics
} // namespace KiUI

#endif // RECTANGLE_HPP

