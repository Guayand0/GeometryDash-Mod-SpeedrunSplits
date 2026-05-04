#pragma once

#include "../core/SplitState.hpp"

namespace speedrun::timer {
    void initializeState(speedrun::core::SplitState& state, std::vector<double> targets);
    void tick(speedrun::core::SplitState& state, float dt, bool isPaused, bool isGameplayActive);
    void resetAttemptState(speedrun::core::SplitState& state);
    void resetAttemptStateIfNoCheckpointReached(speedrun::core::SplitState& state);
    bool hasReachedCheckpointThisAttempt(speedrun::core::SplitState const& state);
    bool shouldShowFinalSplit(speedrun::core::SplitState const& state);
}
