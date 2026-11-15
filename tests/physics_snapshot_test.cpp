#include "../src/physics_snapshot.hpp"
#include <cassert>
#include <map>

// Engine-independent stand-in for the fields the snapshot copies.
struct Player {
    int m_isOnGround = 0, m_isOnGround2 = 0, m_isOnGround3 = 0, m_isOnGround4 = 0;
    int m_stateOnGround = 0, m_jumpBuffered = 0, m_wasJumpBuffered = 0;
    int m_wasRobotJump = 0, m_stateJumpBuffered = 0, m_stateRingJump = 0, m_stateRingJump2 = 0;
    int m_touchedRing = 0, m_touchedCustomRing = 0, m_touchedPad = 0, m_touchedGravityPortal = 0;
    int m_ringJumpRelated = 0, m_stateNoAutoJump = 0, m_gravity = 0, m_gravityMod = 0;
    int m_yStart = 0, m_speedMultiplier = 0, m_yVelocity = 0, m_fallSpeed = 0;
    int m_groundYVelocity = 0, m_yVelocityBeforeSlope = 0, m_yVelocityRelated = 0, m_yVelocityRelated3 = 0;
    int m_totalTime = 0, m_gameModeChangedTime = 0, m_lastJumpTime = 0, m_lastLandTime = 0, m_lastFlipTime = 0;
    int m_lastSpiderFlipTime = 0, m_maybeIsFalling = 0, m_isAccelerating = 0, m_fixRobotJump = 0;
    int m_maybeIsBoosted = 0, m_accelerationOrSpeed = 0;
    int m_lastGroundedPos = 0, m_stateHitHead = 0, m_stateFlipGravity = 0, m_stateBoostX = 0;
    int m_stateBoostY = 0, m_stateForce = 0, m_stateForceVector = 0, m_affectedByForces = 0;
    int m_controlsDisabled = 0, m_inputsLocked = 0;
    std::map<int, bool> m_holdingButtons;
    bool m_enable22Changes = false, m_enableImpulseFix = false, m_gv0123 = false;
    int modeMarker = 9; // mode is selected separately by the game
};

int main() {
    Player real, ghost;
    real.m_isOnGround = 1;
    real.m_jumpBuffered = 1;
    real.m_gravity = 3;
    real.m_gravityMod = 2;
    real.m_yVelocity = -7;
    real.m_totalTime = 12;
    real.m_gameModeChangedTime = 3;
    real.m_maybeIsBoosted = 1;
    real.m_accelerationOrSpeed = 5;
    real.m_holdingButtons[1] = true;
    real.m_enable22Changes = real.m_enableImpulseFix = real.m_gv0123 = true;
    ghost.modeMarker = 17;
    dynamic::copyGroundAndVelocityState(ghost, real);
    assert(ghost.m_isOnGround && ghost.m_jumpBuffered);
    assert(ghost.m_gravity == 3 && ghost.m_gravityMod == 2 && ghost.m_yVelocity == -7);
    assert(ghost.m_totalTime == 12 && ghost.m_gameModeChangedTime == 3 && ghost.m_holdingButtons.at(1));
    assert(ghost.m_maybeIsBoosted == 1 && ghost.m_accelerationOrSpeed == 5);
    assert(ghost.modeMarker == 17);
    assert(ghost.m_enable22Changes && ghost.m_enableImpulseFix && ghost.m_gv0123);

    // A second candidate must replace stale data from the first simulation.
    real = Player{};
    dynamic::copyGroundAndVelocityState(ghost, real);
    assert(!ghost.m_isOnGround && !ghost.m_jumpBuffered && ghost.m_yVelocity == 0);
    assert(ghost.m_gameModeChangedTime == 0);
    assert(ghost.m_holdingButtons.empty() && ghost.modeMarker == 17);
    assert(!ghost.m_maybeIsBoosted && ghost.m_accelerationOrSpeed == 0);
    assert(!ghost.m_enable22Changes && !ghost.m_enableImpulseFix && !ghost.m_gv0123);
}
