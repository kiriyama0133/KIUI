#ifndef EVENTROUTE_HPP
#define EVENTROUTE_HPP

#include <vector>
#include <algorithm>
#include <smart_ptr/shared_ptr.hpp>
#include "RoutedEventArgs.hpp"
#include "UIElement.hpp"
#include "VisualElement.hpp"

namespace KiUI {
namespace Events {

class EventRoute {
public:
    EventRoute(boost::shared_ptr<RoutedEventArgs> args) : args_(args) {}

    /**
     * @brief 添加目标元素到路由路径
     * @param element 要添加的元素
     */
    void AddTarget(boost::shared_ptr<widget::UIElement> element) {
        if (element) {
            path_.push_back(element);
        }
    }

    /**
     * @brief 构建从根元素到目标元素的完整路径
     * @param target 目标元素
     * @return 是否成功构建路径
     */
    bool BuildPath(boost::shared_ptr<widget::UIElement> target) {
        if (!target) return false;
        
        path_.clear();
        boost::shared_ptr<widget::UIElement> current = target;
        while (current) {
            path_.push_back(current);
            current = current->GetParent();
        }
        std::reverse(path_.begin(), path_.end());
        return !path_.empty();
    }

    /**
     * @brief 执行路由分发
     * 根据路由策略（Tunnel/Bubble/Direct）分发事件
     */
    void Invoke() {
        if (!args_ || path_.empty()) return;

        RoutingStrategy strategy = args_->GetStrategy();

        if (strategy == RoutingStrategy::Direct) {
            DispatchToElement(path_.back());
        } else if (strategy == RoutingStrategy::Tunnel) {
            // 隧道：Root -> Target
            for (auto it = path_.begin(); it != path_.end(); ++it) {
                if (args_->Handled()) break;
                DispatchToElement(*it);
            }
        } else if (strategy == RoutingStrategy::Bubble) {
            // 冒泡：Target -> Root
            for (auto it = path_.rbegin(); it != path_.rend(); ++it) {
                if (args_->Handled()) break;
                DispatchToElement(*it);
            }
        }
    }

    const std::vector<boost::shared_ptr<widget::UIElement>>& GetPath() const {
        return path_;
    }

    void Clear() {
        path_.clear();
    }

    /**
     * @brief 执行命中测试，找到指定坐标下的元素
     * @param root 根元素
     * @param x X 坐标
     * @param y Y 坐标
     * @return 被命中的元素，如果没有则返回 nullptr
     */
    static boost::shared_ptr<widget::UIElement> HitTest(
        boost::shared_ptr<widget::UIElement> root, 
        float x, 
        float y) {
        if (!root) return nullptr;

        auto visual = boost::dynamic_pointer_cast<widget::VisualElement>(root);
        if (!visual) {
            const auto& children = root->GetChildren();
            for (auto it = children.rbegin(); it != children.rend(); ++it) {
                auto hit = HitTest(*it, x, y);
                if (hit) return hit;
            }
            return nullptr;
        }

        // 检查可见性
        if (!visual->GetVisibility()) {
            return nullptr;
        }

        // 执行命中测试
        if (!visual->HitTest(x, y)) {
            return nullptr;
        }

        // 递归检查子元素（从后往前，因为后面的元素在上层）
        const auto& children = root->GetChildren();
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            auto hit = HitTest(*it, x, y);
            if (hit) {
                return hit;  // 返回最上层的命中元素
            }
        }

        // 如果没有子元素被命中，返回当前元素
        return root;
    }

private:
    void DispatchToElement(boost::shared_ptr<widget::UIElement> element) {
        if (!element || !args_) return;
        args_->SetNodeResolvingEvent(element);
    }

    boost::shared_ptr<RoutedEventArgs> args_;
    std::vector<boost::shared_ptr<widget::UIElement>> path_;
};

} // namespace Events
} // namespace KiUI

#endif // EVENTROUTE_HPP