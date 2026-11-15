#pragma once

#include <utility>

namespace dynamic {
// A native checkpoint contains visual state too. Never replay a live player's
// ghost-trail setting on the private physics player.
template <typename Checkpoint>
void preparePredictionCheckpoint(Checkpoint& checkpoint) {
    checkpoint.m_ghostType = decltype(checkpoint.m_ghostType)::Disabled;
    checkpoint.m_playEffects = false;
    checkpoint.m_isHidden = true;
}

// Pausing alone leaves timers registered. A native mode change can resume a
// node later, so remove callbacks and actions from the entire private tree.
template <typename Node>
void stopPredictionNode(Node* node) {
    if (!node) return;
    node->unscheduleAllSelectors();
    node->stopAllActions();
    node->pauseSchedulerAndActions();
    auto* children = node->getChildren();
    if (!children) return;
    for (unsigned i = 0; i < children->count(); ++i)
        stopPredictionNode(static_cast<Node*>(children->objectAtIndex(i)));
}

template <typename Cleanup>
class PredictionScope {
    bool& m_active;
    Cleanup m_cleanup;
public:
    PredictionScope(bool& active, Cleanup cleanup)
      : m_active(active), m_cleanup(std::move(cleanup)) { m_active = true; }
    PredictionScope(PredictionScope const&) = delete;
    PredictionScope& operator=(PredictionScope const&) = delete;
    ~PredictionScope() {
        m_cleanup();
        m_active = false;
    }
};
}
