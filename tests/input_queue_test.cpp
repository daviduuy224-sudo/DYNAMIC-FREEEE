#include "../src/input_queue.hpp"
#include <cassert>
int main() {
    dynamic::InputQueue routed;
    routed.addAfterSteps(true, 10, 3, false);
    assert(!routed.pop(13));
    auto secondPress = routed.pop(14);
    assert(secondPress && secondPress->down && !secondPress->player1);
    routed.addAfterSteps(false, 14, 4, false);
    auto interrupted = routed.pop(15, true);
    assert(interrupted && !interrupted->down && !interrupted->player1);
    routed.addAfterSteps(true, 15, 1);
    auto primary = routed.pop(17);
    assert(primary && primary->down && primary->player1);

    // A one-step prediction after tick 10 must preserve input for step 11
    // and apply the new input before step 12.
    dynamic::InputQueue timeline;
    bool physicalDown = false;
    timeline.addAfterSteps(true, 10, 1);
    for (std::uint64_t tick = 11; tick <= 12; ++tick) {
        if (auto event = timeline.pop(tick)) physicalDown = event->down;
        if (tick == 11) assert(!physicalDown);
        if (tick == 12) assert(physicalDown);
    }
    assert(timeline.empty());
    dynamic::InputQueue q;
    assert(!q.pop(0));
    q.add(true, 10, 3);
    q.add(false, 11, q.remaining(11));
    assert(q.remaining(11) == 2 && !q.pop(12));
    auto press = q.pop(13);
    assert(press && press->down && press->due == 13 && !q.pop(13));
    auto release = q.pop(14);
    assert(release && !release->down && release->due == 14 && q.empty());
    q.add(true, 20, 6);
    assert(!q.pop(20));
    assert(q.pop(20, true)->down && q.empty());
    q.add(false, 21, 1);
    q.clear();
    assert(q.empty() && !q.pop(100));
}
