#pragma once

namespace dynamic {
enum class PortalAction { unsupported, gravityNormal, gravityInverted, regularSize, miniSize };

template <typename Type>
constexpr PortalAction portalAction(Type type) {
    if (type == Type::NormalGravityPortal) return PortalAction::gravityNormal;
    if (type == Type::InverseGravityPortal) return PortalAction::gravityInverted;
    if (type == Type::RegularSizePortal) return PortalAction::regularSize;
    if (type == Type::MiniSizePortal) return PortalAction::miniSize;
    return PortalAction::unsupported;
}

// Dispatch exclusively to the candidate player. No object activation, layer
// gravity/camera changes, or shared object flags belong in this path.
template <typename Player>
void applyPortal(Player& player, PortalAction action) {
    switch (action) {
        case PortalAction::gravityNormal:
        case PortalAction::gravityInverted: {
            bool inverted = action == PortalAction::gravityInverted;
            if (player.m_isUpsideDown != inverted) player.flipGravity(inverted, true);
            break;
        }
        case PortalAction::regularSize:
        case PortalAction::miniSize: {
            bool mini = action == PortalAction::miniSize;
            if ((player.m_vehicleSize < 1.f) != mini) player.togglePlayerScale(mini, true);
            break;
        }
        case PortalAction::unsupported: break;
    }
}
}
