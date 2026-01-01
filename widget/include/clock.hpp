#ifndef CLOCK_HPP
#define CLOCK_HPP
#pragma once

#include <chrono>

namespace KiUI {
namespace widget {

// Clock widget class
// TODO: Implement clock widget
class CompositionClock{
    public:
    CompositionClock();
    ~CompositionClock();
    void Tick();
    float GetDeltaTime() const;
    float GetTotalTime() const;
    private:
    std::chrono::steady_clock::time_point lastTickTime_;
    float deltaTime_ = 0.0f;
    float totalTime_ = 0.0f;
};

} // namespace widget
} // namespace KiUI

#endif // CLOCK_HPP

