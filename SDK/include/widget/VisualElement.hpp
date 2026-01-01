#ifndef VISUALELEMENT_HPP
#define VISUALELEMENT_HPP

#include "UIElement.hpp"
#include <include/core/SkMatrix.h>
#include <include/core/SkColor.h>
#include <yoga/Yoga.h>
#include <include/core/SkCanvas.h>

namespace KiUI {
namespace widget {
enum class Margin{
    Top,
    Bottom,
    Left,
    Right,
    All,
};
enum class Padding{
    Top,
    Bottom,
    Left,
    Right,
    All,
};
enum class Border{
    Top,
    Bottom,
    Left,
    Right,
    All,
};
enum class BorderRadius{
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    All,
};
enum class BorderWidth{
    Top,
    Bottom,
    Left,
    Right,
    All,
};
enum class BorderStyle{
    Top,
    Bottom,
    Left,
    Right,
    All,
};
enum class Alignment{
    Start,
    Center,
    End,
    Stretch,
};
enum class Justification{
    Start,
    Center,
    End,
    Stretch,
};
class VisualElement : public UIElement {
public:
    VisualElement();
    virtual ~VisualElement();
    /*
    * @brief Set the opacity of the visual element
    * @param opacity the opacity of the visual element
    */
    void SetOpacity(float opacity);
    /*
    * @brief Get the opacity of the visual element
    * @return the opacity of the visual element
    */
    float GetOpacity() const { return opacity_; }

    /*
    * @brief Set the visibility of the visual element
    * @param visible the visibility of the visual element
    */
    void SetVisibility(bool visible);
    /*
    * @brief Get the visibility of the visual element
    * @return the visibility of the visual element
    */
    bool GetVisibility() const { return visible_; }

    /*
    * @brief Set the transform of the visual element
    * @param matrix the transform of the visual element
    */
    void SetTransform(const SkMatrix& matrix);
    /*
    * @brief Get the transform of the visual element
    * @return the transform of the visual element
    */
    const SkMatrix& GetTransform() const { return transform_; }

    /*
    * @brief Set the width of the visual element
    * @param width the width of the visual element
    */
    void SetWidth(float width);
    /*
    * @brief Get the width of the visual element
    * @return the width of the visual element
    */
    float GetWidth() const { return width_; }
    /*
    * @brief Set the height of the visual element
    * @param height the height of the visual element
    */
    void SetHeight(float height);
    /*
    * @brief Get the height of the visual element
    * @return the height of the visual element
    */
    float GetHeight() const { return height_; }
    /*
    * @brief Set the left position of the visual element (relative to parent)
    * @param left the left position
    */
    void SetLeft(float left);
    /*
    * @brief Get the left position of the visual element (relative to parent)
    * @return the left position
    */
    float GetLeft() const { return left_; }
    /*
    * @brief Set the top position of the visual element (relative to parent)
    * @param top the top position
    */
    void SetTop(float top);
    /*
    * @brief Get the top position of the visual element (relative to parent)
    * @return the top position
    */
    float GetTop() const { return top_; }
    /*
    * @brief Set the alignment of the visual element (cross-axis alignment)
    * @param alignment the alignment value
    */
    void SetAlignment(Alignment alignment);
    /*
    * @brief Get the alignment of the visual element
    * @return the alignment value
    */
    Alignment GetAlignment() const { return alignment_; }
    /*
    * @brief Set the justification of the visual element (main-axis alignment)
    * @param justification the justification value
    */
    void SetJustification(Justification justification);
    /*
    * @brief Get the justification of the visual element
    * @return the justification value
    */
    Justification GetJustification() const { return justification_; }
    /*
    * @brief Calculate layout using Yoga layout engine
    * @param parentWidth parent container width (for root, this is window width)
    * @param parentHeight parent container height (for root, this is window height)
    * @param parentPaddingLeft parent container's left padding
    * @param parentPaddingTop parent container's top padding
    */
    void CalculateLayout(float parentWidth, float parentHeight, 
                       float parentPaddingLeft = 0.0f, float parentPaddingTop = 0.0f);
    /*
    * @brief Update Yoga node properties from current VisualElement properties
    */
    void UpdateYogaNode();
    /*
    * @brief Add a child to the visual element (overrides UIElement::AddChild)
    * @param child the child to add
    */
    void AddChild(boost::shared_ptr<UIElement> child);
    /*
    * @brief Remove a child from the visual element (overrides UIElement::RemoveChild)
    * @param child the child to remove
    */
    void RemoveChild(boost::shared_ptr<UIElement> child);
    /*
    * @brief Set the margin of the visual element
    * @param edge the edge to set (Top/Bottom/Left/Right/All)
    * @param margin the margin value
    */
    void SetMargin(Margin edge, float margin);
    /*
    * @brief Get the margin of the visual element
    * @param edge the edge to get (Top/Bottom/Left/Right)
    * @return the margin value
    */
    float GetMargin(Margin edge) const;
    /*
    * @brief Set the padding of the visual element
    * @param edge the edge to set (Top/Bottom/Left/Right/All)
    * @param padding the padding value
    */
    void SetPadding(Padding edge, float padding);
    /*
    * @brief Get the padding of the visual element
    * @param edge the edge to get (Top/Bottom/Left/Right)
    * @return the padding value
    */
    float GetPadding(Padding edge) const;
    /*
    * @brief Set the border width of the visual element
    * @param edge the edge to set (Top/Bottom/Left/Right/All)
    * @param width the border width value
    */
    void SetBorderWidth(BorderWidth edge, float width);
    /*
    * @brief Get the border width of the visual element
    * @param edge the edge to get (Top/Bottom/Left/Right)
    * @return the border width value
    */
    float GetBorderWidth(BorderWidth edge) const;
    /*
    * @brief Set the border color of the visual element
    * @param color the border color (SkColor)
    */
    void SetBorderColor(SkColor color);
    /*
    * @brief Get the border color of the visual element
    * @return the border color (SkColor)
    */
    SkColor GetBorderColor() const { return borderColor_; }
    /*
    * @brief Set the border radius of the visual element
    * @param corner the corner to set (TopLeft/TopRight/BottomLeft/BottomRight/All)
    * @param radius the border radius value
    */
    void SetBorderRadius(BorderRadius corner, float radius);
    /*
    * @brief Get the border radius of the visual element
    * @param corner the corner to get (TopLeft/TopRight/BottomLeft/BottomRight)
    * @return the border radius value
    */
    float GetBorderRadius(BorderRadius corner) const;
    /*
    * @brief Set the background color of the visual element
    * @param color the background color (SkColor)
    */
    void SetBackgroundColor(SkColor color);
    /*
    * @brief Get the background color of the visual element
    * @return the background color (SkColor)
    */
    SkColor GetBackgroundColor() const { return backgroundColor_; }
    /*
    * @brief Set the foreground color of the visual element
    * @param color the foreground color (SkColor)
    */
    void SetForegroundColor(SkColor color);
    /*
    * @brief Get the foreground color of the visual element
    * @return the foreground color (SkColor)
    */
    SkColor GetForegroundColor() const { return foregroundColor_; }
    /*
    * @brief Convert this UIElement to VisualElement
    * @return shared_ptr to this VisualElement
    * @note Overrides UIElement::AsVisualElement() to avoid expensive dynamic_cast
    */
    virtual boost::shared_ptr<VisualElement> AsVisualElement() override;
    /*
    * @brief Hit test the visual element with parent-local coordinates
    * @param x parent-local x coordinate
    * @param y parent-local y coordinate
    * @return the deepest hit VisualElement, or nullptr if not hit
    */
    virtual boost::shared_ptr<VisualElement> HitTest(float x, float y);
    /*
    * @brief Hit test the visual element with local coordinates
    * @param x local x coordinate (relative to this element)
    * @param y local y coordinate (relative to this element)
    * @return true if the point hits this element, false otherwise
    * @note Derived classes can override this to implement custom hit testing (e.g., circular, path-based)
    */
    virtual bool HitTestLocal(float x, float y) const;
    /*
    * @brief Render the visual element
    * @param canvas the canvas to render the visual element
    */
    virtual void Render(SkCanvas* canvas) = 0;
protected:
    YGNodeRef yogaNode_;
    float TransformX_ = 0.0f;
    float TransformY_ = 0.0f;
    float ScaleX_ = 1.0f;
    float ScaleY_ = 1.0f;
    float Rotate_ = 0.0f;
    float opacity_ = 1.0f;
    
    // Margin values
    float marginTop_ = 0.0f;
    float marginBottom_ = 0.0f;
    float marginLeft_ = 0.0f;
    float marginRight_ = 0.0f;
    
    // Padding values
    float paddingTop_ = 0.0f;
    float paddingBottom_ = 0.0f;
    float paddingLeft_ = 0.0f;
    float paddingRight_ = 0.0f;
    
    // Border width values
    float borderWidthTop_ = 0.0f;
    float borderWidthBottom_ = 0.0f;
    float borderWidthLeft_ = 0.0f;
    float borderWidthRight_ = 0.0f;
    
    // Border color value (SkColor)
    SkColor borderColor_ = SK_ColorBLACK;
    
    // Border radius values
    float borderRadiusTopLeft_ = 0.0f;
    float borderRadiusTopRight_ = 0.0f;
    float borderRadiusBottomLeft_ = 0.0f;
    float borderRadiusBottomRight_ = 0.0f;
    
    // Background and foreground colors
    SkColor backgroundColor_ = SK_ColorTRANSPARENT;
    SkColor foregroundColor_ = SK_ColorBLACK;
    
    float width_ = 0.0f;
    float height_ = 0.0f;
    float left_ = 0.0f;  // Position relative to parent (calculated by Yoga)
    float top_ = 0.0f;   // Position relative to parent (calculated by Yoga)
    bool visible_ = true;
    SkMatrix transform_;
    
    // Layout properties
    Alignment alignment_ = Alignment::Stretch;
    Justification justification_ = Justification::Start;
};

} // namespace widget
} // namespace KiUI
#endif