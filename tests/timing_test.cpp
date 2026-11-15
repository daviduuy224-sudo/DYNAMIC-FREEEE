#include "../src/timing.hpp"

#include <cassert>
#include <limits>
#include <vector>

using dynamic::Verdict;
using dynamic::DecisionReason;

int main() {
    using dynamic::chooseDelay;
    using dynamic::runPredictionSteps;
    using dynamic::validTickDuration;
    assert(validTickDuration(1.f / 240.f));
    assert(!validTickDuration(0.f) && !validTickDuration(-1.f));
    assert(!validTickDuration(2.f) && !validTickDuration(std::numeric_limits<float>::infinity()));
    assert(!validTickDuration(std::numeric_limits<float>::quiet_NaN()));
    using dynamic::compatibleTickDuration;
    assert(compatibleTickDuration(.25f, .25f));
    assert(compatibleTickDuration(.25f, std::nextafter(.25f, 1.f)));
    assert(!compatibleTickDuration(.25f, .125f));
    assert(!compatibleTickDuration(.125f, .25f));
    assert(!compatibleTickDuration(0.f, .25f));
    assert(!compatibleTickDuration(.25f, 0.f));
    assert(!compatibleTickDuration(.25f, std::numeric_limits<float>::quiet_NaN()));

    unsigned checks = 0, simulated = 0;
    auto deadline = [&] { return ++checks >= 4; }; // expires during step 2
    auto safeStep = [&](unsigned) { ++simulated; return Verdict::safe; };
    assert(runPredictionSteps(2, deadline, safeStep) == Verdict::unknown);
    assert(simulated == 2); // includes expiration after the last step
    checks = simulated = 0;
    auto before = [&] { return true; };
    assert(runPredictionSteps(2, before, safeStep) == Verdict::unknown && simulated == 0);
    auto never = [] { return false; };
    auto collision = [&](unsigned i) { ++simulated; return i == 1 ? Verdict::unsafe : Verdict::safe; };
    assert(runPredictionSteps(4, never, collision) == Verdict::unsafe && simulated == 2);
    simulated = 0;
    auto uncertain = [&](unsigned) { ++simulated; return Verdict::unknown; };
    assert(runPredictionSteps(4, never, uncertain) == Verdict::unknown && simulated == 1);
    std::vector<int> calls;
    auto safe = [&](std::uint8_t tick) { calls.push_back(tick); return Verdict::safe; };
    auto disabled = chooseDelay(0, safe);
    assert(disabled.delay == 0 && !disabled.predictionComplete && calls.empty());
    assert(disabled.reason == DecisionReason::disabled && disabled.attempts == 0);
    auto alreadySafe = chooseDelay(4, safe);
    assert(alreadySafe.delay == 0 && alreadySafe.predictionComplete && calls == std::vector<int>{0});
    assert(alreadySafe.reason == DecisionReason::safeNow && alreadySafe.attempts == 1);

    calls.clear();
    auto early = chooseDelay(4, [&](std::uint8_t tick) {
        calls.push_back(tick);
        return tick >= 2 ? Verdict::safe : Verdict::unsafe;
    });
    assert(early.delay == 2 && early.predictionComplete);
    assert(early.reason == DecisionReason::delayed && early.attempts == 3);
    assert((calls == std::vector<int>{0, 1, 2}));

    auto noSafeTick = chooseDelay(3, [](std::uint8_t) { return Verdict::unsafe; });
    assert(noSafeTick.delay == 0 && noSafeTick.predictionComplete);
    assert(noSafeTick.reason == DecisionReason::noSafeTick && noSafeTick.attempts == 4);
    auto unknown = chooseDelay(3, [](std::uint8_t tick) {
        return tick == 1 ? Verdict::unknown : Verdict::unsafe;
    });
    assert(unknown.delay == 0 && !unknown.predictionComplete);
    assert(unknown.reason == DecisionReason::unknownCandidate && unknown.attempts == 2);
    auto initialUnknown = chooseDelay(3, [](std::uint8_t) { return Verdict::unknown; });
    assert(initialUnknown.reason == DecisionReason::unknownInitial && initialUnknown.attempts == 1);
    auto aboveEight = chooseDelay(16, [](std::uint8_t tick) {
        return tick == 12 ? Verdict::safe : Verdict::unsafe;
    });
    assert(aboveEight.delay == 12 && aboveEight.attempts == 13 && aboveEight.predictionComplete);
    auto extended = chooseDelay(64, [](std::uint8_t tick) {
        return tick == 64 ? Verdict::safe : Verdict::unsafe;
    });
    assert(extended.delay == 64 && extended.attempts == 65 && extended.predictionComplete);
    unsigned steps = 0;
    assert(runPredictionSteps(256 + 64, never, [&](unsigned) {
        ++steps; return Verdict::safe;
    }) == Verdict::safe);
    assert(steps == 320);
    auto lastTick = chooseDelay(255, [](std::uint8_t tick) {
        return tick == 255 ? Verdict::safe : Verdict::unsafe;
    });
    assert(lastTick.delay == 255 && lastTick.predictionComplete);
}
