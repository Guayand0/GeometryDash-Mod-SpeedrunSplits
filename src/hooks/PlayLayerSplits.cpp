#include "PlayLayerHooks.hpp"

#include "../config/ModConfig.hpp"
#include "../core/SplitState.hpp"
#include "../list/SplitList.hpp"
#include "../timer/AttemptTimer.hpp"
#include "../ui/SplitUI.hpp"
#include "../ui/SplitViewState.hpp"
#include "../utils/Utils.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(SpeedrunSplitsPlayLayer, PlayLayer) {
public:
    struct Fields {
        speedrun::core::SplitState m_splitState;
        speedrun::ui::SplitViewState m_splitView;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) {
            return false;
        }

        if (!this->m_isPlatformer) {
            return true;
        }

        speedrun::timer::initializeState(
            m_fields->m_splitState,
            speedrun::utils::loadStoredTargets(level)
        );
        m_fields->m_splitState.m_finalTargetSplit = speedrun::utils::loadStoredFinalTarget(level);
        m_fields->m_splitState.m_hasFinalTarget = m_fields->m_splitState.m_finalTargetSplit > 0.0;

        this->syncUIState();
        this->resetAttemptState();

        return true;
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        if (!this->m_isPlatformer) {
            return;
        }

        this->reloadStoredSplitTargets();
        this->syncUIState();

        if (!speedrun::config::modEnabled() || !m_fields->m_splitState.m_active) {
            return;
        }

        speedrun::timer::tick(m_fields->m_splitState, dt, this->m_isPaused, this->isGameplayActive());
        this->refreshTimerLabel();
    }

    void fullReset() {
        PlayLayer::fullReset();

        if (!this->m_isPlatformer) {
            return;
        }

        this->resetAttemptState();
    }

    void resetLevel() {
        PlayLayer::resetLevel();

        if (!this->m_isPlatformer) {
            return;
        }

        this->resetAttemptStateIfNoCheckpointReached();
    }

    void resetLevelFromStart() {
        PlayLayer::resetLevelFromStart();

        if (!this->m_isPlatformer) {
            return;
        }

        this->resetAttemptState();
    }

    void resumeAndRestart(bool fromStart) {
        PlayLayer::resumeAndRestart(fromStart);

        if (!this->m_isPlatformer || !fromStart) {
            return;
        }

        this->resetAttemptState();
    }

    void onExit() {
        speedrun::ui::destroySplitUI(m_fields->m_splitView);
        PlayLayer::onExit();
    }

    void levelComplete() {
        if (this->m_isPlatformer && speedrun::config::modEnabled()) {
            if (speedrun::list::registerFinalSplit(m_fields->m_splitState, this->m_level)) {
                speedrun::ui::syncUIState(this, m_fields->m_splitState, m_fields->m_splitView);
                this->refreshSplitLabel(m_fields->m_splitState.m_targetSplits.size(), true);
            }
        }

        PlayLayer::levelComplete();
    }

    void registerCheckpoint(CheckpointGameObject* checkpoint) {
        if (!this->m_isPlatformer || !checkpoint || !speedrun::config::modEnabled()) {
            return;
        }

        this->syncUIState();

        if (auto checkpointIndex = speedrun::list::registerCheckpoint(
            m_fields->m_splitState,
            checkpoint,
            this->m_level
        )) {
            speedrun::ui::syncUIState(this, m_fields->m_splitState, m_fields->m_splitView);
            this->refreshSplitLabel(*checkpointIndex, true);
        }
    }

private:
    void reloadStoredSplitTargets() {
        m_fields->m_splitState.m_targetSplits = speedrun::utils::loadStoredTargets(this->m_level);
        m_fields->m_splitState.m_finalTargetSplit =
            speedrun::utils::loadStoredFinalTarget(this->m_level);
        m_fields->m_splitState.m_hasFinalTarget = m_fields->m_splitState.m_finalTargetSplit > 0.0;

        speedrun::ui::syncUIState(this, m_fields->m_splitState, m_fields->m_splitView);
        speedrun::ui::refreshAttemptState(m_fields->m_splitState, m_fields->m_splitView);
    }

    void resetAttemptStateIfNoCheckpointReached() {
        speedrun::timer::resetAttemptStateIfNoCheckpointReached(m_fields->m_splitState);
        speedrun::ui::refreshAttemptState(m_fields->m_splitState, m_fields->m_splitView);
    }

    void syncUIState() {
        speedrun::ui::syncUIState(this, m_fields->m_splitState, m_fields->m_splitView);
    }

    void resetAttemptState() {
        speedrun::timer::resetAttemptState(m_fields->m_splitState);
        speedrun::ui::refreshAttemptState(m_fields->m_splitState, m_fields->m_splitView);
    }

    void refreshTimerLabel() {
        speedrun::ui::refreshTimerLabel(m_fields->m_splitState, m_fields->m_splitView);
    }

    void refreshSplitLabel(size_t index, bool justReached) {
        speedrun::ui::refreshSplitLabel(m_fields->m_splitState, m_fields->m_splitView, index, justReached);
    }
};

namespace speedrun::hooks {
    void registerCheckpointForPlayLayer(PlayLayer* playLayer, CheckpointGameObject* checkpoint) {
        if (!playLayer || !checkpoint || !playLayer->m_isPlatformer) {
            return;
        }

        static_cast<SpeedrunSplitsPlayLayer*>(playLayer)->registerCheckpoint(checkpoint);
    }
}
