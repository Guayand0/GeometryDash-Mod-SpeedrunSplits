#pragma once

#include <Geode/Geode.hpp>

#include <unordered_map>
#include <vector>

namespace speedrun::core {
    struct SplitState {
        double m_runTime = 0.0;
        bool m_active = false;
        std::vector<double> m_targetSplits;
        std::vector<double> m_reachedSplits;
        std::vector<bool> m_reachedCheckpoints;
        std::vector<int> m_splitResults;
        std::unordered_map<CheckpointGameObject*, size_t> m_checkpointIndices;
        double m_finalTargetSplit = 0.0;
        double m_finalReachedSplit = -1.0;
        int m_finalSplitResult = 0;
        bool m_hasFinalTarget = false;
        bool m_finalReached = false;
    };
}
