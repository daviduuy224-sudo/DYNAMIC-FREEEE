#pragma once

#include <cstdint>
#include <functional>

namespace dynamic {

// The simulator must distinguish an unsafe trajectory from one it cannot
// predict (for example, a portal or an exhausted time budget).
enum class Verdict { safe, unsafe, unknown };
enum class DecisionReason { disabled, safeNow, unknownInitial, delayed, unknownCandidate, noSafeTick };

struct Decision {
    std::uint8_t delay = 0;
    bool predictionComplete = false;
    DecisionReason reason = DecisionReason::disabled;
    unsigned attempts = 0;
};

// Only an early input can be delayed: this function cannot move an input into
// the past. If the prediction is incomplete, forward the original input.
inline Decision chooseDelay(std::uint8_t maxDelay,
                            const std::function<Verdict(std::uint8_t)>& simulate) {
    if (!simulate || maxDelay == 0) return {};

    auto initial = simulate(0);
    if (initial == Verdict::unknown) return {0, false, DecisionReason::unknownInitial, 1};
    if (initial == Verdict::safe) return {0, true, DecisionReason::safeNow, 1};

    for (unsigned tick = 1; tick <= maxDelay; ++tick) {
        auto verdict = simulate(static_cast<std::uint8_t>(tick));
        if (verdict == Verdict::unknown) return {0, false, DecisionReason::unknownCandidate, tick + 1};
        if (verdict == Verdict::safe) return {static_cast<std::uint8_t>(tick), true, DecisionReason::delayed, tick + 1};
    }
    return {0, true, DecisionReason::noSafeTick, static_cast<unsigned>(maxDelay) + 1};
}

}  // namespace dynamic
