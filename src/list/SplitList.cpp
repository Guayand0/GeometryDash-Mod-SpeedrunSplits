#include "SplitList.hpp"

#include "../utils/Utils.hpp"

#include <cmath>

using namespace geode::prelude;

namespace speedrun::list {

    size_t checkpointIndexFor(speedrun::core::SplitState& state, CheckpointGameObject* checkpoint) {
        if (auto it = state.m_checkpointIndices.find(checkpoint); it != state.m_checkpointIndices.end()) {
            return it->second;
        }

        auto nextIndex = state.m_checkpointIndices.size();
        state.m_checkpointIndices.emplace(checkpoint, nextIndex);
        return nextIndex;
    }

    void ensureSplitSlot(speedrun::core::SplitState& state, size_t index) {
        while (state.m_targetSplits.size() <= index) {
            state.m_targetSplits.push_back(0.0);
        }

        while (state.m_reachedSplits.size() <= index) {
            state.m_reachedSplits.push_back(-1.0);
        }

        while (state.m_reachedCheckpoints.size() <= index) {
            state.m_reachedCheckpoints.push_back(false);
        }

        while (state.m_splitResults.size() <= index) {
            state.m_splitResults.push_back(0);
        }
    }

    std::optional<size_t> registerCheckpoint(
        speedrun::core::SplitState& state,
        CheckpointGameObject* checkpoint,
        GJGameLevel* level
    ) {
        auto checkpointIndex = checkpointIndexFor(state, checkpoint);
        ensureSplitSlot(state, checkpointIndex);

        if (state.m_reachedCheckpoints.at(checkpointIndex)) {
            return std::nullopt;
        }

        state.m_reachedCheckpoints[checkpointIndex] = true;
        state.m_reachedSplits[checkpointIndex] = state.m_runTime;

        auto previousTarget = state.m_targetSplits[checkpointIndex];
        auto isNewTarget = previousTarget <= 0.0;
        auto result = 0;

        if (isNewTarget) {
            result = 0;
        }
        else if (std::abs(state.m_runTime - previousTarget) <= speedrun::utils::kTimeEqualityEpsilon) {
            result = 0;
        }
        else if (state.m_runTime < previousTarget) {
            result = -1;
        }
        else {
            result = 1;
        }

        if (isNewTarget || state.m_runTime < previousTarget - speedrun::utils::kTimeEqualityEpsilon) {
            state.m_targetSplits[checkpointIndex] = state.m_runTime;
            Mod::get()->setSavedValue(
                speedrun::utils::levelKey(level),
                speedrun::utils::serializeTargets(state.m_targetSplits)
            );
        }

        state.m_splitResults[checkpointIndex] = result;
        return checkpointIndex;
    }

    bool registerFinalSplit(speedrun::core::SplitState& state, GJGameLevel* level) {
        if (state.m_finalReached) {
            return false;
        }

        state.m_active = false;
        state.m_finalReached = true;
        state.m_finalReachedSplit = state.m_runTime;

        auto previousTarget = state.m_finalTargetSplit;
        auto isNewTarget = !state.m_hasFinalTarget || previousTarget <= 0.0;
        auto result = 0;

        if (isNewTarget) {
            result = 0;
        }
        else if (std::abs(state.m_runTime - previousTarget) <= speedrun::utils::kTimeEqualityEpsilon) {
            result = 0;
        }
        else if (state.m_runTime < previousTarget) {
            result = -1;
        }
        else {
            result = 1;
        }

        if (isNewTarget || state.m_runTime < previousTarget - speedrun::utils::kTimeEqualityEpsilon) {
            state.m_finalTargetSplit = state.m_runTime;
            state.m_hasFinalTarget = true;
            Mod::get()->setSavedValue(
                speedrun::utils::finalLevelKey(level),
                state.m_finalTargetSplit
            );
        }

        state.m_finalSplitResult = result;
        return true;
    }
}
