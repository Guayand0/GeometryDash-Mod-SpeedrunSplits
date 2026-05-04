#include "Utils.hpp"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace speedrun::utils {
    namespace {
        std::string formatWithMilliseconds(double seconds, int decimals, bool hideEmptyUnits) {
            if (seconds < 0.0) {
                seconds = 0.0;
            }

            decimals = std::clamp(decimals, 0, 3);

            auto totalMilliseconds = static_cast<int>(std::round(seconds * 1000.0));
            auto hours = totalMilliseconds / 3600000;
            auto minutes = (totalMilliseconds / 60000) % 60;
            auto secs = (totalMilliseconds / 1000) % 60;
            auto millis = totalMilliseconds % 1000;
            auto fractional = millis;

            if (decimals == 2) {
                fractional = millis / 10;
            }
            else if (decimals == 0) {
                fractional = 0;
            }

            if (!hideEmptyUnits || hours > 0) {
                if (decimals == 3) {
                    return fmt::format("{:02d}:{:02d}:{:02d}.{:03d}", hours, minutes, secs, fractional);
                }
                if (decimals == 2) {
                    return fmt::format("{:02d}:{:02d}:{:02d}.{:02d}", hours, minutes, secs, fractional);
                }
                return fmt::format("{:02d}:{:02d}:{:02d}", hours, minutes, secs);
            }

            if (totalMilliseconds >= 60000) {
                if (decimals == 3) {
                    return fmt::format("{:02d}:{:02d}.{:03d}", minutes, secs, fractional);
                }
                if (decimals == 2) {
                    return fmt::format("{:02d}:{:02d}.{:02d}", minutes, secs, fractional);
                }
                return fmt::format("{:02d}:{:02d}", minutes, secs);
            }

            if (decimals == 3) {
                return fmt::format("{:02d}.{:03d}", secs, fractional);
            }
            if (decimals == 2) {
                return fmt::format("{:02d}.{:02d}", secs, fractional);
            }
            return fmt::format("{:02d}", secs);
        }
    }

    std::string formatTime(double seconds) {
        return formatWithMilliseconds(seconds, 3, true);
    }

    std::string formatMainTimerTime(double seconds, std::string const& format, int decimals) {
        if (format == "Seconds / Milliseconds") {
            if (seconds < 0.0) {
                seconds = 0.0;
            }

            decimals = std::clamp(decimals, 0, 3);

            auto totalMilliseconds = static_cast<int>(std::round(seconds * 1000.0));
            auto totalSeconds = totalMilliseconds / 1000;
            auto millis = totalMilliseconds % 1000;
            auto fractional = decimals == 3 ? millis : millis / 10;

            if (decimals == 3) {
                return fmt::format("{}.{:03d}", totalSeconds, fractional);
            }

            if (decimals == 2) {
                return fmt::format("{}.{:02d}", totalSeconds, fractional);
            }

            return fmt::format("{}", totalSeconds);
        }

        return formatWithMilliseconds(seconds, decimals, true);
    }

    std::vector<double> parseTargets(std::string const& serialized) {
        std::vector<double> targets;
        std::stringstream stream(serialized);
        std::string item;

        while (std::getline(stream, item, ',')) {
            if (item.empty()) {
                continue;
            }

            try {
                targets.push_back(std::stod(item));
            }
            catch (...) {}
        }

        return targets;
    }

    std::string serializeTargets(std::vector<double> const& targets) {
        std::ostringstream stream;

        for (size_t i = 0; i < targets.size(); ++i) {
            if (i != 0) {
                stream << ',';
            }
            stream << targets[i];
        }

        return stream.str();
    }

    std::string levelKey(GJGameLevel* level) {
        if (!level) {
            return "unknown-level";
        }

        return fmt::format(
            "{}:{}",
            level->m_levelID.value(),
            std::string(level->m_levelName)
        );
    }

    std::string finalLevelKey(GJGameLevel* level) {
        return fmt::format("{}:final", levelKey(level));
    }
}
