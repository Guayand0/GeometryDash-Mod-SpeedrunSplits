#pragma once

#include <Geode/Geode.hpp>

#include <string>

namespace speedrun::config {
    inline constexpr char const* kSplitRootId = "speedrun-splits-root";
    inline constexpr float kDefaultFontScale = 0.5f;
    inline constexpr float kDefaultLineSpacing = 14.0f;
    inline constexpr float kUiMargin = 12.0f;

    bool modEnabled();
    bool showTimer();
    std::string timerFormat();
    int timerDecimals();
    bool showList();
    float fontScale();
    float lineSpacing();
    std::string uiPosition();
}
