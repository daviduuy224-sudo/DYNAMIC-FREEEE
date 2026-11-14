#pragma once
#include "mode_policy.hpp"

namespace dynamic {
template <typename Type>
constexpr Vehicle portalVehicle(Type type) {
    if (type == Type::CubePortal) return Vehicle::cube;
    if (type == Type::ShipPortal) return Vehicle::ship;
    if (type == Type::UfoPortal) return Vehicle::ufo;
    if (type == Type::BallPortal) return Vehicle::ball;
    if (type == Type::WavePortal) return Vehicle::wave;
    if (type == Type::RobotPortal) return Vehicle::robot;
    if (type == Type::SpiderPortal) return Vehicle::spider;
    if (type == Type::SwingPortal) return Vehicle::swing;
    return Vehicle::unknown;
}

// Editor speed portal IDs. Modifier also includes unrelated gameplay objects;
// never infer a speed change from the Modifier type alone.
constexpr float portalSpeed(int id) {
    switch (id) {
        case 200: return .7f;
        case 201: return .9f;
        case 202: return 1.1f;
        case 203: return 1.3f;
        case 1334: return 1.6f;
        default: return 0.f;
    }
}

template <typename Type>
constexpr bool nativeOrbOrPad(Type type) {
    return type == Type::YellowJumpRing || type == Type::PinkJumpRing ||
        type == Type::GravityRing || type == Type::GreenRing ||
        type == Type::RedJumpRing || type == Type::DropRing ||
        type == Type::YellowJumpPad || type == Type::PinkJumpPad ||
        type == Type::GravityPad || type == Type::RedJumpPad ||
        type == Type::DashRing || type == Type::GravityDashRing ||
        type == Type::SpiderOrb || type == Type::SpiderPad;
}
template <typename Player, typename Type>
void changeMode(Player* player, Type type) {
    auto wanted = dynamic::portalVehicle(type);
    auto current = dynamic::vehicleFor({player->m_isShip, player->m_isBird,
        player->m_isBall, player->m_isDart, player->m_isRobot,
        player->m_isSpider, player->m_isSwing});
    if (wanted == dynamic::Vehicle::unknown || wanted == current) return;
    // Each native toggle performs the mode's own entry/exit bookkeeping.
    if (player->m_isShip) player->toggleFlyMode(false, true);
    if (player->m_isBird) player->toggleBirdMode(false, true);
    if (player->m_isBall) player->toggleRollMode(false, true);
    if (player->m_isDart) player->toggleDartMode(false, true);
    if (player->m_isRobot) player->toggleRobotMode(false, true);
    if (player->m_isSpider) player->toggleSpiderMode(false, true);
    if (player->m_isSwing) player->toggleSwingMode(false, true);
    player->switchedToMode(type);
    switch (wanted) {
        case dynamic::Vehicle::ship: player->toggleFlyMode(true, true); break;
        case dynamic::Vehicle::ufo: player->toggleBirdMode(true, true); break;
        case dynamic::Vehicle::ball: player->toggleRollMode(true, true); break;
        case dynamic::Vehicle::wave: player->toggleDartMode(true, true); break;
        case dynamic::Vehicle::robot: player->toggleRobotMode(true, true); break;
        case dynamic::Vehicle::spider: player->toggleSpiderMode(true, true); break;
        case dynamic::Vehicle::swing: player->toggleSwingMode(true, true); break;
        default: break;
    }
}

}
