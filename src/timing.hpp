#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>

namespace dynamic {

// The simulator must distinguish an unsafe trajectory from one it cannot
// predict (for example, a portal or an exhausted time budget).
// unavoidable means a death in the unchanged-input prefix: every larger
// delay shares that same prefix. It is a proof for pruning, never uncertainty.
enum class Verdict { safe, unsafe, unknown, unavoidable };
enum class DecisionReason { disabled, safeNow, unknownInitial, delayed, unknownCandidate, noSafeTick };

// Invalid physics steps cannot be used as a prediction clock.
inline bool validTickDuration(float dt) {
    return std::isfinite(dt) && dt > 0.f && dt < 2.f;
}

// A queued delay is valid only for the physics clock used to predict it.
// Allow float rounding, but flush when the game's substep actually changes.
inline bool compatibleTickDuration(float predicted, float current) {
    return validTickDuration(predicted) && validTickDuration(current) &&
        std::abs(predicted - current) <= std::max(predicted, current) * 1e-5f;
}

// Check both sides of each step: an individual update or collision check can
// itself exceed the deadline, including on the very last step.
template <typename Expired, typename Step>
Verdict runPredictionSteps(unsigned steps, Expired&& expired, Step&& step) {
    for (unsigned i = 0; i < steps; ++i) {
        if (expired()) return Verdict::unknown;
        auto result = step(i);
        if (expired()) return Verdict::unknown;
        if (result != Verdict::safe) return result;
    }
    return Verdict::safe;
}

struct Decision {
    std::uint8_t delay = 0;
    bool predictionComplete = false;
    DecisionReason reason = DecisionReason::disabled;
    unsigned attempts = 0;
    unsigned skippedCandidates = 0;
};

// Only an early input can be delayed: this function cannot move an input into
// the past. If the prediction is incomplete, forward the original input.
inline Decision chooseDelay(std::uint8_t maxDelay,
                            const std::function<Verdict(std::uint8_t)>& simulate) {
    if (!simulate || maxDelay == 0) return {};

    auto initial = simulate(0);
    if (initial == Verdict::unknown) return {0, false, DecisionReason::unknownInitial, 1};
    if (initial == Verdict::unavoidable) return {0, true, DecisionReason::noSafeTick, 1, maxDelay};
    if (initial == Verdict::safe) return {0, true, DecisionReason::safeNow, 1};

    for (unsigned tick = 1; tick <= maxDelay; ++tick) {
        auto verdict = simulate(static_cast<std::uint8_t>(tick));
        if (verdict == Verdict::unknown) return {0, false, DecisionReason::unknownCandidate, tick + 1};
        if (verdict == Verdict::unavoidable)
            return {0, true, DecisionReason::noSafeTick, tick + 1, static_cast<unsigned>(maxDelay) - tick};
        if (verdict == Verdict::safe) return {static_cast<std::uint8_t>(tick), true, DecisionReason::delayed, tick + 1};
    }
    return {0, true, DecisionReason::noSafeTick, static_cast<unsigned>(maxDelay) + 1};
}

}  // namespace dynamic
