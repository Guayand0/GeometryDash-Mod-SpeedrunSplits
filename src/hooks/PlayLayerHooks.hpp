#pragma once

#include <Geode/binding/CheckpointGameObject.hpp>
#include <Geode/binding/PlayLayer.hpp>

namespace speedrun::hooks {
    void registerCheckpointForPlayLayer(PlayLayer* playLayer, CheckpointGameObject* checkpoint);
}
