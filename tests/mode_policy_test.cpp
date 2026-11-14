#include "../src/mode_policy.hpp"
#include <array>
#include <cassert>
using namespace dynamic;
int main() {
    const std::array flags = {
        VehicleFlags{}, VehicleFlags{.ship=true}, VehicleFlags{.ufo=true},
        VehicleFlags{.ball=true}, VehicleFlags{.wave=true}, VehicleFlags{.robot=true},
        VehicleFlags{.spider=true}, VehicleFlags{.swing=true}
    };
    const std::array expected = {Vehicle::cube, Vehicle::ship, Vehicle::ufo,
        Vehicle::ball, Vehicle::wave, Vehicle::robot, Vehicle::spider, Vehicle::swing};
    for (unsigned i = 0; i < flags.size(); ++i) {
        auto mode = vehicleFor(flags[i]);
        assert(mode == expected[i]);
        assert(canPredict({.vehicle=mode, .enabled=true}));
        assert(blockReason({.vehicle=mode, .enabled=true}) == BlockReason::none);
        assert(blockReason({.vehicle=mode, .enabled=false}) == BlockReason::disabled);
        assert(blockReason({.vehicle=mode, .enabled=true, .dead=true}) == BlockReason::dead);
        assert(blockReason({.vehicle=mode, .enabled=true, .dashing=true}) == BlockReason::none);
        assert(blockReason({.vehicle=mode, .enabled=true, .dual=true}) == BlockReason::dual);
        assert(blockReason({.vehicle=mode, .enabled=true, .platformer=true}) == BlockReason::platformer);
        assert(blockReason({.vehicle=mode, .enabled=true, .controlsLocked=true}) == BlockReason::controlsLocked);
        if (mode != Vehicle::wave) {
            assert(blockReason({.vehicle=mode, .enabled=true, .onSlope=true}) == BlockReason::none);
            assert(blockReason({.vehicle=mode, .enabled=true, .underForce=true}) == BlockReason::none);
        }
        assert(!pendingInvalidated({.vehicle=mode, .enabled=true}, mode));
        assert(pendingInvalidated({.vehicle=mode, .enabled=false}, mode));
        auto other = mode == Vehicle::cube ? Vehicle::ship : Vehicle::cube;
        assert(pendingInvalidated({.vehicle=mode, .enabled=true}, other));
        assert(!canPredict({.vehicle=mode, .enabled=false}));
        assert(!canPredict({.vehicle=mode, .enabled=true, .dead=true}));
        assert(canPredict({.vehicle=mode, .enabled=true, .dashing=true}));
        assert(!canPredict({.vehicle=mode, .enabled=true, .dual=true}));
        assert(!canPredict({.vehicle=mode, .enabled=true, .platformer=true}));
        assert(!canPredict({.vehicle=mode, .enabled=true, .controlsLocked=true}));
        assert(canPredict({.vehicle=mode, .enabled=true, .onSlope=true}) == true);
        assert(canPredict({.vehicle=mode, .enabled=true, .underForce=true}) == true);
    }
    // Check the policy across combinations with checkpoint-backed dash support, including
    // several simultaneous reasons and wave's slope/force exception.
    for (auto mode : expected) {
        for (unsigned bits = 0; bits < 256; ++bits) {
            PredictionContext c{mode, bool(bits & 1), bool(bits & 2), bool(bits & 4),
                bool(bits & 8), bool(bits & 16), bool(bits & 32), bool(bits & 64), bool(bits & 128)};
            bool original = c.enabled && !c.dead && !c.dual &&
                !c.platformer && !c.controlsLocked;
            assert(canPredict(c) == original);
            assert((blockReason(c) == BlockReason::none) == original);
        }
    }
    assert(slopeDecision(true, false) == SlopeDecision::simulate);
    assert(slopeDecision(true, true) == SlopeDecision::simulate);
    assert(slopeDecision(false, false) == SlopeDecision::simulate);
    assert(slopeDecision(false, true) == SlopeDecision::simulate);
    // The engine counts force timers down below zero after they expire.
    assert(!hasActiveForce(-100, false));
    assert(!hasActiveForce(-1, false));
    assert(!hasActiveForce(0, false));
    assert(hasActiveForce(1, false));
    assert(hasActiveForce(2, false));
    assert(hasActiveForce(-1, true));
    assert(canPredict({.vehicle=Vehicle::cube, .enabled=true,
        .underForce=hasActiveForce(-1, false)}));
    assert(canPredict({.vehicle=Vehicle::cube, .enabled=true,
        .underForce=hasActiveForce(1, false)}));
    assert(canPredict({.vehicle=Vehicle::cube, .enabled=true,
        .underForce=hasActiveForce(-1, true)}));
    assert(vehicleFor({.ship=true, .wave=true}) == Vehicle::unknown);
    assert(!canPredict({.vehicle=Vehicle::unknown, .enabled=true}));
    assert(blockReason({.vehicle=Vehicle::unknown, .enabled=true}) == BlockReason::unknownVehicle);
}
