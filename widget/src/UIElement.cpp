#include "UIElement.hpp"
#include <algorithm>

namespace KiUI {
namespace widget {

UIElement::UIElement() {
}

UIElement::~UIElement() {
    // 清除所有子元素的父引用
    for (auto& child : Children_) {
        if (child) {
            child->Parent_.reset();
        }
    }
    Children_.clear();
}

void UIElement::AddChild(boost::shared_ptr<UIElement> child) {
    if (!child) return;
    
    // 如果子元素已经有父元素，先从旧父元素中移除
    if (auto oldParent = child->GetParent()) {
        oldParent->RemoveChild(child);
    }
    
    // 设置子元素的父引用（使用 shared_ptr）
    child->Parent_ = shared_from_this();
    
    // 添加到子元素列表
    Children_.push_back(child);
}

void UIElement::RemoveChild(boost::shared_ptr<UIElement> child) {
    if (!child) return;
    
    // 从子元素列表中移除
    auto it = std::remove(Children_.begin(), Children_.end(), child);
    if (it != Children_.end()) {
        Children_.erase(it, Children_.end());
        // 清除子元素的父引用
        child->Parent_.reset();
    }
}

} // namespace widget
} // namespace KiUI