#include "../src/portal_policy.hpp"
#include <cassert>

enum class Type { NormalGravityPortal = 4, InverseGravityPortal = 3,
    RegularSizePortal = 17, MiniSizePortal = 18 };
struct Player {
    bool m_isUpsideDown = false;
    float m_vehicleSize = 1.f;
    int flips = 0, scales = 0;
    void flipGravity(bool inverted, bool noEffects) {
        assert(noEffects); m_isUpsideDown = inverted; ++flips;
    }
    void togglePlayerScale(bool mini, bool noEffects) {
        assert(noEffects); m_vehicleSize = mini ? 0.6f : 1.f; ++scales;
    }
};
int main() {
    using namespace dynamic;
    Player live, ghost;
    for (int t = -1; t < 50; ++t) {
        auto a = portalAction(static_cast<Type>(t));
        assert((a != PortalAction::unsupported) == (t == 3 || t == 4 || t == 17 || t == 18));
    }
    applyPortal(ghost, portalAction(Type::InverseGravityPortal));
    applyPortal(ghost, portalAction(Type::InverseGravityPortal));
    assert(ghost.m_isUpsideDown && ghost.flips == 1);
    applyPortal(ghost, portalAction(Type::NormalGravityPortal));
    assert(!ghost.m_isUpsideDown && ghost.flips == 2);
    applyPortal(ghost, portalAction(Type::MiniSizePortal));
    applyPortal(ghost, portalAction(Type::MiniSizePortal));
    assert(ghost.m_vehicleSize == 0.6f && ghost.scales == 1);
    applyPortal(ghost, portalAction(Type::RegularSizePortal));
    assert(ghost.m_vehicleSize == 1.f && ghost.scales == 2);
    applyPortal(ghost, PortalAction::unsupported);
    assert(ghost.flips == 2 && ghost.scales == 2);
    assert(!live.m_isUpsideDown && live.m_vehicleSize == 1.f && live.flips == 0 && live.scales == 0);
}
