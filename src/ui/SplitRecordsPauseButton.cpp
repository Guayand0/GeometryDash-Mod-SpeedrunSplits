#include "SplitRecordsPopup.hpp"

#include "../config/ModConfig.hpp"

#include <Geode/Geode.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

using namespace geode::prelude;

namespace {
    constexpr char const* kPauseButtonMenuId = "speedrun-splits-pause-menu";
    constexpr float kPauseButtonMarginLeft = 42.0f;
    constexpr float kPauseButtonMarginBottom = 38.0f;
}

class $modify(SpeedrunSplitsPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto playLayer = PlayLayer::get();
        if (!playLayer || !playLayer->m_isPlatformer || !speedrun::config::modEnabled()) {
            return;
        }

        if (this->getChildByID(kPauseButtonMenuId)) {
            return;
        }

        cocos2d::CCNode* icon = CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");
        if (!icon) {
            icon = CCSprite::createWithSpriteFrameName("GJ_stopWatchIcon_001.png");
        }
        if (!icon) {
            icon = CCLabelBMFont::create("T", "goldFont.fnt");
        }

        auto buttonSprite = geode::CircleButtonSprite::create(
            icon,
            geode::CircleBaseColor::Green,
            geode::CircleBaseSize::Small
        );

        auto button = CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(SpeedrunSplitsPauseLayer::onOpenSplitRecords)
        );
        button->setID("speedrun-splits-records-button");

        auto menu = cocos2d::CCMenu::create();
        menu->setID(kPauseButtonMenuId);
        menu->setPosition({kPauseButtonMarginLeft, kPauseButtonMarginBottom});
        this->addChild(menu);

        button->setPosition({0.0f, 0.0f});
        menu->addChild(button);
    }

    void onOpenSplitRecords(cocos2d::CCObject*) {
        auto playLayer = PlayLayer::get();
        if (!playLayer) {
            return;
        }

        speedrun::ui::showSplitRecordsPopup(playLayer->m_level);
    }
};
