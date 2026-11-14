// The ghost-player collision technique is derived from GD Mega Overlay's
// ShowTrajectory (https://github.com/maxnut/GDMegaOverlay), GPL-3.0-only.
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/RingObject.hpp>
#include <Windows.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "controls.hpp"
#include "collision_policy.hpp"
#include "portal_policy.hpp"
#include "interaction_policy.hpp"
#include "timing.hpp"
#include "input_queue.hpp"
#include "mode_policy.hpp"
#include "physics_snapshot.hpp"

using namespace geode::prelude;
namespace {
using Clock = std::chrono::steady_clock;
enum class SimulationIssue { none, missingPlayer, dashing, noTick, deadline, object, slope, activation, ring };

const char* issueName(SimulationIssue issue) {
    switch (issue) {
        case SimulationIssue::none: return "none";
        case SimulationIssue::missingPlayer: return "missing-player-or-ghost";
        case SimulationIssue::dashing: return "dashing";
        case SimulationIssue::noTick: return "no-physics-tick";
        case SimulationIssue::deadline: return "time-budget";
        case SimulationIssue::object: return "unsupported-object";
        case SimulationIssue::slope: return "unsupported-slope";
        case SimulationIssue::activation: return "unsupported-activation";
        case SimulationIssue::ring: return "ring";
    }
    return "unknown";
}
const char* blockName(dynamic::BlockReason reason) {
    switch (reason) {
        case dynamic::BlockReason::none: return "none";
        case dynamic::BlockReason::disabled: return "window-zero";
        case dynamic::BlockReason::unknownVehicle: return "unknown-vehicle";
        case dynamic::BlockReason::dead: return "player-dead";
        case dynamic::BlockReason::dashing: return "dashing";
        case dynamic::BlockReason::dual: return "dual";
        case dynamic::BlockReason::platformer: return "platformer";
        case dynamic::BlockReason::controlsLocked: return "controls-locked";
        case dynamic::BlockReason::slope: return "slope";
        case dynamic::BlockReason::force: return "force";
    }
    return "unknown";
}
const char* decisionName(dynamic::DecisionReason reason) {
    switch (reason) {
        case dynamic::DecisionReason::disabled: return "disabled";
        case dynamic::DecisionReason::safeNow: return "already-safe";
        case dynamic::DecisionReason::unknownInitial: return "unknown-initial";
        case dynamic::DecisionReason::delayed: return "delayed";
        case dynamic::DecisionReason::unknownCandidate: return "unknown-candidate";
        case dynamic::DecisionReason::noSafeTick: return "no-safe-tick";
    }
    return "unknown";
}
struct State {
    PlayLayer* layer = nullptr;
    CCNode* holder = nullptr;
    PlayerObject* ghost = nullptr;
    dynamic::Vehicle ghostVehicle = dynamic::Vehicle::unknown;
    struct ObjectCopy { Ref<GameObject> object; std::uint64_t generation = 0; };
    std::unordered_map<GameObject*, ObjectCopy> copies;
    std::unordered_set<GameObject*> scratchObjects;
    std::unordered_set<GameObject*> activatedPortals;
    std::uint64_t generation = 0;
    Ref<PlayerCheckpoint> checkpoint;
    bool simulating = false;
    bool simDead = false;
    bool uncertain = false;
    SimulationIssue issue = SimulationIssue::none;
    int issueObjectID = -1;
    int issueObjectType = -1;
    bool injecting = false;
    bool deliveredDown = false;
    bool f6Down = false;
    bool f7Down = false;
    std::int64_t lastWindow = 1;
    std::int64_t displayedWindow = -1;
    bool displayedControls = false;
    CCMenu* controls = nullptr;
    CCLabelBMFont* status = nullptr;
    dynamic::AttemptStats stats;
    std::string lastResult = "Esperando clic";
    std::uint64_t completedSteps = 0;
    std::uint64_t pendingStartStep = 0;
    std::uint8_t pendingDelay = 0;
    float tickDt = 0;
    std::uint64_t tick = 0;
    dynamic::InputQueue pending;
    dynamic::Vehicle pendingVehicle = dynamic::Vehicle::unknown;
} state;

void updateControls() {
    auto window = dynamic::clampWindow(Mod::get()->getSettingValue<int64_t>("window"));
    auto visible = Mod::get()->getSettingValue<bool>("show-controls");
    if (window > 0) state.lastWindow = window;
    state.displayedWindow = window;
    state.displayedControls = visible;
    if (state.controls) state.controls->setVisible(visible);
    if (state.status) {
        state.status->setVisible(visible);
        auto text = fmt::format("Assist: {} | {} ticks (F6/F7)\nCorrecciones: {} | ticks: {} | ultimo: {}\n{}",
            window ? "ON" : "OFF", window, state.stats.corrected,
            state.stats.delayedTicks, state.stats.lastDelay, state.lastResult);
        state.status->setString(text.c_str());
    }
}
void showResult(std::string result) {
    if (state.lastResult == result) return;
    state.lastResult = std::move(result);
    updateControls();
}
void inject(PlayLayer* layer, bool down) {
    state.injecting = true;
    layer->handleButton(down, 1, true);
    state.injecting = false;
    state.deliveredDown = down;
}
void forwardQueued(PlayLayer* layer, dynamic::QueuedInput const& event, char const* reason) {
    state.stats.delivered(state.pendingStartStep, state.completedSteps);
    auto actualDelay = state.completedSteps >= state.pendingStartStep
        ? state.completedSteps - state.pendingStartStep : 0;
    inject(layer, event.down);
    state.lastResult = fmt::format("Entrada aplicada: +{} ticks", actualDelay);
    updateControls();
    if (Mod::get()->getSettingValue<bool>("diagnostics")) {
        log::info("[Dynamic Assist diagnostic] queued input forwarded: reason={} input={} due_tick={} current_tick={}",
            reason, event.down ? "press" : "release", event.due, state.tick);
    }
}
void discardPending(char const* reason) {
    if (!state.pending.empty() && Mod::get()->getSettingValue<bool>("diagnostics")) {
        log::info("[Dynamic Assist diagnostic] pending input discarded: {}", reason);
    }
    state.pending.clear();
    state.pendingDelay = 0;
    state.pendingVehicle = dynamic::Vehicle::unknown;
}
void flush(PlayLayer* layer) {
    while (!state.pending.empty()) {
        auto event = state.pending.pop(state.tick, true);
        forwardQueued(layer, *event, "flush");
    }
    state.pendingDelay = 0;
    state.pendingVehicle = dynamic::Vehicle::unknown;
}
void setWindow(PlayLayer* layer, std::int64_t value) {
    value = dynamic::clampWindow(value);
    auto previous = dynamic::clampWindow(Mod::get()->getSettingValue<int64_t>("window"));
    if (previous > 0) state.lastWindow = previous;
    Mod::get()->setSettingValue<int64_t>("window", value);
    if (!state.pending.empty() && dynamic::pendingExceedsWindow(state.pendingDelay, value)) flush(layer);
    if (previous != value) state.lastResult = "Esperando clic";
    updateControls();
}
void resetState(PlayLayer* layer) {
    state.layer = layer;
    state.simulating = false;
    state.simDead = false;
    state.uncertain = false;
    state.issue = SimulationIssue::none;
    state.injecting = false;
    state.deliveredDown = false;
    state.tick = 0;
    state.tickDt = 0;
    state.completedSteps = 0;
    state.pendingStartStep = 0;
    state.stats = {};
    state.lastResult = "Esperando clic";
    discardPending("level-reset");
    updateControls();
}

dynamic::PredictionContext predictionContext(PlayerObject* p, PlayLayer* layer, bool enabled) {
    if (!p || !layer) return {};
    auto vehicle = dynamic::vehicleFor({p->m_isShip, p->m_isBird, p->m_isBall,
        p->m_isDart, p->m_isRobot, p->m_isSpider, p->m_isSwing});
    return {vehicle, enabled, p->m_isDead, p->m_isDashing,
        layer->m_gameState.m_isDualMode, layer->m_isPlatformer,
        p->m_controlsDisabled || p->m_inputsLocked || p->m_isLocked,
        p->m_isOnSlope || p->m_wasOnSlope,
        dynamic::hasActiveForce(p->m_stateForce, p->m_affectedByForces)};
}
void pauseTree(CCNode* node) {
    node->pauseSchedulerAndActions();
    auto* children = node->getChildren();
    if (!children) return;
    for (unsigned i = 0; i < children->count(); ++i)
        pauseTree(static_cast<CCNode*>(children->objectAtIndex(i)));
}
void ensureGhost(PlayLayer* layer, dynamic::Vehicle vehicle) {
    if (!layer || !layer->m_objectLayer) return;
    if (state.ghost && state.ghostVehicle == vehicle) return;
    if (state.holder) {
        auto* oldHolder = state.holder;
        state.ghost = nullptr;
        state.holder = nullptr;
        state.ghostVehicle = dynamic::Vehicle::unknown;
        oldHolder->removeFromParent();
    }
    auto* holder = CCLayer::create();
    if (!holder) return;
    holder->setVisible(false);
    layer->m_objectLayer->addChild(holder);
    auto* ghost = PlayerObject::create(1, 1, layer, holder, true);
    if (!ghost) {
        holder->removeFromParent();
        return;
    }
    holder->addChild(ghost);
    ghost->setVisible(false);
    pauseTree(holder);
    state.holder = holder;
    state.ghost = ghost;
    state.ghostVehicle = vehicle;
}
// Keep activation state on private objects. Native ring/pad physics may mark
// an object activated, claim its touch, or animate it; none of those writes may
// reach a real level object. Copies are reset once for each candidate.
GameObject* interactionCopy(GameObject* original) {
    auto& entry = state.copies[original];
    if (!entry.object) entry.object = GameObject::createWithKey(original->m_objectID);
    auto* copy = entry.object.data();
    if (!copy) return nullptr;
    state.scratchObjects.insert(copy);
    if (entry.generation == state.generation) return copy;
    entry.generation = state.generation;
    copy->resetObject();
    copy->setPosition(original->getPosition());
    copy->setScaleX(original->getScaleX());
    copy->setScaleY(original->getScaleY());
    copy->setRotation(original->getRotation());
    copy->m_uniqueID = original->m_uniqueID;
    copy->m_objectType = original->m_objectType;
    copy->m_isNoTouch = original->m_isNoTouch;
    copy->m_isRingPoweredOn = original->m_isRingPoweredOn;
    copy->m_isActivated = original->m_isActivated;
    copy->m_hasNoEffects = true;
    copy->m_hasNoParticles = true;
    copy->setVisible(false);
    if (auto* src = typeinfo_cast<EffectGameObject*>(original)) {
        auto* dst = typeinfo_cast<EffectGameObject*>(copy);
        if (!dst) return nullptr;
        dst->m_gravityValue = src->m_gravityValue;
        dst->m_gravityMod = src->m_gravityMod;
        dst->m_isReverse = src->m_isReverse;
        dst->m_targetGroupID = src->m_targetGroupID;
    }
    if (auto* src = typeinfo_cast<EnhancedGameObject*>(original)) {
        auto* dst = typeinfo_cast<EnhancedGameObject*>(copy);
        if (!dst) return nullptr;
        dst->m_isMultiActivate = src->m_isMultiActivate;
        dst->m_isNoMultiActivate = src->m_isNoMultiActivate;
        dst->m_activated = src->m_activated;
        dst->m_activatedByPlayer1 = src->m_activatedByPlayer1;
        dst->m_activatedByPlayer2 = src->m_activatedByPlayer2;
    }
    if (auto* src = typeinfo_cast<RingObject*>(original)) {
        auto* dst = typeinfo_cast<RingObject*>(copy);
        if (!dst) return nullptr;
        dst->m_claimTouch = src->m_claimTouch;
        dst->m_isSpawnOnly = src->m_isSpawnOnly;
    }
    if (auto* src = typeinfo_cast<DashRingObject*>(original)) {
        auto* dst = typeinfo_cast<DashRingObject*>(copy);
        if (!dst) return nullptr;
        dst->m_dashSpeed = src->m_dashSpeed;
        dst->m_endBoost = src->m_endBoost;
        dst->m_maxDuration = src->m_maxDuration;
        dst->m_allowCollide = src->m_allowCollide;
        dst->m_stopSlide = src->m_stopSlide;
    }
    return copy;
}

void clearLogs(PlayerObject* p) {
    if (p->m_collisionLogTop) p->m_collisionLogTop->removeAllObjects();
    if (p->m_collisionLogBottom) p->m_collisionLogBottom->removeAllObjects();
    if (p->m_collisionLogLeft) p->m_collisionLogLeft->removeAllObjects();
    if (p->m_collisionLogRight) p->m_collisionLogRight->removeAllObjects();
}
void button(PlayerObject* p, bool down) {
    if (down) p->pushButton(PlayerButton::Jump);
    else p->releaseButton(PlayerButton::Jump);
}

dynamic::Verdict simulate(PlayLayer* layer, bool down, unsigned delay,
                          Clock::time_point deadline) {
    auto* player = layer->m_player1;
    auto* ghost = state.ghost;
    state.issue = SimulationIssue::none;
    state.issueObjectID = -1;
    state.issueObjectType = -1;
    if (!ghost || !player) { state.issue = SimulationIssue::missingPlayer; return dynamic::Verdict::unknown; }
    if (state.tickDt <= 0) { state.issue = SimulationIssue::noTick; return dynamic::Verdict::unknown; }
    if (Clock::now() >= deadline) { state.issue = SimulationIssue::deadline; return dynamic::Verdict::unknown; }
    state.simulating = true;
    struct SimulationGuard {
        PlayLayer* layer;
        GJGameState saved;
        ~SimulationGuard() {
            layer->m_gameState = std::move(saved);
            state.simulating = false;
        }
    } simulationGuard{layer, layer->m_gameState};
    state.simDead = false;
    state.uncertain = false;
    ++state.generation;
    state.activatedPortals.clear();
    // Use the game's practice checkpoint restore, including all vehicle flags,
    // velocities, slope data, held input and ring bookkeeping. The old partial
    // copy left state from the previous candidate on the hidden player.
    if (!state.checkpoint) { state.issue = SimulationIssue::missingPlayer; return dynamic::Verdict::unknown; }
    ghost->loadFromCheckpoint(state.checkpoint.data());
    dynamic::copyGroundAndVelocityState(*ghost, *player);
    if (player->m_isDashing && player->m_dashRing) {
        ghost->m_dashRing = typeinfo_cast<DashRingObject*>(interactionCopy(player->m_dashRing));
        if (!ghost->m_dashRing) { state.issue = SimulationIssue::dashing; return dynamic::Verdict::unknown; }
    }
    ghost->m_isDead = false;
    ghost->m_playEffects = false;
    ghost->setVisible(false);
    if (!ghost->m_touchingRings || ghost->m_touchingRings == player->m_touchingRings) {
        state.issue = SimulationIssue::ring;
        return dynamic::Verdict::unknown;
    }
    ghost->m_touchingRings->removeAllObjects();
    // A click can arrive while already inside an orb. Rebuild these contacts
    // with private objects before applying the first candidate input.
    if (player->m_touchingRings) {
        for (unsigned i = 0; i < player->m_touchingRings->count(); ++i) {
            auto* ring = static_cast<RingObject*>(player->m_touchingRings->objectAtIndex(i));
            if (!dynamic::nativeOrbOrPad(ring->m_objectType)) {
                state.issue = SimulationIssue::ring;
                return dynamic::Verdict::unknown;
            }
            auto* copy = typeinfo_cast<RingObject*>(interactionCopy(ring));
            if (!copy) { state.issue = SimulationIssue::ring; return dynamic::Verdict::unknown; }
            ghost->m_touchingRings->addObject(copy);
        }
    }
    pauseTree(state.holder);

    constexpr unsigned horizon = 32;
    auto expired = [&] {
        if (Clock::now() < deadline) return false;
        state.issue = SimulationIssue::deadline;
        return true;
    };
    return dynamic::runPredictionSteps(horizon + delay, expired, [&](unsigned i) {
        if (i == delay) button(ghost, down);
        ghost->update(state.tickDt);
        clearLogs(ghost);
        if (!state.simDead && !state.uncertain) {
            layer->checkCollisions(ghost, state.tickDt, false);
        }
        return state.uncertain ? dynamic::Verdict::unknown :
            state.simDead ? dynamic::Verdict::unsafe : dynamic::Verdict::safe;
    });
}
} // namespace

class $modify(DynamicGameLayer, GJBaseGameLayer) {
    void handleButton(bool down, int buttonId, bool player1) {
        auto* layer = PlayLayer::get();
        if (state.simulating || state.injecting || !layer || static_cast<GJBaseGameLayer*>(layer) != static_cast<GJBaseGameLayer*>(this) ||
            state.layer != layer) {
            return GJBaseGameLayer::handleButton(down, buttonId, player1);
        }
        const bool diagnostics = Mod::get()->getSettingValue<bool>("diagnostics");
        if (buttonId != 1 || !player1) {
            if (diagnostics) log::info("[Dynamic Assist diagnostic] ignored button={} player1={}", buttonId, player1);
            return GJBaseGameLayer::handleButton(down, buttonId, player1);
        }
        auto window = dynamic::clampWindow(Mod::get()->getSettingValue<int64_t>("window"));
        auto* player = layer->m_player1;
        if (!player) {
            if (diagnostics) log::info("[Dynamic Assist diagnostic] input ignored: missing-player");
            flush(layer);
            showResult("Sin jugador disponible");
            state.deliveredDown = down;
            return GJBaseGameLayer::handleButton(down, buttonId, player1);
        }
        auto context = predictionContext(player, layer, window > 0);
        if (!dynamic::canPredict(context)) {
            auto reason = dynamic::blockReason(context);
            if (diagnostics) log::info("[Dynamic Assist diagnostic] input ignored: {}", blockName(reason));
            flush(layer);
            showResult(fmt::format("Sin prediccion: {}", blockName(reason)));
            state.deliveredDown = down;
            return GJBaseGameLayer::handleButton(down, buttonId, player1);
        }
        ensureGhost(layer, context.vehicle);
        if (!state.ghost) {
            if (diagnostics) log::info("[Dynamic Assist diagnostic] input ignored: ghost-unavailable");
            flush(layer);
            showResult("Sin prediccion: ghost-unavailable");
            state.deliveredDown = down;
            return GJBaseGameLayer::handleButton(down, buttonId, player1);
        }
        if (!state.pending.empty()) {
            // The prediction assumed the previous button state persists. A new
            // input invalidates it, so stop waiting and preserve event order.
            flush(layer);
            log::info("Dynamic Assist: ended the planned delay on new input");
            if (diagnostics) log::info("[Dynamic Assist diagnostic] pending delay interrupted by another input");
            showResult("Entrada interrumpida por nuevo clic");
            state.deliveredDown = down;
            return GJBaseGameLayer::handleButton(down, buttonId, player1);
        }
        state.checkpoint = PlayerCheckpoint::create();
        if (!state.checkpoint) {
            state.deliveredDown = down;
            return GJBaseGameLayer::handleButton(down, buttonId, player1);
        }
        player->saveToCheckpoint(state.checkpoint.data());
        auto start = Clock::now();
        auto budget = std::clamp<int64_t>(Mod::get()->getSettingValue<int64_t>("budget"), 1, 33);
        auto deadline = start + std::chrono::milliseconds(budget);
        auto decision = dynamic::chooseDelay(static_cast<std::uint8_t>(window), [&](std::uint8_t delay) {
            return simulate(layer, down, delay, deadline);
        });
        if (diagnostics) {
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - start).count();
            log::info("[Dynamic Assist diagnostic] input={} mode={} window={} result={} delay={} attempts={} issue={} object_id={} object_type={} tick_dt={} elapsed_us={}",
                down ? "press" : "release", static_cast<int>(context.vehicle), window, decisionName(decision.reason),
                static_cast<unsigned>(decision.delay), decision.attempts, issueName(state.issue),
                state.issue == SimulationIssue::object ? state.issueObjectID : -1,
                state.issue == SimulationIssue::object ? state.issueObjectType : -1, state.tickDt, elapsed);
        }
        switch (decision.reason) {
            case dynamic::DecisionReason::safeNow:
                showResult("Sin cambio: trayectoria segura");
                break;
            case dynamic::DecisionReason::noSafeTick:
                showResult("Sin alternativa en el margen");
                break;
            case dynamic::DecisionReason::unknownInitial:
            case dynamic::DecisionReason::unknownCandidate:
                if (state.issue == SimulationIssue::object) {
                    showResult(fmt::format("Objeto no compatible: ID {} tipo {}", state.issueObjectID, state.issueObjectType));
                } else {
                    showResult(fmt::format("Prediccion incompleta: {}", issueName(state.issue)));
                }
                break;
            case dynamic::DecisionReason::delayed:
                showResult(fmt::format("Ajuste previsto: +{} ticks", static_cast<unsigned>(decision.delay)));
                break;
            case dynamic::DecisionReason::disabled:
                showResult("Asistencia desactivada");
                break;
        }
        if (decision.delay) {
            state.pendingStartStep = state.completedSteps;
            state.pendingDelay = decision.delay;
            state.pending.addAfterSteps(down, state.tick, decision.delay);
            state.pendingVehicle = context.vehicle;
            log::info("Dynamic Assist: planned delay {} tick(s)", static_cast<unsigned>(decision.delay));
            return;
        }
        state.deliveredDown = down;
        GJBaseGameLayer::handleButton(down, buttonId, player1);
    }

    void collisionCheckObjects(PlayerObject* player, gd::vector<GameObject*>* objects, int count, float dt) {
        if (!state.simulating || player != state.ghost || !objects)
            return GJBaseGameLayer::collisionCheckObjects(player, objects, count, dt);
        gd::vector<GameObject*> filtered;
        auto limit = std::max(0, std::min<int>(count, static_cast<int>(objects->size())));
        filtered.reserve(static_cast<std::size_t>(std::min(limit, 64)));
        for (int i = 0; i < limit; ++i) {
            auto* object = (*objects)[i];
            if (!object || object->m_isGroupDisabled || object->m_isDisabled) continue;
            auto type = object->m_objectType;
            auto category = dynamic::collisionCategory(type);
            if (category == dynamic::CollisionCategory::slope) {
                bool overlaps = !player->m_isDart && (object->m_objectRadius > 0.f
                    ? GJBaseGameLayer::playerCircleCollision(player, object)
                    : object->getObjectRect(2.f, 2.f).intersectsRect(player->getObjectRect()));
                auto slope = dynamic::slopeDecision(player->m_isDart, overlaps);
                if (slope == dynamic::SlopeDecision::simulate) filtered.push_back(object);
                if (slope == dynamic::SlopeDecision::unknown) {
                    state.uncertain = true;
                    state.issue = SimulationIssue::slope;
                }
                continue;
            }
            auto portal = dynamic::portalAction(type);
            auto vehicle = dynamic::portalVehicle(type);
            auto speed = type == GameObjectType::Modifier ? dynamic::portalSpeed(object->m_objectID) : 0.f;
            if (dynamic::nativeOrbOrPad(type)) {
                auto* effect = typeinfo_cast<EffectGameObject*>(object);
                if (effect && effect->m_targetGroupID != 0 &&
                    object->getObjectRect().intersectsRect(player->getObjectRect())) {
                    state.uncertain = true; state.issue = SimulationIssue::activation;
                    return;
                }
                auto* copy = interactionCopy(object);
                if (copy) filtered.push_back(copy);
                else { state.uncertain = true; state.issue = SimulationIssue::object;
                    state.issueObjectID = object->m_objectID; state.issueObjectType = static_cast<int>(type); }
            } else if (portal != dynamic::PortalAction::unsupported ||
                       vehicle != dynamic::Vehicle::unknown || speed > 0.f) {
                // Preserve the engine's object order: resolve preceding solids
                // before changing the candidate's gravity or hitbox size.
                if (!filtered.empty()) {
                    GJBaseGameLayer::collisionCheckObjects(player, &filtered,
                        static_cast<int>(filtered.size()), dt);
                    filtered.clear();
                }
                if (state.simDead || state.uncertain) return;
                bool overlaps = object->m_objectRadius > 0.f
                    ? GJBaseGameLayer::playerCircleCollision(player, object)
                    : object->getObjectRect().intersectsRect(player->getObjectRect());
                if (overlaps && state.activatedPortals.insert(object).second) {
                    dynamic::applyPortal(*player, portal);
                    if (vehicle != dynamic::Vehicle::unknown) {
                        dynamic::changeMode(player, type);
                        // Only candidate game state is changed; the guard restores
                        // it before the real input is delivered.
                        GJBaseGameLayer::processCameraObject(object, player);
                    }
                    if (speed > 0.f) player->m_playerSpeed = speed;
                }
            } else if (category == dynamic::CollisionCategory::geometry) {
                filtered.push_back(object);
            } else if (category == dynamic::CollisionCategory::unsupported &&
                       object->getObjectRect().intersectsRect(player->getObjectRect())) {
                state.uncertain = true;
                state.issue = SimulationIssue::object;
                state.issueObjectID = object->m_objectID;
                state.issueObjectType = static_cast<int>(type);
            }
        }
        GJBaseGameLayer::collisionCheckObjects(player, &filtered, static_cast<int>(filtered.size()), dt);
    }
    void flipGravity(PlayerObject* player, bool flip, bool noEffects) {
        if (state.simulating) {
            if (player == state.ghost) player->flipGravity(flip, true);
            else { state.uncertain = true; state.issue = SimulationIssue::activation; }
            return;
        }
        GJBaseGameLayer::flipGravity(player, flip, noEffects);
    }
    void gameEventTriggered(GJGameEvent event, int material, int playerID) {
        if (!state.simulating) GJBaseGameLayer::gameEventTriggered(event, material, playerID);
    }
    bool canBeActivatedByPlayer(PlayerObject* player, EffectGameObject* object) {
        if (state.simulating) {
            if (player == state.ghost && state.scratchObjects.contains(object))
                return GJBaseGameLayer::canBeActivatedByPlayer(player, object);
            state.uncertain = true;
            state.issue = SimulationIssue::activation;
            return false;
        }
        return GJBaseGameLayer::canBeActivatedByPlayer(player, object);
    }
    void playerTouchedRing(PlayerObject* player, RingObject* ring) {
        if (state.simulating && (player != state.ghost || !state.scratchObjects.contains(ring))) {
            state.uncertain = true; state.issue = SimulationIssue::ring; return;
        }
        GJBaseGameLayer::playerTouchedRing(player, ring);
    }
};

class $modify(DynamicPlayer, PlayerObject) {
    void update(float dt) {
        auto* layer = PlayLayer::get();
        if (!state.simulating && layer && layer == state.layer && this == layer->m_player1) {
            ++state.tick;
            state.tickDt = dynamic::validTickDuration(dt) ? dt : 0.f;
            DWORD foregroundProcess = 0;
            auto* foreground = GetForegroundWindow();
            if (foreground) GetWindowThreadProcessId(foreground, &foregroundProcess);
            bool focused = foregroundProcess == GetCurrentProcessId();
            bool pressed = focused && (GetAsyncKeyState(VK_F6) & 0x8000) != 0;
            if (pressed && !state.f6Down) {
                auto next = dynamic::nextWindow(Mod::get()->getSettingValue<int64_t>("window"));
                setWindow(layer, next);
                log::info("Dynamic Assist window: {} ticks", next);
            }
            state.f6Down = pressed;
            bool toggle = focused && (GetAsyncKeyState(VK_F7) & 0x8000) != 0;
            if (toggle && !state.f7Down) {
                setWindow(layer, dynamic::toggleWindow(Mod::get()->getSettingValue<int64_t>("window"), state.lastWindow));
            }
            state.f7Down = toggle;
            auto displayedWindow = dynamic::clampWindow(Mod::get()->getSettingValue<int64_t>("window"));
            if (state.displayedWindow != displayedWindow) state.lastResult = "Esperando clic";
            if (state.displayedWindow != displayedWindow ||
                state.displayedControls != Mod::get()->getSettingValue<bool>("show-controls")) updateControls();
            auto window = dynamic::clampWindow(Mod::get()->getSettingValue<int64_t>("window"));
            auto context = predictionContext(this, layer, window > 0);
            if (!state.pending.empty() &&
                (state.tickDt == 0.f || dynamic::pendingInvalidated(context, state.pendingVehicle) ||
                 dynamic::pendingExceedsWindow(state.pendingDelay, window)))
                flush(layer);
            while (auto event = state.pending.pop(state.tick)) forwardQueued(layer, *event, "scheduled");
            if (state.pending.empty()) {
                state.pendingDelay = 0;
                state.pendingVehicle = dynamic::Vehicle::unknown;
            }
        }
        PlayerObject::update(dt);
        if (!state.simulating && layer && layer == state.layer && this == layer->m_player1)
            ++state.completedSteps;
    }
    void ringJump(RingObject* ring, bool skipCheck) {
        if (state.simulating && (this != state.ghost || !state.scratchObjects.contains(ring))) {
            state.uncertain = true; state.issue = SimulationIssue::ring; return;
        }
        PlayerObject::ringJump(ring, skipCheck);
    }
    void playBumpEffect(int type, GameObject* object) {
        if (!state.simulating) PlayerObject::playBumpEffect(type, object);
    }
    void incrementJumps() {
        if (!state.simulating) PlayerObject::incrementJumps();
    }
    void playSpiderDashEffect(CCPoint from, CCPoint to) {
        if (!state.simulating) PlayerObject::playSpiderDashEffect(from, to);
    }
};
class $modify(DynamicRing, RingObject) {
    void triggerActivated(float x) {
        if (!state.simulating) RingObject::triggerActivated(x);
    }
};
class $modify(DynamicStreak, HardStreak) {
    void addPoint(CCPoint point) { if (!state.simulating) HardStreak::addPoint(point); }
};
class $modify(DynamicObject, GameObject) {
    void playShineEffect() { if (!state.simulating) GameObject::playShineEffect(); }
};
class $modify(DynamicEffect, EffectGameObject) {
    void triggerObject(GJBaseGameLayer* layer, int id, gd::vector<int> const* keys) {
        if (!state.simulating) EffectGameObject::triggerObject(layer, id, keys);
        else if (this->m_targetGroupID != 0) {
            // A custom group effect needs future world simulation as well.
            state.uncertain = true;
            state.issue = SimulationIssue::activation;
        }
    }
};
class $modify(DynamicPlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool replay, bool skipObjects) {
        if (!PlayLayer::init(level, replay, skipObjects)) return false;
        state = State{};
        resetState(this);
        auto size = CCDirector::sharedDirector()->getWinSize();
        auto* menu = CCMenu::create();
        menu->setPosition({0, 0});
        menu->setID("dynamic-assist-controls");
        this->addChild(menu, 1000);
        state.controls = menu;
        auto* status = CCLabelBMFont::create("", "bigFont.fnt");
        status->setScale(0.28f);
        status->setAnchorPoint({0, 1});
        status->setPosition({8, size.height - 8});
        this->addChild(status, 1001);
        state.status = status;
        auto addControl = [&](char const* text, float x, SEL_MenuHandler callback) {
            auto* label = CCLabelBMFont::create(text, "bigFont.fnt");
            auto* item = CCMenuItemSpriteExtra::create(label, this, callback);
            item->setScale(0.45f);
            item->setPosition({x, size.height - 43});
            menu->addChild(item);
        };
        addControl("ON/OFF", 35, menu_selector(DynamicPlayLayer::onAssistToggle));
        addControl("-", 85, menu_selector(DynamicPlayLayer::onAssistLess));
        addControl("+", 110, menu_selector(DynamicPlayLayer::onAssistMore));
        updateControls();
        return true;
    }
    void onAssistToggle(CCObject*) {
        setWindow(this, dynamic::toggleWindow(Mod::get()->getSettingValue<int64_t>("window"), state.lastWindow));
    }
    void onAssistLess(CCObject*) {
        setWindow(this, dynamic::clampWindow(Mod::get()->getSettingValue<int64_t>("window")) - 1);
    }
    void onAssistMore(CCObject*) {
        setWindow(this, dynamic::clampWindow(Mod::get()->getSettingValue<int64_t>("window")) + 1);
    }
    void resetLevel() {
        resetState(this);
        PlayLayer::resetLevel();
    }
    void onQuit() {
        state.layer = nullptr;
        state.controls = nullptr;
        state.status = nullptr;
        state.ghost = nullptr;
        state.holder = nullptr;
        state.ghostVehicle = dynamic::Vehicle::unknown;
        state.checkpoint = nullptr;
        state.copies.clear();
        state.scratchObjects.clear();
        state.activatedPortals.clear();
        discardPending("quit");
        PlayLayer::onQuit();
    }
    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (state.simulating && player == state.ghost) { state.simDead = true; return; }
        discardPending("player-died");
        PlayLayer::destroyPlayer(player, object);
    }
    void playEndAnimationToPos(CCPoint pos) {
        if (!state.simulating) PlayLayer::playEndAnimationToPos(pos);
    }
};
