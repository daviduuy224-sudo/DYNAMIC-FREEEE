#pragma once

namespace dynamic {
// Use the game's resolved type, never the editor ID: many IDs share one
// collision behavior, and different objects can have very different effects.
// Color-only triggers identified by GameObject::isColorTrigger in the pinned
// SDK. Do not include move/toggle/spawn or unknown Modifier IDs here.
constexpr bool colorOnlyTrigger(int id) {
    return id == 29 || id == 30 || id == 105 || id == 744 ||
        id == 899 || id == 900 || id == 915;
}

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
