#include "Shapes.hpp"
#include <include/core/SkRRect.h>
#include <algorithm>

namespace KiUI {
namespace graphics {

void Shapes::DrawRectangle(SkCanvas* canvas,
                          float x, float y, float width, float height,
                          SkColor fillColor,
                          SkColor strokeColor,
                          float strokeWidth,
                          float opacity) {
    if (!canvas || width <= 0.0f || height <= 0.0f) {
        return;
    }
    
    SkRect rect = SkRect::MakeXYWH(x, y, width, height);
    
    // Draw fill
    if (fillColor != SK_ColorTRANSPARENT) {
        SkPaint fillPaint;
        fillPaint.setColor(SkColorSetA(fillColor, static_cast<U8CPU>(opacity * 255)));
        fillPaint.setStyle(SkPaint::kFill_Style);
        fillPaint.setAntiAlias(true);
        canvas->drawRect(rect, fillPaint);
    }
    
    // Draw stroke
    if (strokeColor != SK_ColorTRANSPARENT && strokeWidth > 0.0f) {
        SkPaint strokePaint;
        strokePaint.setColor(SkColorSetA(strokeColor, static_cast<U8CPU>(opacity * 255)));
        strokePaint.setStyle(SkPaint::kStroke_Style);
        strokePaint.setStrokeWidth(strokeWidth);
        strokePaint.setAntiAlias(true);
        canvas->drawRect(rect, strokePaint);
    }
}

void Shapes::DrawRoundedRectangle(SkCanvas* canvas,
                                  float x, float y, float width, float height,
                                  float radius,
                                  SkColor fillColor,
                                  SkColor strokeColor,
                                  float strokeWidth,
                                  float opacity) {
    DrawRoundedRectangle(canvas, x, y, width, height,
                        radius, radius, radius, radius,
                        fillColor, strokeColor, strokeWidth, opacity);
}

void Shapes::DrawRoundedRectangle(SkCanvas* canvas,
                                  float x, float y, float width, float height,
                                  float topLeft, float topRight,
                                  float bottomRight, float bottomLeft,
                                  SkColor fillColor,
                                  SkColor strokeColor,
                                  float strokeWidth,
                                  float opacity) {
    if (!canvas || width <= 0.0f || height <= 0.0f) {
        return;
    }
    
    SkRect rect = SkRect::MakeXYWH(x, y, width, height);
    
    // Clamp radii to half the size of the rect
    float maxRadius = std::min(width, height) / 2.0f;
    topLeft = std::min(topLeft, maxRadius);
    topRight = std::min(topRight, maxRadius);
    bottomRight = std::min(bottomRight, maxRadius);
    bottomLeft = std::min(bottomLeft, maxRadius);
    
    SkRRect rrect;
    rrect.setRectXY(rect, topLeft, topLeft);
    
    // For different radii, we need to set them individually
    if (topLeft != topRight || topRight != bottomRight || bottomRight != bottomLeft) {
        SkVector radii[4] = {
            {topLeft, topLeft},
            {topRight, topRight},
            {bottomRight, bottomRight},
            {bottomLeft, bottomLeft}
        };
        rrect.setRectRadii(rect, radii);
    }
    
    // Draw fill
    if (fillColor != SK_ColorTRANSPARENT) {
        SkPaint fillPaint;
        fillPaint.setColor(SkColorSetA(fillColor, static_cast<U8CPU>(opacity * 255)));
        fillPaint.setStyle(SkPaint::kFill_Style);
        fillPaint.setAntiAlias(true);
        canvas->drawRRect(rrect, fillPaint);
    }
    
    // Draw stroke
    if (strokeColor != SK_ColorTRANSPARENT && strokeWidth > 0.0f) {
        SkPaint strokePaint;
        strokePaint.setColor(SkColorSetA(strokeColor, static_cast<U8CPU>(opacity * 255)));
        strokePaint.setStyle(SkPaint::kStroke_Style);
        strokePaint.setStrokeWidth(strokeWidth);
        strokePaint.setAntiAlias(true);
        canvas->drawRRect(rrect, strokePaint);
    }
}

void Shapes::DrawCircle(SkCanvas* canvas,
                       float centerX, float centerY, float radius,
                       SkColor fillColor,
                       SkColor strokeColor,
                       float strokeWidth,
                       float opacity) {
    if (!canvas || radius <= 0.0f) {
        return;
    }
    
    // Draw fill
    if (fillColor != SK_ColorTRANSPARENT) {
        SkPaint fillPaint;
        fillPaint.setColor(SkColorSetA(fillColor, static_cast<U8CPU>(opacity * 255)));
        fillPaint.setStyle(SkPaint::kFill_Style);
        fillPaint.setAntiAlias(true);
        canvas->drawCircle(centerX, centerY, radius, fillPaint);
    }
    
    // Draw stroke
    if (strokeColor != SK_ColorTRANSPARENT && strokeWidth > 0.0f) {
        SkPaint strokePaint;
        strokePaint.setColor(SkColorSetA(strokeColor, static_cast<U8CPU>(opacity * 255)));
        strokePaint.setStyle(SkPaint::kStroke_Style);
        strokePaint.setStrokeWidth(strokeWidth);
        strokePaint.setAntiAlias(true);
        canvas->drawCircle(centerX, centerY, radius, strokePaint);
    }
}

void Shapes::DrawEllipse(SkCanvas* canvas,
                        float x, float y, float width, float height,
                        SkColor fillColor,
                        SkColor strokeColor,
                        float strokeWidth,
                        float opacity) {
    if (!canvas || width <= 0.0f || height <= 0.0f) {
        return;
    }
    
    SkRect rect = SkRect::MakeXYWH(x, y, width, height);
    
    // Draw fill
    if (fillColor != SK_ColorTRANSPARENT) {
        SkPaint fillPaint;
        fillPaint.setColor(SkColorSetA(fillColor, static_cast<U8CPU>(opacity * 255)));
        fillPaint.setStyle(SkPaint::kFill_Style);
        fillPaint.setAntiAlias(true);
        canvas->drawOval(rect, fillPaint);
    }
    
    // Draw stroke
    if (strokeColor != SK_ColorTRANSPARENT && strokeWidth > 0.0f) {
        SkPaint strokePaint;
        strokePaint.setColor(SkColorSetA(strokeColor, static_cast<U8CPU>(opacity * 255)));
        strokePaint.setStyle(SkPaint::kStroke_Style);
        strokePaint.setStrokeWidth(strokeWidth);
        strokePaint.setAntiAlias(true);
        canvas->drawOval(rect, strokePaint);
    }
}

void Shapes::DrawLine(SkCanvas* canvas,
                     float x1, float y1, float x2, float y2,
                     SkColor color,
                     float width,
                     float opacity) {
    if (!canvas) {
        return;
    }
    
    SkPaint paint;
    paint.setColor(SkColorSetA(color, static_cast<U8CPU>(opacity * 255)));
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(width);
    paint.setAntiAlias(true);
    
    canvas->drawLine(x1, y1, x2, y2, paint);
}

} // namespace graphics
} // namespace KiUI

