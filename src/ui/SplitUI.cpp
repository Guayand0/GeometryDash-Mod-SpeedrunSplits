#include "SplitUI.hpp"

#include "../config/ModConfig.hpp"
#include "../timer/AttemptTimer.hpp"
#include "../utils/Utils.hpp"

namespace {
    constexpr float kTimerListGap = 6.0f;
    constexpr char const* kEmptySplitTime = "--.---";

    size_t totalDisplayEntries(speedrun::core::SplitState const& state) {
        return state.m_targetSplits.size() + (speedrun::timer::shouldShowFinalSplit(state) ? 1u : 0u);
    }

    bool isFinalIndex(speedrun::core::SplitState const& state, size_t index) {
        return speedrun::timer::shouldShowFinalSplit(state) && index == state.m_targetSplits.size();
    }

    void createSplitRoot(PlayLayer* layer, speedrun::ui::SplitViewState& view) {
        if (auto oldRoot = layer->getChildByID(speedrun::config::kSplitRootId)) {
            oldRoot->removeFromParent();
        }

        auto root = cocos2d::CCNode::create();
        root->setID(speedrun::config::kSplitRootId);
        root->setZOrder(1000);
        layer->addChild(root);

        auto timerLabel = cocos2d::CCLabelBMFont::create("00.00", "goldFont.fnt");
        root->addChild(timerLabel);

        view.m_splitRoot = root;
        view.m_timerLabel = timerLabel;
        view.m_splitLabels.clear();
    }

    void updateUILayout(speedrun::ui::SplitViewState& view) {
        if (!view.m_splitRoot || !view.m_timerLabel) {
            return;
        }

        auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        auto position = speedrun::config::uiPosition();
        auto top = position == "Top Left" || position == "Top Right";
        auto left = position == "Top Left" || position == "Bottom Left";
        auto timerVisible = speedrun::config::showTimer();
        auto listVisible = speedrun::config::showList();
        auto textScale = speedrun::config::fontScale();
        auto spacing = speedrun::config::lineSpacing();
        auto anchor = cocos2d::CCPoint(left ? 0.0f : 1.0f, top ? 1.0f : 0.0f);
        auto x = left ? speedrun::config::kUiMargin : winSize.width - speedrun::config::kUiMargin;
        auto y = top ? winSize.height - speedrun::config::kUiMargin : speedrun::config::kUiMargin;

        view.m_splitRoot->setAnchorPoint(anchor);
        view.m_splitRoot->setPosition({x, y});

        view.m_timerLabel->setAnchorPoint(anchor);
        view.m_timerLabel->setVisible(timerVisible);
        view.m_timerLabel->setScale(textScale + 0.02f);
        view.m_timerLabel->setPosition({0.0f, 0.0f});

        auto timerOffset = timerVisible ? (spacing + 4.0f + kTimerListGap) : 0.0f;
        auto direction = top ? -1.0f : 1.0f;

        for (size_t i = 0; i < view.m_splitLabels.size(); ++i) {
            auto label = view.m_splitLabels[i];
            label->setAnchorPoint(anchor);
            label->setVisible(listVisible);
            label->setScale(textScale);
            label->setPosition({0.0f, direction * (timerOffset + static_cast<float>(i) * spacing)});
        }
    }

    void refreshTimerLabelImpl(speedrun::core::SplitState const& state, speedrun::ui::SplitViewState& view) {
        if (!view.m_timerLabel) {
            return;
        }

        view.m_timerLabel->setVisible(speedrun::config::showTimer());
        view.m_timerLabel->setString(
            fmt::format(
                "Timer {}",
                speedrun::utils::formatMainTimerTime(
                    state.m_runTime,
                    speedrun::config::timerFormat(),
                    speedrun::config::timerDecimals()
                )
            ).c_str()
        );
    }

    void updateExtraSplitLabelVisibility(
        speedrun::core::SplitState const& state,
        speedrun::ui::SplitViewState& view
    ) {
        auto visibleEntries = totalDisplayEntries(state);
        for (size_t i = 0; i < view.m_splitLabels.size(); ++i) {
            if (i >= visibleEntries) {
                view.m_splitLabels[i]->setVisible(false);
            }
        }
    }

    void ensureSplitSlot(
        PlayLayer* layer,
        speedrun::core::SplitState const& state,
        speedrun::ui::SplitViewState& view,
        size_t index
    ) {
        while (view.m_splitLabels.size() <= index) {
            if (!view.m_splitRoot) {
                createSplitRoot(layer, view);
            }

            auto label = cocos2d::CCLabelBMFont::create("", "chatFont.fnt");
            label->setAnchorPoint({0.0f, 1.0f});
            label->setScale(speedrun::config::kDefaultFontScale);
            label->setPosition({
                0.0f,
                -18.0f - static_cast<float>(view.m_splitLabels.size()) * speedrun::config::kDefaultLineSpacing
            });
            view.m_splitRoot->addChild(label);
            view.m_splitLabels.push_back(label);
        }

        updateUILayout(view);
        speedrun::ui::refreshSplitLabel(state, view, index, false);
    }

    void ensureUIReady(PlayLayer* layer, speedrun::core::SplitState const& state, speedrun::ui::SplitViewState& view) {
        if (!view.m_splitRoot || !view.m_timerLabel) {
            if (!view.m_splitRoot) {
                createSplitRoot(layer, view);
            }
        }

        for (size_t i = 0; i < totalDisplayEntries(state); ++i) {
            ensureSplitSlot(layer, state, view, i);
        }

        updateUILayout(view);
        updateExtraSplitLabelVisibility(state, view);
        refreshTimerLabelImpl(state, view);
    }
}

namespace speedrun::ui {
    void syncUIState(PlayLayer* layer, speedrun::core::SplitState const& state, SplitViewState& view) {
        if (!speedrun::config::modEnabled()) {
            destroySplitUI(view);
            return;
        }

        ensureUIReady(layer, state, view);
        updateUILayout(view);
    }

    void destroySplitUI(SplitViewState& view) {
        if (view.m_splitRoot) {
            view.m_splitRoot->removeFromParent();
        }

        view.m_splitRoot = nullptr;
        view.m_timerLabel = nullptr;
        view.m_splitLabels.clear();
    }

    void refreshTimerLabel(speedrun::core::SplitState const& state, SplitViewState& view) {
        refreshTimerLabelImpl(state, view);
    }

    void refreshAttemptState(speedrun::core::SplitState const& state, SplitViewState& view) {
        refreshTimerLabelImpl(state, view);

        for (size_t i = 0; i < view.m_splitLabels.size(); ++i) {
            refreshSplitLabel(state, view, i, false);
        }

        updateExtraSplitLabelVisibility(state, view);
    }

    void refreshSplitLabel(speedrun::core::SplitState const& state, SplitViewState& view, size_t index, bool justReached) {
        if (index >= view.m_splitLabels.size()) {
            return;
        }

        if (index >= totalDisplayEntries(state)) {
            view.m_splitLabels[index]->setVisible(false);
            return;
        }

        auto label = view.m_splitLabels[index];
        auto finalIndex = isFinalIndex(state, index);

        if (finalIndex) {
            auto reached = state.m_finalReached;
            auto reachedTime = state.m_finalReachedSplit;
            auto targetTime = state.m_hasFinalTarget ? state.m_finalTargetSplit : 0.0;

            if (!reached) {
                if (targetTime > 0.0) {
                    label->setString(
                        fmt::format(
                            "[] Final | {} / {}",
                            kEmptySplitTime,
                            speedrun::utils::formatTime(targetTime)
                        ).c_str()
                    );
                }
                else {
                    label->setString(fmt::format("[] Final | {} / {}", kEmptySplitTime, kEmptySplitTime).c_str());
                }
                label->setColor({200, 200, 200});
                label->setVisible(speedrun::config::showList());
                return;
            }

            auto text = fmt::format(
                "[X] Final | {} / {}",
                speedrun::utils::formatTime(reachedTime),
                targetTime > 0.0 ? speedrun::utils::formatTime(targetTime) : std::string(kEmptySplitTime)
            );

            label->setString(text.c_str());
            if (state.m_finalSplitResult < 0) {
                label->setColor({80, 255, 120});
            }
            else if (state.m_finalSplitResult > 0) {
                label->setColor({255, 90, 90});
            }
            else {
                label->setColor({255, 255, 255});
            }
            label->setVisible(speedrun::config::showList());

            if (justReached) {
                label->stopAllActions();
                label->setScale(speedrun::config::fontScale() + 0.12f);
                label->runAction(
                    cocos2d::CCSequence::create(
                        cocos2d::CCScaleTo::create(0.12f, speedrun::config::fontScale() + 0.22f),
                        cocos2d::CCScaleTo::create(0.12f, speedrun::config::fontScale()),
                        nullptr
                    )
                );
            }
            return;
        }

        auto reached = index < state.m_reachedCheckpoints.size() && state.m_reachedCheckpoints[index];
        auto reachedTime = index < state.m_reachedSplits.size() ? state.m_reachedSplits[index] : -1.0;
        auto targetTime = index < state.m_targetSplits.size() ? state.m_targetSplits[index] : 0.0;

        if (!reached) {
            if (targetTime > 0.0) {
                label->setString(
                    fmt::format(
                        "[] {} | {} / {}",
                        index + 1,
                        kEmptySplitTime,
                        speedrun::utils::formatTime(targetTime)
                    ).c_str()
                );
            }
            else {
                label->setString(fmt::format("[] {} | {} / {}", index + 1, kEmptySplitTime, kEmptySplitTime).c_str());
            }
            label->setColor({200, 200, 200});
            label->setVisible(speedrun::config::showList());
            return;
        }

        auto text = fmt::format(
            "[X] {} | {} / {}",
            index + 1,
            speedrun::utils::formatTime(reachedTime),
            targetTime > 0.0 ? speedrun::utils::formatTime(targetTime) : std::string(kEmptySplitTime)
        );

        label->setString(text.c_str());
        if (state.m_splitResults[index] < 0) {
            label->setColor({80, 255, 120});
        }
        else if (state.m_splitResults[index] > 0) {
            label->setColor({255, 90, 90});
        }
        else {
            label->setColor({255, 255, 255});
        }
        label->setVisible(speedrun::config::showList());

        if (justReached) {
            label->stopAllActions();
            label->setScale(speedrun::config::fontScale() + 0.12f);
            label->runAction(
                cocos2d::CCSequence::create(
                    cocos2d::CCScaleTo::create(0.12f, speedrun::config::fontScale() + 0.22f),
                    cocos2d::CCScaleTo::create(0.12f, speedrun::config::fontScale()),
                    nullptr
                )
            );
        }
    }
}
