#pragma once

#include <Geode/Geode.hpp>

#include <string>
#include <vector>

namespace speedrun::utils {
    inline constexpr double kTimeEqualityEpsilon = 0.005;

    std::string formatTime(double seconds);
    std::string formatMainTimerTime(double seconds, std::string const& format, int decimals);
    std::vector<double> parseTargets(std::string const& serialized);
    std::string serializeTargets(std::vector<double> const& targets);
    std::vector<double> loadStoredTargets(GJGameLevel* level);
    double loadStoredFinalTarget(GJGameLevel* level);
    std::string levelKey(GJGameLevel* level);
    std::string finalLevelKey(GJGameLevel* level);
}
