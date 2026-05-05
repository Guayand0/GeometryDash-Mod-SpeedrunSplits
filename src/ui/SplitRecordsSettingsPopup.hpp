#pragma once

#include <functional>

namespace speedrun::ui {
    void showSplitRecordsSettingsPopup(std::function<void()> onChanged);
}
