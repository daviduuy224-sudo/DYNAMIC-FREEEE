#pragma once
#include <algorithm>
#include <cstdint>
#include <deque>
#include <optional>

namespace dynamic {
struct QueuedInput { bool down; std::uint64_t due; bool player1 = true; };
class InputQueue {
    std::deque<QueuedInput> events_;
public:
    bool empty() const { return events_.empty(); }
    void clear() { events_.clear(); }
    void add(bool down, std::uint64_t now, std::uint64_t delay, bool player1 = true) {
        auto due = now + delay;
        if (!events_.empty()) due = std::max(due, events_.back().due + 1);
        events_.push_back({down, due, player1});
    }
    // Updates advance the clock before running physics. The first future
    // update is still an old-input step when the requested delay is one.
    void addAfterSteps(bool down, std::uint64_t now, std::uint8_t steps, bool player1 = true) {
        add(down, now, static_cast<std::uint64_t>(steps) + 1, player1);
    }
    std::uint64_t remaining(std::uint64_t now) const {
        return empty() || events_.front().due <= now ? 0 : events_.front().due - now;
    }
    std::optional<QueuedInput> pop(std::uint64_t now, bool force = false) {
        if (empty() || (!force && events_.front().due > now)) return std::nullopt;
        auto result = events_.front();
        events_.pop_front();
        return result;
    }
};
}
