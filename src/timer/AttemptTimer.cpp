#include "AttemptTimer.hpp"

#include <algorithm>
#include <utility>

namespace speedrun::timer {
    void initializeState(speedrun::core::SplitState& state, std::vector<double> targets) {
        state.m_targetSplits = std::move(targets);
        state.m_reachedSplits.assign(state.m_targetSplits.size(), -1.0);
        state.m_reachedCheckpoints.assign(state.m_targetSplits.size(), false);
        state.m_splitResults.assign(state.m_targetSplits.size(), 0);
    }

    void tick(speedrun::core::SplitState& state, float dt, bool isPaused, bool isGameplayActive) {
        if (!state.m_active) {
            return;
        }

        if (!isPaused && isGameplayActive) {
            state.m_runTime += dt;
        }
    }

    void resetAttemptState(speedrun::core::SplitState& state) {
        state.m_runTime = 0.0;
        state.m_active = true;
        std::fill(state.m_reachedSplits.begin(), state.m_reachedSplits.end(), -1.0);
        std::fill(state.m_reachedCheckpoints.begin(), state.m_reachedCheckpoints.end(), false);
        std::fill(state.m_splitResults.begin(), state.m_splitResults.end(), 0);
        state.m_finalReachedSplit = -1.0;
        state.m_finalSplitResult = 0;
        state.m_finalReached = false;
    }

    bool hasReachedCheckpointThisAttempt(speedrun::core::SplitState const& state) {
        return std::any_of(
            state.m_reachedCheckpoints.begin(),
            state.m_reachedCheckpoints.end(),
            [](bool reached) { return reached; }
        );
    }

    void resetAttemptStateIfNoCheckpointReached(speedrun::core::SplitState& state) {
        if (!hasReachedCheckpointThisAttempt(state)) {
            resetAttemptState(state);
        }
    }

    bool shouldShowFinalSplit(speedrun::core::SplitState const& state) {
        return state.m_hasFinalTarget || state.m_finalReached;
    }
}
