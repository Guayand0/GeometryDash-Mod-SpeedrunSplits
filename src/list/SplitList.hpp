#pragma once

#include "../core/SplitState.hpp"

#include <optional>

namespace speedrun::list {
    size_t checkpointIndexFor(speedrun::core::SplitState& state, CheckpointGameObject* checkpoint);
    void ensureSplitSlot(speedrun::core::SplitState& state, size_t index);
    std::optional<size_t> registerCheckpoint(
        speedrun::core::SplitState& state,
        CheckpointGameObject* checkpoint,
        GJGameLevel* level
    );
    bool registerFinalSplit(speedrun::core::SplitState& state, GJGameLevel* level);
}
