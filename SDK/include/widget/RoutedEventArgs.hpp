#ifndef ROUTEDEVENTARGS_HPP
#define ROUTEDEVENTARGS_HPP
#include <smart_ptr/shared_ptr.hpp>
#pragma once

namespace KiUI {
namespace widget {
    class UIElement;
}
namespace Events {
    enum class RoutingStrategy {
        Bubble,
        Direct,
        Tunnel,
    };
    class RoutedEventArgs {
        public:
        /*
        * @brief Constructor
        * @param routingStrategy the routing strategy
        */
        RoutedEventArgs(RoutingStrategy routingStrategy) : strategy_(routingStrategy), handled_(false) {}
        virtual ~RoutedEventArgs() = default;
        /*
        * @brief Get if the event is handled
        * @return if the event is handled
        */
        bool Handled() const { return handled_; }
        /*
        * @brief Set if the event is handled
        * @param handled if the event is handled
        */
        void SetHandled(bool handled) { handled_ = handled; }
        /*
        * @brief Get the original source of the event
        * @return the routing strategy
        */
        boost::shared_ptr<KiUI::widget::UIElement> GetOriginalSource() const { return originalSource_; }
        /*
        * @brief Set the original source of the event
        * @param originalSource the original source of the event
        */
        void SetOriginalSource(boost::shared_ptr<KiUI::widget::UIElement> originalSource) { originalSource_ = originalSource; }
        /*
        * @brief Get the node resolving event
        * @return the node resolving event
         */
        boost::shared_ptr<KiUI::widget::UIElement> GetNodeResolvingEvent() const { return source_;}
        void SetNodeResolvingEvent(boost::shared_ptr<KiUI::widget::UIElement> nodeResolvingEvent) { source_ = nodeResolvingEvent; }
        /*
        * @brief Get the routing strategy
        * @return the routing strategy
        */
        RoutingStrategy GetStrategy() const { return strategy_; }
        private:
        RoutingStrategy strategy_;
        bool handled_;
        boost::shared_ptr<KiUI::widget::UIElement> source_;
        boost::shared_ptr<KiUI::widget::UIElement> originalSource_;
    };
}
} // namespace KiUI

#endif // ROUTEDEVENTARGS_HPP