#include "../src/collision_policy.hpp"
#include <cassert>

// Values and names from Geode 5.11.2's GameObjectType. Production code
// instantiates the same policy with the actual SDK enum.
enum class ObjectType {
    Solid = 0, Hazard = 2, Decoration = 7, Slope = 25, AnimatedHazard = 47
};

int main() {
    using dynamic::CollisionCategory;
    using dynamic::collisionCategory;
    assert(collisionCategory(ObjectType::Solid) == CollisionCategory::geometry);
    assert(collisionCategory(ObjectType::Hazard) == CollisionCategory::geometry);
    assert(collisionCategory(ObjectType::AnimatedHazard) == CollisionCategory::geometry);
    assert(collisionCategory(ObjectType::Slope) == CollisionCategory::slope);
    assert(collisionCategory(ObjectType::Decoration) == CollisionCategory::decoration);
    // Portals, rings, pads, breakable objects and unknown future types must not
    // be mistaken for safe geometry or decoration.
    for (int value = -1; value <= 48; ++value) {
        if (value == 0 || value == 2 || value == 7 || value == 25 || value == 47) continue;
        assert(collisionCategory(static_cast<ObjectType>(value)) == CollisionCategory::unsupported);
    }
    assert(collisionCategory(static_cast<ObjectType>(9999)) == CollisionCategory::unsupported);
}
