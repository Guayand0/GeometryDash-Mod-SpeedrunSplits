#pragma once

namespace speedrun::ui::records {
    inline constexpr char const* kTextScaleKey = "split-records-text-scale";
    inline constexpr char const* kShowFinalKey = "split-records-show-final";
    inline constexpr char const* kColumnsKey = "split-records-columns";
    inline constexpr float kDefaultTextScale = 0.55f;
    inline constexpr float kMinTextScale = 0.35f;
    inline constexpr float kMaxTextScale = 0.9f;

    float textScale();
    bool showFinal();
    int columns();
    float columnWidth(float scrollWidth, int columns);

    void setTextScale(float value);
    void setShowFinal(bool value);
    void setColumns(int value);
}
