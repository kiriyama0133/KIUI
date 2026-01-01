#include "VisualElement.hpp"
#include <logger.hpp>
#include <vector>
#include "logger.hpp"
namespace KiUI {
namespace widget {

VisualElement::VisualElement() {
    // Create Yoga node
    yogaNode_ = YGNodeNew();
    if (yogaNode_) {
        // Set default layout properties
        YGNodeStyleSetFlexDirection(yogaNode_, YGFlexDirectionColumn);
    }
}

VisualElement::~VisualElement() {
    // Free Yoga node
    if (yogaNode_) {
        YGNodeFree(yogaNode_);
        yogaNode_ = nullptr;
    }
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
    UpdateYogaNode();
}

void VisualElement::SetHeight(float height) {
    height_ = height;
    UpdateYogaNode();
}

void VisualElement::SetLeft(float left) {
    left_ = left;
}

void VisualElement::SetTop(float top) {
    top_ = top;
}

void VisualElement::SetAlignment(Alignment alignment) {
    alignment_ = alignment;
    UpdateYogaNode();
}

void VisualElement::SetJustification(Justification justification) {
    justification_ = justification;
    UpdateYogaNode();
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
    UpdateYogaNode();
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
    UpdateYogaNode();
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

// Override AddChild and RemoveChild to manage Yoga node tree
void VisualElement::AddChild(boost::shared_ptr<UIElement> child) {
    // Call parent's AddChild first
    UIElement::AddChild(child);
    
    // Add child's Yoga node to this node's Yoga tree
    auto visualChild = boost::dynamic_pointer_cast<VisualElement>(child);
    if (visualChild && visualChild->yogaNode_ && yogaNode_) {
        YGNodeInsertChild(yogaNode_, visualChild->yogaNode_, YGNodeGetChildCount(yogaNode_));
        visualChild->UpdateYogaNode();
    }
}

void VisualElement::RemoveChild(boost::shared_ptr<UIElement> child) {
    // Remove child's Yoga node from this node's Yoga tree
    auto visualChild = boost::dynamic_pointer_cast<VisualElement>(child);
    if (visualChild && visualChild->yogaNode_ && yogaNode_) {
        YGNodeRemoveChild(yogaNode_, visualChild->yogaNode_);
    }
    
    // Call parent's RemoveChild
    UIElement::RemoveChild(child);
}

// Layout methods
void VisualElement::UpdateYogaNode() {
    if (!yogaNode_) {
        return;
    }
    
    // Set width and height
    if (width_ > 0.0f) {
        YGNodeStyleSetWidth(yogaNode_, width_);
    } else {
        YGNodeStyleSetWidthAuto(yogaNode_);
    }
    
    if (height_ > 0.0f) {
        YGNodeStyleSetHeight(yogaNode_, height_);
    } else {
        YGNodeStyleSetHeightAuto(yogaNode_);
    }
    
    // Set padding
    YGNodeStyleSetPadding(yogaNode_, YGEdgeTop, paddingTop_);
    YGNodeStyleSetPadding(yogaNode_, YGEdgeBottom, paddingBottom_);
    YGNodeStyleSetPadding(yogaNode_, YGEdgeLeft, paddingLeft_);
    YGNodeStyleSetPadding(yogaNode_, YGEdgeRight, paddingRight_);
    
    // Check if this element has a parent (is a child element)
    auto parent = GetParent();
    bool isChild = (parent != nullptr);
    
    if (isChild) {
        // For child elements: use alignSelf to override parent's cross-axis alignment
        // In Column layout: cross-axis is horizontal (left/right)
        YGAlign yogaAlignSelf = YGAlignAuto; // Auto means inherit from parent
        switch (alignment_) {
            case Alignment::Start:
                yogaAlignSelf = YGAlignFlexStart;
                break;
            case Alignment::Center:
                yogaAlignSelf = YGAlignCenter;
                break;
            case Alignment::End:
                yogaAlignSelf = YGAlignFlexEnd;
                break;
            case Alignment::Stretch:
                yogaAlignSelf = YGAlignStretch;
                break;
        }
        YGNodeStyleSetAlignSelf(yogaNode_, yogaAlignSelf);
        
        // Handle vertical positioning (justification/main-axis)
        switch (justification_) {
            case Justification::Start:
                // Start: use explicit top margin, bottom margin
                YGNodeStyleSetMargin(yogaNode_, YGEdgeTop, marginTop_);
                YGNodeStyleSetMargin(yogaNode_, YGEdgeBottom, marginBottom_);
                break;
            case Justification::Center:
                // Center vertically: both margins auto
                YGNodeStyleSetMarginAuto(yogaNode_, YGEdgeTop);
                YGNodeStyleSetMarginAuto(yogaNode_, YGEdgeBottom);
                break;
            case Justification::End:
                // End vertically: top margin auto pushes to bottom
                YGNodeStyleSetMarginAuto(yogaNode_, YGEdgeTop);
                YGNodeStyleSetMargin(yogaNode_, YGEdgeBottom, marginBottom_);
                break;
            case Justification::Stretch:
                // Stretch: use explicit margins
                YGNodeStyleSetMargin(yogaNode_, YGEdgeTop, marginTop_);
                YGNodeStyleSetMargin(yogaNode_, YGEdgeBottom, marginBottom_);
                break;
        }
        
        // Handle horizontal positioning (alignment/cross-axis)
        switch (alignment_) {
            case Alignment::Start:
                // Start: use explicit margins
                YGNodeStyleSetMargin(yogaNode_, YGEdgeLeft, marginLeft_);
                YGNodeStyleSetMargin(yogaNode_, YGEdgeRight, marginRight_);
                break;
            case Alignment::Center:
                // Center horizontally: both margins auto
                YGNodeStyleSetMarginAuto(yogaNode_, YGEdgeLeft);
                YGNodeStyleSetMarginAuto(yogaNode_, YGEdgeRight);
                break;
            case Alignment::End:
                // End horizontally: left margin auto pushes to right
                YGNodeStyleSetMarginAuto(yogaNode_, YGEdgeLeft);
                YGNodeStyleSetMargin(yogaNode_, YGEdgeRight, marginRight_);
                break;
            case Alignment::Stretch:
                // Stretch: handled by alignSelf, use explicit margins
                YGNodeStyleSetMargin(yogaNode_, YGEdgeLeft, marginLeft_);
                YGNodeStyleSetMargin(yogaNode_, YGEdgeRight, marginRight_);
                break;
        }
    } else {
        // For root/parent elements: set margins normally
        YGNodeStyleSetMargin(yogaNode_, YGEdgeTop, marginTop_);
        YGNodeStyleSetMargin(yogaNode_, YGEdgeBottom, marginBottom_);
        YGNodeStyleSetMargin(yogaNode_, YGEdgeLeft, marginLeft_);
        YGNodeStyleSetMargin(yogaNode_, YGEdgeRight, marginRight_);
        
        // Set alignItems and justifyContent for children
        YGAlign yogaAlign = YGAlignStretch;
        switch (alignment_) {
            case Alignment::Start:
                yogaAlign = YGAlignFlexStart;
                break;
            case Alignment::Center:
                yogaAlign = YGAlignCenter;
                break;
            case Alignment::End:
                yogaAlign = YGAlignFlexEnd;
                break;
            case Alignment::Stretch:
                yogaAlign = YGAlignStretch;
                break;
        }
        YGNodeStyleSetAlignItems(yogaNode_, yogaAlign);
        
        // Set justification (main-axis) for parent
        YGJustify yogaJustify = YGJustifyFlexStart;
        switch (justification_) {
            case Justification::Start:
                yogaJustify = YGJustifyFlexStart;
                break;
            case Justification::Center:
                yogaJustify = YGJustifyCenter;
                break;
            case Justification::End:
                yogaJustify = YGJustifyFlexEnd;
                break;
            case Justification::Stretch:
                yogaJustify = YGJustifySpaceBetween;
                break;
        }
        YGNodeStyleSetJustifyContent(yogaNode_, yogaJustify);
    }
    
    // Update children's Yoga nodes (but don't insert them here, that should be done in AddChild)
    for (const auto& child : GetChildren()) {
        auto visualChild = boost::dynamic_pointer_cast<VisualElement>(child);
        if (visualChild) {
            visualChild->UpdateYogaNode();
        }
    }
}

void VisualElement::CalculateLayout(float parentWidth, float parentHeight, 
                                   float parentPaddingLeft, float parentPaddingTop) {
    if (!yogaNode_) {
        return;
    }
    
    // Update Yoga node properties
    UpdateYogaNode();

    auto parent = GetParent();
    float parentPaddingRight = 0.0f;
    float parentPaddingBottom = 0.0f;
    if (parent) {
        auto visualParent = boost::dynamic_pointer_cast<VisualElement>(parent);
        if (visualParent) {
            parentPaddingRight = visualParent->GetPadding(Padding::Right);
            parentPaddingBottom = visualParent->GetPadding(Padding::Bottom);
        }
    }
    
    float availableWidth = parentWidth - parentPaddingLeft - parentPaddingRight;
    float availableHeight = parentHeight - parentPaddingTop - parentPaddingBottom;
    
    // Calculate layout with Yoga
    YGNodeCalculateLayout(yogaNode_, availableWidth, availableHeight, YGDirectionLTR);
    
    left_ = YGNodeLayoutGetLeft(yogaNode_) + parentPaddingLeft;
    top_ = YGNodeLayoutGetTop(yogaNode_) + parentPaddingTop;
    width_ = YGNodeLayoutGetWidth(yogaNode_);
    height_ = YGNodeLayoutGetHeight(yogaNode_);
    
    float childParentWidth = width_ - paddingLeft_ - paddingRight_;
    float childParentHeight = height_ - paddingTop_ - paddingBottom_;
    
    for (const auto& child : GetChildren()) {
        auto visualChild = boost::dynamic_pointer_cast<VisualElement>(child);
        if (visualChild) {
            visualChild->CalculateLayout(childParentWidth, childParentHeight, 
                                        paddingLeft_, paddingTop_);
        }
    }
}

boost::shared_ptr<VisualElement> VisualElement::AsVisualElement() {
    return boost::static_pointer_cast<VisualElement>(shared_from_this());
}

boost::shared_ptr<VisualElement> VisualElement::HitTest(float x, float y) {
#ifdef TRACY_ENABLE
    ZoneScopedN("VisualElement::HitTest");
#endif
    
    if (!visible_ || opacity_ <= 0.0f) return nullptr;

    float myX = GetLeft(); // YGNodeLayoutGetLeft(yogaNode_)
    float myY = GetTop();

    float localX = x - myX;
    float localY = y - myY;

    // P_local = Matrix_Inverse × (P_parent - Offset_yoga)
    if (!transform_.isIdentity()) {
        SkMatrix inverse;
        if (transform_.invert(&inverse)) {
            float newX = inverse[SkMatrix::kMScaleX] * localX + 
                         inverse[SkMatrix::kMSkewX] * localY + 
                         inverse[SkMatrix::kMTransX];
            float newY = inverse[SkMatrix::kMSkewY] * localX + 
                         inverse[SkMatrix::kMScaleY] * localY + 
                         inverse[SkMatrix::kMTransY];
            localX = newX;
            localY = newY;
        } else {
            // 如果矩阵不可逆，跳过命中测试
            return nullptr;
        }
    }

    // 范围判定 (HitTestLocal 逻辑)
    if (HitTestLocal(localX, localY)) {
        
#ifdef TRACY_ENABLE
        ZoneScopedN("HitTest Children");
#endif
        // 逆序遍历子节点（后画的在上层）
        const auto& children = GetChildren();
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            auto visualChild = (*it)->AsVisualElement();
            if (visualChild) {
                auto hit = visualChild->HitTest(localX, localY);
                if (hit) return hit;
            }
        }

        // 如果子节点都没中，但点在自己范围内，则命中自己
        return boost::static_pointer_cast<VisualElement>(shared_from_this());
    }

    return nullptr;
}

bool VisualElement::HitTestLocal(float x, float y) const {
    bool result = (x >= 0.0f && x <= width_ && y >= 0.0f && y <= height_);
    
    #ifndef NDEBUG
    foundation::Logger::Debug("HitTestLocal: x={0}, y={1}, width={2}, height={3}, result={4}", 
                              x, y, width_, height_, result ? "YES" : "NO");
    #endif
    
    return result;
}

} // namespace widget
} // namespace KiUI