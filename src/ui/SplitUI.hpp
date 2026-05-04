#pragma once

#include "../core/SplitState.hpp"
#include "SplitViewState.hpp"

#include <Geode/binding/PlayLayer.hpp>

namespace speedrun::ui {
    void syncUIState(PlayLayer* layer, speedrun::core::SplitState const& state, SplitViewState& view);
    void destroySplitUI(SplitViewState& view);
    void refreshTimerLabel(speedrun::core::SplitState const& state, SplitViewState& view);
    void refreshAttemptState(speedrun::core::SplitState const& state, SplitViewState& view);
    void refreshSplitLabel(speedrun::core::SplitState const& state, SplitViewState& view, size_t index, bool justReached);
}
