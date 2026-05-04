#include "ModConfig.hpp"

using namespace geode::prelude;

namespace speedrun::config {
    bool modEnabled() {
        return Mod::get()->getSettingValue<bool>("mod-enabled");
    }

    bool showTimer() {
        return Mod::get()->getSettingValue<bool>("show-timer");
    }

    std::string timerFormat() {
        return Mod::get()->getSettingValue<std::string>("timer-format");
    }

    int timerDecimals() {
        auto value = Mod::get()->getSettingValue<std::string>("timer-decimals");
        return value == "3" ? 3 : 2;
    }

    bool showList() {
        return Mod::get()->getSettingValue<bool>("show-list");
    }

    float fontScale() {
        return static_cast<float>(Mod::get()->getSettingValue<double>("font-scale"));
    }

    float lineSpacing() {
        return static_cast<float>(Mod::get()->getSettingValue<double>("line-spacing"));
    }

    std::string uiPosition() {
        return Mod::get()->getSettingValue<std::string>("ui-position");
    }
}
