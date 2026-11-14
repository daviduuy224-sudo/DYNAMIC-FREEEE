#include "../src/interaction_policy.hpp"
#include <array>
#include <cassert>

enum class Type { CubePortal=6, ShipPortal=5, UfoPortal=19, BallPortal=16,
    WavePortal=26, RobotPortal=27, SpiderPortal=33, SwingPortal=41,
    YellowJumpRing=11, PinkJumpRing=12, GravityRing=13, GreenRing=29,
    RedJumpRing=35, DropRing=32, YellowJumpPad=8, PinkJumpPad=9,
    GravityPad=10, RedJumpPad=34, DashRing=37, GravityDashRing=38, SpiderOrb=43, SpiderPad=44 };
struct Player {
    bool m_isShip=false, m_isBird=false, m_isBall=false, m_isDart=false,
        m_isRobot=false, m_isSpider=false, m_isSwing=false;
    double velocity = -7.5;
    bool gravity = true;
    float size = .6f;
    int changes = 0;
    void switchedToMode(Type) { ++changes; }
#define TOGGLE(method, flag) void method(bool on, bool noEffects) { assert(noEffects); flag=on; }
    TOGGLE(toggleFlyMode, m_isShip)
    TOGGLE(toggleBirdMode, m_isBird)
    TOGGLE(toggleRollMode, m_isBall)
    TOGGLE(toggleDartMode, m_isDart)
    TOGGLE(toggleRobotMode, m_isRobot)
    TOGGLE(toggleSpiderMode, m_isSpider)
    TOGGLE(toggleSwingMode, m_isSwing)
#undef TOGGLE
    dynamic::Vehicle vehicle() const {
        return dynamic::vehicleFor({m_isShip,m_isBird,m_isBall,m_isDart,m_isRobot,m_isSpider,m_isSwing});
    }
};
int main() {
    using namespace dynamic;
    constexpr std::array modes{Type::CubePortal,Type::ShipPortal,Type::UfoPortal,
        Type::BallPortal,Type::WavePortal,Type::RobotPortal,Type::SpiderPortal,Type::SwingPortal};
    // Every pair, including a repeated target. In particular a candidate that
    // crosses wave -> ship must not retain the wave flag on the next restore.
    for (auto source : modes) for (auto target : modes) {
        Player live, ghost;
        changeMode(&ghost, source);
        changeMode(&ghost, target);
        assert(ghost.vehicle() == portalVehicle(target));
        auto calls = ghost.changes;
        changeMode(&ghost, target);
        assert(ghost.changes == calls);
        changeMode(&ghost, source);
        assert(ghost.vehicle() == portalVehicle(source));
        assert(live.vehicle() == Vehicle::cube && live.changes == 0);
        assert(ghost.velocity == -7.5 && ghost.gravity && ghost.size == .6f);
    }
    Player p;
    changeMode(&p, static_cast<Type>(999));
    assert(p.vehicle()==Vehicle::cube && p.changes==0);
    assert(portalSpeed(200)==.7f && portalSpeed(201)==.9f && portalSpeed(202)==1.1f);
    assert(portalSpeed(203)==1.3f && portalSpeed(1334)==1.6f);
    assert(portalSpeed(0)==0 && portalSpeed(204)==0 && portalSpeed(9999)==0);
    for (int t=-1;t<50;++t) {
        bool expected=t==8||t==9||t==10||t==11||t==12||t==13||t==29||t==32||t==34||t==35||t==37||t==38||t==43||t==44;
        assert(nativeOrbOrPad(static_cast<Type>(t)) == expected);
    }
}
