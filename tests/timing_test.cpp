#include "../src/timing.hpp"

#include <cassert>
#include <vector>

using dynamic::Verdict;
using dynamic::DecisionReason;

int main() {
    using dynamic::chooseDelay;
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
    auto lastTick = chooseDelay(255, [](std::uint8_t tick) {
        return tick == 255 ? Verdict::safe : Verdict::unsafe;
    });
    assert(lastTick.delay == 255 && lastTick.predictionComplete);
}
