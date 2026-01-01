#include "Box.hpp"
#include <Shapes.hpp>

namespace KiUI {
namespace widget {

Box::Box() {
}

Box::~Box() {
}

void Box::Render(SkCanvas* canvas) {
    if (!canvas || !GetVisibility()) {
        return;
    }
    
    // Save canvas state
    canvas->save();
    
    // Apply transform if any
    if (!transform_.isIdentity()) {
        canvas->concat(transform_);
    }
    
    // Calculate border width (use average if different sides have different widths)
    float avgBorderWidth = 0.0f;
    bool hasBorder = false;
    if (borderWidthTop_ > 0.0f || borderWidthBottom_ > 0.0f || 
        borderWidthLeft_ > 0.0f || borderWidthRight_ > 0.0f) {
        hasBorder = true;
        avgBorderWidth = (borderWidthTop_ + borderWidthBottom_ + 
                         borderWidthLeft_ + borderWidthRight_) / 4.0f;
    }
    
    // Check if we have border radius
    bool hasBorderRadius = (borderRadiusTopLeft_ > 0.0f || borderRadiusTopRight_ > 0.0f || 
                           borderRadiusBottomLeft_ > 0.0f || borderRadiusBottomRight_ > 0.0f);
    
    // Use graphics layer's Shapes class for drawing
    if (hasBorderRadius) {
        // Draw rounded rectangle with different corner radii
        ::KiUI::graphics::Shapes::DrawRoundedRectangle(
            canvas,
            0.0f, 0.0f, width_, height_,
            borderRadiusTopLeft_, borderRadiusTopRight_,
            borderRadiusBottomRight_, borderRadiusBottomLeft_,
            backgroundColor_,
            hasBorder ? borderColor_ : SK_ColorTRANSPARENT,
            hasBorder ? avgBorderWidth : 0.0f,
            opacity_
        );
    } else {
        // Draw regular rectangle
        ::KiUI::graphics::Shapes::DrawRectangle(
            canvas,
            0.0f, 0.0f, width_, height_,
            backgroundColor_,
            hasBorder ? borderColor_ : SK_ColorTRANSPARENT,
            hasBorder ? avgBorderWidth : 0.0f,
            opacity_
        );
    }
    
    // Restore canvas state
    canvas->restore();
}

boost::shared_ptr<VisualElement> Box::HitTest(float x, float y) {
    // Call parent's HitTest which handles coordinate transformation and recursion
    return VisualElement::HitTest(x, y);
}

} // namespace widget
} // namespace KiUI

