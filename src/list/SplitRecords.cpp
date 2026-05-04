#include "SplitRecords.hpp"

#include "../utils/Utils.hpp"

using namespace geode::prelude;

namespace speedrun::list {
    std::vector<StoredSplitRecord> loadStoredSplitRecords(GJGameLevel* level) {
        std::vector<StoredSplitRecord> records;
        auto targets = speedrun::utils::parseTargets(
            Mod::get()->getSavedValue<std::string>(speedrun::utils::levelKey(level), "")
        );

        for (size_t i = 0; i < targets.size(); ++i) {
            StoredSplitRecord record;
            record.m_index = i;
            record.m_isFinal = false;
            record.m_hasPb = targets[i] > 0.0;
            record.m_pb = targets[i];
            record.m_name = fmt::format("CP {}", i + 1);
            records.push_back(std::move(record));
        }

        auto finalPb = Mod::get()->getSavedValue<double>(
            speedrun::utils::finalLevelKey(level),
            0.0
        );
        if (finalPb > 0.0) {
            StoredSplitRecord finalRecord;
            finalRecord.m_index = targets.size();
            finalRecord.m_isFinal = true;
            finalRecord.m_hasPb = true;
            finalRecord.m_pb = finalPb;
            finalRecord.m_name = "Final";
            records.push_back(std::move(finalRecord));
        }

        return records;
    }

    void clearAllStoredSplitRecords(GJGameLevel* level) {
        Mod::get()->setSavedValue(speedrun::utils::levelKey(level), std::string(""));
        Mod::get()->setSavedValue(speedrun::utils::finalLevelKey(level), 0.0);
    }

    void clearSelectedStoredSplitRecords(
        GJGameLevel* level,
        std::vector<size_t> const& checkpointIndices,
        bool clearFinal
    ) {
        auto targets = speedrun::utils::parseTargets(
            Mod::get()->getSavedValue<std::string>(speedrun::utils::levelKey(level), "")
        );

        for (auto checkpointIndex : checkpointIndices) {
            if (checkpointIndex < targets.size()) {
                targets[checkpointIndex] = 0.0;
            }
        }

        Mod::get()->setSavedValue(
            speedrun::utils::levelKey(level),
            speedrun::utils::serializeTargets(targets)
        );

        if (clearFinal) {
            Mod::get()->setSavedValue(speedrun::utils::finalLevelKey(level), 0.0);
        }
    }
}
