#include "../src/timing.hpp"
#include <cassert>
#include <optional>
using dynamic::Verdict;

// Deterministic corridor with a fixed obstacle. Before the click the icon
// moves right; clicking reverses its motion. Compare against full exhaustive
// replay across speeds, obstacle distances, click delays, and search windows.
int main() {
    for (int speed = 1; speed <= 5; ++speed) {
        for (int obstacle = 1; obstacle <= 80; ++obstacle) {
            for (unsigned window : {1u, 16u, 64u}) {
                auto replay = [&](unsigned delay, bool prune) {
                    int x = 0;
                    for (unsigned step = 0; step < 32 + delay; ++step) {
                        x += step < delay ? speed : -speed;
                        if (x >= obstacle) {
                            return prune && step < delay ? Verdict::unavoidable : Verdict::unsafe;
                        }
                        // A second obstacle means an immediate click may fail,
                        // while a short delay can still be valid.
                        if (x < -60) return Verdict::unsafe;
                    }
                    return Verdict::safe;
                };
                std::optional<unsigned> expected;
                for (unsigned delay = 0; delay <= window; ++delay) {
                    if (replay(delay, false) == Verdict::safe) { expected = delay; break; }
                }
                auto decision = dynamic::chooseDelay(static_cast<unsigned char>(window), [&](auto delay) {
                    return replay(delay, true);
                });
                assert(decision.predictionComplete);
                assert(decision.delay == expected.value_or(0));
                if (!expected) assert(decision.reason == dynamic::DecisionReason::noSafeTick);
            }
        }
    }
    unsigned calls = 0;
    auto pruned = dynamic::chooseDelay(64, [&](unsigned delay) {
        ++calls;
        return delay < 4 ? Verdict::unsafe : Verdict::unavoidable;
    });
    assert(calls == 5 && pruned.attempts == 5 && pruned.skippedCandidates == 60);
    assert(pruned.predictionComplete && pruned.delay == 0);
    auto independentDeath = dynamic::chooseDelay(64, [](unsigned) { return Verdict::unavoidable; });
    assert(independentDeath.attempts == 1 && independentDeath.skippedCandidates == 64);
    unsigned clockChecks = 0;
    auto exhausted = dynamic::runPredictionSteps(10, [&] { return ++clockChecks == 2; },
        [](unsigned) { return Verdict::unavoidable; });
    assert(exhausted == Verdict::unknown); // deadline still takes precedence
    auto uncertain = dynamic::chooseDelay(64, [](unsigned) { return Verdict::unknown; });
    assert(!uncertain.predictionComplete && uncertain.skippedCandidates == 0);
}
