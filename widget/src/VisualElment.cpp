#include "VisualElement.hpp"

namespace KiUI {
namespace widget {

VisualElement::VisualElement() {
}

VisualElement::~VisualElement() {
}

void VisualElement::SetOpacity(float opacity) {
    opacity_ = opacity;
}

void VisualElement::SetVisibility(bool visible) {
    visible_ = visible;
}

void VisualElement::SetTransform(const SkMatrix& matrix) {
    transform_ = matrix;
}

void VisualElement::SetWidth(float width) {
    width_ = width;
}

void VisualElement::SetHeight(float height) {
    height_ = height;
}

// Margin methods
void VisualElement::SetMargin(Margin edge, float margin) {
    switch (edge) {
        case Margin::Top:
            marginTop_ = margin;
            break;
        case Margin::Bottom:
            marginBottom_ = margin;
            break;
        case Margin::Left:
            marginLeft_ = margin;
            break;
        case Margin::Right:
            marginRight_ = margin;
            break;
        case Margin::All:
            marginTop_ = marginBottom_ = marginLeft_ = marginRight_ = margin;
            break;
    }
}

float VisualElement::GetMargin(Margin edge) const {
    switch (edge) {
        case Margin::Top:
            return marginTop_;
        case Margin::Bottom:
            return marginBottom_;
        case Margin::Left:
            return marginLeft_;
        case Margin::Right:
            return marginRight_;
        case Margin::All:
        default:
            return 0.0f;
    }
}

// Padding methods
void VisualElement::SetPadding(Padding edge, float padding) {
    switch (edge) {
        case Padding::Top:
            paddingTop_ = padding;
            break;
        case Padding::Bottom:
            paddingBottom_ = padding;
            break;
        case Padding::Left:
            paddingLeft_ = padding;
            break;
        case Padding::Right:
            paddingRight_ = padding;
            break;
        case Padding::All:
            paddingTop_ = paddingBottom_ = paddingLeft_ = paddingRight_ = padding;
            break;
    }
}

float VisualElement::GetPadding(Padding edge) const {
    switch (edge) {
        case Padding::Top:
            return paddingTop_;
        case Padding::Bottom:
            return paddingBottom_;
        case Padding::Left:
            return paddingLeft_;
        case Padding::Right:
            return paddingRight_;
        case Padding::All:
        default:
            return 0.0f;
    }
}

// Border width methods
void VisualElement::SetBorderWidth(BorderWidth edge, float width) {
    switch (edge) {
        case BorderWidth::Top:
            borderWidthTop_ = width;
            break;
        case BorderWidth::Bottom:
            borderWidthBottom_ = width;
            break;
        case BorderWidth::Left:
            borderWidthLeft_ = width;
            break;
        case BorderWidth::Right:
            borderWidthRight_ = width;
            break;
        case BorderWidth::All:
            borderWidthTop_ = borderWidthBottom_ = borderWidthLeft_ = borderWidthRight_ = width;
            break;
    }
}

float VisualElement::GetBorderWidth(BorderWidth edge) const {
    switch (edge) {
        case BorderWidth::Top:
            return borderWidthTop_;
        case BorderWidth::Bottom:
            return borderWidthBottom_;
        case BorderWidth::Left:
            return borderWidthLeft_;
        case BorderWidth::Right:
            return borderWidthRight_;
        case BorderWidth::All:
        default:
            return 0.0f;
    }
}

// Border color methods
void VisualElement::SetBorderColor(SkColor color) {
    borderColor_ = color;
}

// Border radius methods
void VisualElement::SetBorderRadius(BorderRadius corner, float radius) {
    switch (corner) {
        case BorderRadius::TopLeft:
            borderRadiusTopLeft_ = radius;
            break;
        case BorderRadius::TopRight:
            borderRadiusTopRight_ = radius;
            break;
        case BorderRadius::BottomLeft:
            borderRadiusBottomLeft_ = radius;
            break;
        case BorderRadius::BottomRight:
            borderRadiusBottomRight_ = radius;
            break;
        case BorderRadius::All:
            borderRadiusTopLeft_ = borderRadiusTopRight_ = borderRadiusBottomLeft_ = borderRadiusBottomRight_ = radius;
            break;
    }
}

float VisualElement::GetBorderRadius(BorderRadius corner) const {
    switch (corner) {
        case BorderRadius::TopLeft:
            return borderRadiusTopLeft_;
        case BorderRadius::TopRight:
            return borderRadiusTopRight_;
        case BorderRadius::BottomLeft:
            return borderRadiusBottomLeft_;
        case BorderRadius::BottomRight:
            return borderRadiusBottomRight_;
        case BorderRadius::All:
        default:
            return 0.0f;
    }
}

// Background color methods
void VisualElement::SetBackgroundColor(SkColor color) {
    backgroundColor_ = color;
}

// Foreground color methods
void VisualElement::SetForegroundColor(SkColor color) {
    foregroundColor_ = color;
}

} // namespace widget
} // namespace KiUI