#include "../src/simulation_safety.hpp"
#include <cassert>
#include <stdexcept>
#include <vector>

enum class GhostType { Disabled, Enabled };
struct Checkpoint {
    GhostType m_ghostType = GhostType::Enabled;
    bool m_playEffects = true;
    bool m_isHidden = false;
    double velocity = -12.5;
    bool ship = true;
};

struct Node {
    struct Children {
        std::vector<Node*> nodes;
        unsigned count() const { return static_cast<unsigned>(nodes.size()); }
        Node* objectAtIndex(unsigned i) { return nodes.at(i); }
    } children;
    int callbacks = 1;
    int actions = 1;
    bool paused = false;
    void unscheduleAllSelectors() { callbacks = 0; }
    void stopAllActions() { actions = 0; }
    void pauseSchedulerAndActions() { paused = true; }
    Children* getChildren() { return &children; }
    void resume() { paused = false; }
    bool canRunVisualWork() const { return !paused && (callbacks || actions); }
};

int main() {
    Checkpoint live;
    auto prediction = live;
    dynamic::preparePredictionCheckpoint(prediction);
    assert(prediction.m_ghostType == GhostType::Disabled);
    assert(!prediction.m_playEffects && prediction.m_isHidden);
    assert(prediction.velocity == live.velocity && prediction.ship == live.ship);
    assert(live.m_ghostType == GhostType::Enabled && live.m_playEffects && !live.m_isHidden);

    Node root, child, nested, realPlayer;
    root.children.nodes.push_back(&child);
    child.children.nodes.push_back(&nested);
    bool simulating = false;
    unsigned cleaned = 0;
    auto attempt = [&](bool throws) {
        dynamic::PredictionScope guard(simulating, [&] {
            assert(simulating); // effects remain suppressed during teardown
            dynamic::stopPredictionNode(&root);
            ++cleaned;
        });
        assert(simulating);
        // Native checkpoint/mode code may register timers on a fresh child.
        nested.callbacks = 2;
        nested.actions = 3;
        if (throws) throw std::runtime_error("incomplete candidate");
        return false; // death, deadline and unknown interactions all exit here
    };
    assert(!attempt(false));
    assert(!simulating && cleaned == 1);
    for (auto* node : {&root, &child, &nested}) {
        assert(node->paused && !node->callbacks && !node->actions);
        node->resume();
        assert(!node->canRunVisualWork());
    }
    assert(realPlayer.canRunVisualWork());
    try { attempt(true); assert(false); } catch (std::runtime_error const&) {}
    assert(!simulating && cleaned == 2 && !nested.callbacks && !nested.actions);
    dynamic::stopPredictionNode<Node>(nullptr);
}
