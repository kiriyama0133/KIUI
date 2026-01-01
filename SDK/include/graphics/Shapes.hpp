#ifndef SHAPES_HPP
#define SHAPES_HPP
#pragma once

#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <include/core/SkColor.h>
#include <include/core/SkRect.h>
#include <include/core/SkRRect.h>

namespace KiUI {
namespace graphics {

/**
 * @brief 绘制工具类，提供各种几何图形的绘制方法
 */
class Shapes {
public:
    /**
     * @brief 绘制矩形
     * @param canvas Skia 画布
     * @param x 左上角 x 坐标
     * @param y 左上角 y 坐标
     * @param width 宽度
     * @param height 高度
     * @param fillColor 填充颜色
     * @param strokeColor 边框颜色
     * @param strokeWidth 边框宽度
     * @param opacity 透明度 (0.0-1.0)
     */
    static void DrawRectangle(SkCanvas* canvas,
                             float x, float y, float width, float height,
                             SkColor fillColor = SK_ColorTRANSPARENT,
                             SkColor strokeColor = SK_ColorTRANSPARENT,
                             float strokeWidth = 0.0f,
                             float opacity = 1.0f);
    
    /**
     * @brief 绘制圆角矩形
     * @param canvas Skia 画布
     * @param x 左上角 x 坐标
     * @param y 左上角 y 坐标
     * @param width 宽度
     * @param height 高度
     * @param radius 圆角半径（统一）
     * @param fillColor 填充颜色
     * @param strokeColor 边框颜色
     * @param strokeWidth 边框宽度
     * @param opacity 透明度 (0.0-1.0)
     */
    static void DrawRoundedRectangle(SkCanvas* canvas,
                                    float x, float y, float width, float height,
                                    float radius,
                                    SkColor fillColor = SK_ColorTRANSPARENT,
                                    SkColor strokeColor = SK_ColorTRANSPARENT,
                                    float strokeWidth = 0.0f,
                                    float opacity = 1.0f);
    
    /**
     * @brief 绘制圆角矩形（支持不同圆角）
     * @param canvas Skia 画布
     * @param x 左上角 x 坐标
     * @param y 左上角 y 坐标
     * @param width 宽度
     * @param height 高度
     * @param topLeft 左上角半径
     * @param topRight 右上角半径
     * @param bottomRight 右下角半径
     * @param bottomLeft 左下角半径
     * @param fillColor 填充颜色
     * @param strokeColor 边框颜色
     * @param strokeWidth 边框宽度
     * @param opacity 透明度 (0.0-1.0)
     */
    static void DrawRoundedRectangle(SkCanvas* canvas,
                                    float x, float y, float width, float height,
                                    float topLeft, float topRight,
                                    float bottomRight, float bottomLeft,
                                    SkColor fillColor = SK_ColorTRANSPARENT,
                                    SkColor strokeColor = SK_ColorTRANSPARENT,
                                    float strokeWidth = 0.0f,
                                    float opacity = 1.0f);
    
    /**
     * @brief 绘制圆形
     * @param canvas Skia 画布
     * @param centerX 圆心 x 坐标
     * @param centerY 圆心 y 坐标
     * @param radius 半径
     * @param fillColor 填充颜色
     * @param strokeColor 边框颜色
     * @param strokeWidth 边框宽度
     * @param opacity 透明度 (0.0-1.0)
     */
    static void DrawCircle(SkCanvas* canvas,
                          float centerX, float centerY, float radius,
                          SkColor fillColor = SK_ColorTRANSPARENT,
                          SkColor strokeColor = SK_ColorTRANSPARENT,
                          float strokeWidth = 0.0f,
                          float opacity = 1.0f);
    
    /**
     * @brief 绘制椭圆
     * @param canvas Skia 画布
     * @param x 边界框左上角 x 坐标
     * @param y 边界框左上角 y 坐标
     * @param width 椭圆宽度
     * @param height 椭圆高度
     * @param fillColor 填充颜色
     * @param strokeColor 边框颜色
     * @param strokeWidth 边框宽度
     * @param opacity 透明度 (0.0-1.0)
     */
    static void DrawEllipse(SkCanvas* canvas,
                           float x, float y, float width, float height,
                           SkColor fillColor = SK_ColorTRANSPARENT,
                           SkColor strokeColor = SK_ColorTRANSPARENT,
                           float strokeWidth = 0.0f,
                           float opacity = 1.0f);
    
    /**
     * @brief 绘制线条
     * @param canvas Skia 画布
     * @param x1 起点 x 坐标
     * @param y1 起点 y 坐标
     * @param x2 终点 x 坐标
     * @param y2 终点 y 坐标
     * @param color 线条颜色
     * @param width 线条宽度
     * @param opacity 透明度 (0.0-1.0)
     */
    static void DrawLine(SkCanvas* canvas,
                        float x1, float y1, float x2, float y2,
                        SkColor color = SK_ColorBLACK,
                        float width = 1.0f,
                        float opacity = 1.0f);
};

} // namespace graphics
} // namespace KiUI

#endif // SHAPES_HPP

