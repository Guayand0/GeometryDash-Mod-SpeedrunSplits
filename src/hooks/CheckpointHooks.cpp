#include "PlayLayerHooks.hpp"

#include <Geode/Geode.hpp>
#include <Geode/modify/CheckpointGameObject.hpp>

using namespace geode::prelude;

class $modify(SpeedrunSplitsCheckpointGameObject, CheckpointGameObject) {
    void triggerObject(GJBaseGameLayer* layer, int uniqueID, gd::vector<int> const* remapKeys) {
        CheckpointGameObject::triggerObject(layer, uniqueID, remapKeys);

        auto playLayer = typeinfo_cast<PlayLayer*>(layer);
        speedrun::hooks::registerCheckpointForPlayLayer(playLayer, this);
    }
};
