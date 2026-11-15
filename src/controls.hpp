#pragma once

#include <algorithm>
#include <cstdint>

namespace dynamic {
inline constexpr std::int64_t maxWindow = 64;

inline constexpr std::int64_t maxHorizon = 256;
inline constexpr std::int64_t maxBudget = 66;
constexpr unsigned clampHorizon(std::int64_t value) {
    return static_cast<unsigned>(std::clamp(value, std::int64_t{32}, maxHorizon));
}
constexpr std::int64_t clampBudget(std::int64_t value) {
    return std::clamp(value, std::int64_t{1}, maxBudget);
}

constexpr std::int64_t clampWindow(std::int64_t value) {
    return std::clamp(value, std::int64_t{0}, maxWindow);
}
constexpr std::int64_t nextWindow(std::int64_t value) {
    return (clampWindow(value) + 1) % (maxWindow + 1);
}
constexpr std::int64_t toggleWindow(std::int64_t current, std::int64_t previous) {
    return clampWindow(current) > 0 ? 0 : std::max<std::int64_t>(1, clampWindow(previous));
}

constexpr bool pendingExceedsWindow(std::uint8_t plannedDelay, std::int64_t window) {
    return plannedDelay > clampWindow(window);
}

struct AttemptStats {
    std::uint64_t corrected = 0;
    std::uint64_t delayedTicks = 0;
    std::uint64_t lastDelay = 0;

    void delivered(std::uint64_t startStep, std::uint64_t completedSteps) {
        auto delay = completedSteps >= startStep ? completedSteps - startStep : 0;
        if (delay == 0) return;
        ++corrected;
        delayedTicks += delay;
        lastDelay = delay;
    }
};
}
