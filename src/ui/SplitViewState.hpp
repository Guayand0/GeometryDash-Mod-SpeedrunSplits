#pragma once

#include <Geode/Geode.hpp>

#include <vector>

namespace speedrun::ui {
    struct SplitViewState {
        cocos2d::CCNode* m_splitRoot = nullptr;
        cocos2d::CCLabelBMFont* m_timerLabel = nullptr;
        std::vector<cocos2d::CCLabelBMFont*> m_splitLabels;
    };
}
