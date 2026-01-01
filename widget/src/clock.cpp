#include "clock.hpp"
#include <chrono>
#include <ratio>

namespace KiUI {
namespace widget {

// Clock widget implementation
// TODO: Implement clock widget

CompositionClock::CompositionClock() {
}

CompositionClock::~CompositionClock() {
}

void CompositionClock::Tick() {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> diff = now - lastTickTime_;
    deltaTime_ = diff.count();
    totalTime_ += deltaTime_;
    lastTickTime_ = now;
    if (deltaTime_ > 100.0f) {
        deltaTime_ = 8.0f; // if deltaTime is too large, set it to 8ms
    }
}
float CompositionClock::GetDeltaTime() const {
    return deltaTime_;
}

float CompositionClock::GetTotalTime() const {
    return totalTime_;
}

} // namespace widget
} // namespace KiUI
