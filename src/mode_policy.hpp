#pragma once

namespace dynamic {
enum class Vehicle { cube, ship, ufo, ball, wave, robot, spider, swing, unknown };

struct VehicleFlags {
    bool ship = false;
    bool ufo = false;
    bool ball = false;
    bool wave = false;
    bool robot = false;
    bool spider = false;
    bool swing = false;
};

constexpr Vehicle vehicleFor(VehicleFlags f) {
    int active = int(f.ship) + int(f.ufo) + int(f.ball) + int(f.wave) +
        int(f.robot) + int(f.spider) + int(f.swing);
    if (active > 1) return Vehicle::unknown;
    if (f.ship) return Vehicle::ship;
    if (f.ufo) return Vehicle::ufo;
    if (f.ball) return Vehicle::ball;
    if (f.wave) return Vehicle::wave;
    if (f.robot) return Vehicle::robot;
    if (f.spider) return Vehicle::spider;
    if (f.swing) return Vehicle::swing;
    return Vehicle::cube;
}

// GD decrements this timer past zero; only positive values indicate a live force.
constexpr bool hasActiveForce(int remainingTicks, bool affectedByForces) {
    return remainingTicks > 0 || affectedByForces;
}

// Native checkpoints restore slope contacts and velocities for every vehicle.
// The collision engine now resolves slopes for the candidate player itself.
enum class SlopeDecision { simulate, ignore, unknown };
constexpr SlopeDecision slopeDecision(bool, bool) {
    return SlopeDecision::simulate;
}

struct PredictionContext {
    Vehicle vehicle = Vehicle::unknown;
    bool enabled = false;
    bool dead = false;
    bool dashing = false;
    bool dual = false;
    bool platformer = false;
    bool controlsLocked = false;
    bool onSlope = false;
    bool underForce = false;
};

enum class BlockReason { none, disabled, unknownVehicle, dead, dashing, dual, platformer, controlsLocked, slope, force };

// Explain the same eligibility policy used for input passthrough.
constexpr BlockReason blockReason(PredictionContext c) {
    if (!c.enabled) return BlockReason::disabled;
    if (c.vehicle == Vehicle::unknown) return BlockReason::unknownVehicle;
    if (c.dead) return BlockReason::dead;
    // Dash state and its private ring are restored from the native checkpoint.
    if (c.dual) return BlockReason::dual;
    if (c.platformer) return BlockReason::platformer;
    if (c.controlsLocked) return BlockReason::controlsLocked;
    // Existing slope/force state is restored by the native checkpoint.
    return BlockReason::none;
}

constexpr bool canPredict(PredictionContext c) {
    return blockReason(c) == BlockReason::none;
}

constexpr bool pendingInvalidated(PredictionContext c, Vehicle savedVehicle) {
    return !canPredict(c) || c.vehicle != savedVehicle;
}
}
