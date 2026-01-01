#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP
#pragma once
#include <smart_ptr/shared_ptr.hpp>
#include <smart_ptr/enable_shared_from_this.hpp>
#include <vector>

namespace KiUI {
namespace widget {
class UIElement : public boost::enable_shared_from_this<UIElement> {
    public:
    /*
    * @brief Add a child to the UI element
    * @param child the child to add
    */
    void AddChild(boost::shared_ptr<UIElement> child);
    /*
    * @brief Remove a child from the UI element
    * @param child the child to remove
    */
    void RemoveChild(boost::shared_ptr<UIElement> child);
    /*
    * @brief Get the children of the UI element
    * @return the children of the UI element
    */
    const std::vector<boost::shared_ptr<UIElement>>& GetChildren() const { return Children_; }
    /*
    * @brief Get the parent of the UI element
    * @return the parent of the UI element
    */
    boost::shared_ptr<UIElement> GetParent() const { return Parent_; }
    /*
    * @brief Get the children count of the UI element
    * @return the children count of the UI element
    */
    size_t GetChildrenCount() const { return Children_.size(); }
    /*
    * @brief Constructor
    * @return the UI element
    */
    UIElement();
    ~UIElement();
    UIElement(const UIElement& other) = delete; // copy constructor is deleted
    UIElement& operator=(const UIElement& other) = delete; // copy assignment operator is deleted
    
    protected:
    boost::shared_ptr<UIElement> Parent_;
    std::vector<boost::shared_ptr<UIElement>> Children_;
    private:
};

} // namespace widget
} // namespace KiUI
#endif // UIELEMENT_HPP