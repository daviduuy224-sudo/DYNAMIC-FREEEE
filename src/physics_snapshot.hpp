#pragma once

namespace dynamic {
// Copy scalar physics state that PlayerObject::copyAttributes omits. The game
// supplies mode changes and held-jump state; world objects are not shared here.
template <typename Player>
void copyGroundAndVelocityState(Player& dst, Player const& src) {
#define COPY_FIELD(name) dst.name = src.name
    COPY_FIELD(m_isOnGround); COPY_FIELD(m_isOnGround2);
    COPY_FIELD(m_isOnGround3); COPY_FIELD(m_isOnGround4);
    COPY_FIELD(m_stateOnGround);
    COPY_FIELD(m_jumpBuffered); COPY_FIELD(m_wasJumpBuffered);
    COPY_FIELD(m_wasRobotJump); COPY_FIELD(m_stateJumpBuffered);
    COPY_FIELD(m_stateRingJump); COPY_FIELD(m_stateRingJump2);
    COPY_FIELD(m_touchedRing); COPY_FIELD(m_touchedCustomRing);
    COPY_FIELD(m_touchedPad); COPY_FIELD(m_touchedGravityPortal);
    COPY_FIELD(m_ringJumpRelated); COPY_FIELD(m_stateNoAutoJump);
    COPY_FIELD(m_gravity); COPY_FIELD(m_gravityMod);
    COPY_FIELD(m_yStart); COPY_FIELD(m_speedMultiplier);
    COPY_FIELD(m_yVelocity); COPY_FIELD(m_fallSpeed);
    COPY_FIELD(m_groundYVelocity); COPY_FIELD(m_yVelocityBeforeSlope);
    COPY_FIELD(m_yVelocityRelated); COPY_FIELD(m_yVelocityRelated3);
    COPY_FIELD(m_totalTime); COPY_FIELD(m_gameModeChangedTime); COPY_FIELD(m_lastJumpTime);
    COPY_FIELD(m_lastLandTime); COPY_FIELD(m_lastFlipTime);
    COPY_FIELD(m_lastSpiderFlipTime); COPY_FIELD(m_maybeIsFalling);
    COPY_FIELD(m_isAccelerating); COPY_FIELD(m_fixRobotJump);
    COPY_FIELD(m_maybeIsBoosted); COPY_FIELD(m_accelerationOrSpeed);
    COPY_FIELD(m_lastGroundedPos); COPY_FIELD(m_stateHitHead);
    COPY_FIELD(m_stateFlipGravity); COPY_FIELD(m_stateBoostX);
    COPY_FIELD(m_stateBoostY); COPY_FIELD(m_stateForce);
    COPY_FIELD(m_stateForceVector); COPY_FIELD(m_affectedByForces);
    COPY_FIELD(m_controlsDisabled); COPY_FIELD(m_inputsLocked);
    COPY_FIELD(m_holdingButtons);
    // Level/player physics options are not fields of PlayerCheckpoint. A new
    // PlayerObject has defaults that can differ from the live level's rules.
    COPY_FIELD(m_enable22Changes); COPY_FIELD(m_enableImpulseFix);
    COPY_FIELD(m_gv0123);
#undef COPY_FIELD
}
}
