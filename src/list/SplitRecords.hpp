#pragma once

#include <Geode/binding/GJGameLevel.hpp>

#include <string>
#include <vector>

namespace speedrun::list {
    struct StoredSplitRecord {
        size_t m_index = 0;
        bool m_isFinal = false;
        bool m_hasPb = false;
        double m_pb = 0.0;
        std::string m_name;
    };

    std::vector<StoredSplitRecord> loadStoredSplitRecords(GJGameLevel* level);
    void clearAllStoredSplitRecords(GJGameLevel* level);
    void clearSelectedStoredSplitRecords(
        GJGameLevel* level,
        std::vector<size_t> const& checkpointIndices,
        bool clearFinal
    );
}
