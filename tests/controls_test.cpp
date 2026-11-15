#include "../src/controls.hpp"
#include "../src/input_queue.hpp"
#include <cassert>

int main() {
    using namespace dynamic;
    assert(clampWindow(-2) == 0 && clampWindow(99) == 64);
    assert(nextWindow(8) == 9 && nextWindow(15) == 16 && nextWindow(16) == 17 && nextWindow(63) == 64 && nextWindow(64) == 0);
    assert(toggleWindow(8, 8) == 0);
    assert(toggleWindow(0, 12) == 12 && toggleWindow(0, 0) == 1);
    assert(toggleWindow(0, 100) == 64);
    assert(!pendingExceedsWindow(12, 16));
    assert(!pendingExceedsWindow(12, 12));
    assert(pendingExceedsWindow(12, 11));
    assert(pendingExceedsWindow(1, 0));

    assert(clampHorizon(-1) == 32 && clampHorizon(128) == 128 && clampHorizon(999) == 256);
    assert(clampBudget(-1) == 1 && clampBudget(50) == 50 && clampBudget(999) == 66);
    InputQueue extended;
    extended.addAfterSteps(true, 100, 64);
    assert(!extended.pop(164));
    assert(extended.pop(165)->down);
    assert(!pendingExceedsWindow(64, 64) && pendingExceedsWindow(64, 63));
    AttemptStats attempt;
    // An immediate flush has not delayed an input and must not count.
    attempt.delivered(10, 10);
    assert(attempt.corrected == 0 && attempt.delayedTicks == 0);
    // A scheduled delivery and an interrupted delivery count actual steps.
    attempt.delivered(10, 21);
    attempt.delivered(21, 23);
    assert(attempt.corrected == 2 && attempt.delayedTicks == 13 && attempt.lastDelay == 2);
    attempt = {};
    assert(attempt.corrected == 0 && attempt.delayedTicks == 0 && attempt.lastDelay == 0);

    InputQueue pending;
    pending.addAfterSteps(true, 100, 12);
    std::uint64_t completedSteps = 100;
    for (std::uint64_t tick = 101; tick <= 113; ++tick) {
        if (auto event = pending.pop(tick)) {
            assert(event->down);
            attempt.delivered(100, completedSteps);
        }
        ++completedSteps;
    }
    assert(pending.empty() && attempt.corrected == 1 && attempt.lastDelay == 12);
    pending.addAfterSteps(false, 113, 12);
    completedSteps = 115; // Another input interrupts after two completed steps.
    assert(pending.pop(115, true));
    attempt.delivered(113, completedSteps);
    assert(attempt.corrected == 2 && attempt.delayedTicks == 14 && attempt.lastDelay == 2);
}
