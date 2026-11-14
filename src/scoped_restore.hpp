#pragma once
#include <tuple>
#include <type_traits>
#include <utility>

namespace dynamic {
// GD exposes aliased fields through unions, so its complete state struct is
// not copyable. Save the explicitly used context fields by value instead.
template <typename State, typename... Members>
class ScopedRestore {
    State& state;
    std::tuple<Members...> members;
    std::tuple<std::decay_t<decltype(std::declval<State&>().*std::declval<Members>())>...> saved;
    template <std::size_t... I>
    void restore(std::index_sequence<I...>) {
        ((state.*std::get<I>(members) = std::move(std::get<I>(saved))), ...);
    }
public:
    ScopedRestore(State& value, Members... fields)
        : state(value), members(fields...), saved(value.*fields...) {}
    ScopedRestore(ScopedRestore const&) = delete;
    ScopedRestore& operator=(ScopedRestore const&) = delete;
    ~ScopedRestore() { restore(std::index_sequence_for<Members...>{}); }
};
}
