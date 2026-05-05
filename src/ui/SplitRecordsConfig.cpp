#include "SplitRecordsConfig.hpp"

#include <Geode/Geode.hpp>

#include <algorithm>

using namespace geode::prelude;

namespace speedrun::ui::records {
    float textScale() {
        return std::clamp(
            static_cast<float>(Mod::get()->getSavedValue<double>(kTextScaleKey, kDefaultTextScale)),
            kMinTextScale,
            kMaxTextScale
        );
    }

    bool showFinal() {
        return Mod::get()->getSavedValue<bool>(kShowFinalKey, true);
    }

    int columns() {
        return std::clamp(
            static_cast<int>(Mod::get()->getSavedValue<int64_t>(kColumnsKey, 2)),
            1,
            2
        );
    }

    float columnWidth(float scrollWidth, int columns) {
        return columns <= 1 ? (scrollWidth - 8.0f) : ((scrollWidth - 20.0f) / 2.0f);
    }

    void setTextScale(float value) {
        Mod::get()->setSavedValue(
            kTextScaleKey,
            static_cast<double>(std::clamp(value, kMinTextScale, kMaxTextScale))
        );
    }

    void setShowFinal(bool value) {
        Mod::get()->setSavedValue(kShowFinalKey, value);
    }

    void setColumns(int value) {
        Mod::get()->setSavedValue<int64_t>(kColumnsKey, std::clamp(value, 1, 2));
    }
}
