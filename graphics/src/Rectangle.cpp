#include "Rectangle.hpp"
#include "Shapes.hpp"

namespace KiUI {
namespace graphics {

Rectangle::Rectangle(float x, float y, float width, float height)
    : x_(x), y_(y), width_(width), height_(height) {
}

Rectangle::~Rectangle() {
}

void Rectangle::Draw(SkCanvas* canvas) const {
    if (!canvas || width_ <= 0.0f || height_ <= 0.0f) {
        return;
    }
    
    Shapes::DrawRectangle(
        canvas,
        x_, y_, width_, height_,
        fillColor_,
        strokeColor_,
        strokeWidth_,
        opacity_
    );
}

} // namespace graphics
} // namespace KiUI

