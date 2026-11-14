#pragma once

namespace dynamic {
// Use the game's resolved type, never the editor ID: many IDs share one
// collision behavior, and different objects can have very different effects.
enum class CollisionCategory { geometry, slope, decoration, unsupported };

template <typename Type>
constexpr CollisionCategory collisionCategory(Type type) {
    if (type == Type::Solid || type == Type::Hazard || type == Type::AnimatedHazard)
        return CollisionCategory::geometry;
    if (type == Type::Slope) return CollisionCategory::slope;
    if (type == Type::Decoration) return CollisionCategory::decoration;
    return CollisionCategory::unsupported;
}
}
