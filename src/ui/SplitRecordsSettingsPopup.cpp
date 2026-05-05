#include "SplitRecordsSettingsPopup.hpp"

#include "SplitRecordsConfig.hpp"

#include <Geode/Geode.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/SliderNode.hpp>

using namespace geode::prelude;

namespace {
    constexpr float kSettingsPopupWidth = 250.0f;
    constexpr float kSettingsPopupHeight = 180.0f;

    class SplitRecordsSettingsPopup final : public geode::Popup {
    protected:
        std::function<void()> m_onChanged;
        geode::SliderNode* m_textSizeSlider = nullptr;
        cocos2d::CCLabelBMFont* m_textSizeValueLabel = nullptr;
        CCMenuItemToggler* m_showFinalToggle = nullptr;
        CCMenuItemSpriteExtra* m_oneColumnButton = nullptr;
        CCMenuItemSpriteExtra* m_twoColumnButton = nullptr;

        void notifyChanged() {
            if (m_onChanged) {
                m_onChanged();
            }
        }

        void updateTextSizeValue() {
            if (m_textSizeValueLabel && m_textSizeSlider) {
                m_textSizeValueLabel->setString(
                    fmt::format("{:.2f}", m_textSizeSlider->getValue()).c_str()
                );
            }
        }

        void updateColumnButtons() {
            auto columns = speedrun::ui::records::columns();
            auto updateButton = [columns](CCMenuItemSpriteExtra* button, int value) {
                if (!button) {
                    return;
                }

                auto enabled = columns != value;
                button->setEnabled(enabled);
                button->setOpacity(enabled ? 255 : 155);

                if (auto sprite = typeinfo_cast<ButtonSprite*>(button->getNormalImage())) {
                    sprite->setColor(columns == value ? cocos2d::ccYELLOW : cocos2d::ccWHITE);
                    sprite->setOpacity(enabled ? 255 : 200);
                    button->updateSprite();
                }
            };

            updateButton(m_oneColumnButton, 1);
            updateButton(m_twoColumnButton, 2);
        }

        bool setup(std::function<void()> onChanged) {
            m_onChanged = std::move(onChanged);

            if (!this->init(kSettingsPopupWidth, kSettingsPopupHeight)) {
                return false;
            }

            this->setTitle("Records Settings");

            auto buttonMenu = cocos2d::CCMenu::create();
            buttonMenu->setPosition({0.0f, 0.0f});
            m_mainLayer->addChild(buttonMenu);

            auto textSizeLabel = cocos2d::CCLabelBMFont::create("Text Size", "goldFont.fnt");
            textSizeLabel->setAnchorPoint({0.0f, 0.5f});
            textSizeLabel->setScale(0.5f);
            textSizeLabel->setPosition({22.0f, 126.0f});
            m_mainLayer->addChild(textSizeLabel);

            m_textSizeValueLabel = cocos2d::CCLabelBMFont::create("", "chatFont.fnt");
            m_textSizeValueLabel->setAnchorPoint({1.0f, 0.5f});
            m_textSizeValueLabel->setScale(0.55f);
            m_textSizeValueLabel->setPosition({226.0f, 126.0f});
            m_mainLayer->addChild(m_textSizeValueLabel);

            m_textSizeSlider = geode::SliderNode::create([this](geode::SliderNode*, float value) {
                speedrun::ui::records::setTextScale(value);
                this->updateTextSizeValue();
                this->notifyChanged();
            });
            m_textSizeSlider->setContentSize({160.0f, 16.0f});
            m_textSizeSlider->setMin(speedrun::ui::records::kMinTextScale);
            m_textSizeSlider->setMax(speedrun::ui::records::kMaxTextScale);
            m_textSizeSlider->setSnapStep(0.05f);
            m_textSizeSlider->setValue(speedrun::ui::records::textScale());
            m_textSizeSlider->setPosition({125.0f, 102.0f});
            m_mainLayer->addChild(m_textSizeSlider);
            this->updateTextSizeValue();

            auto showFinalLabel = cocos2d::CCLabelBMFont::create("Show Final", "goldFont.fnt");
            showFinalLabel->setAnchorPoint({0.0f, 0.5f});
            showFinalLabel->setScale(0.5f);
            showFinalLabel->setPosition({22.0f, 72.0f});
            m_mainLayer->addChild(showFinalLabel);

            m_showFinalToggle = CCMenuItemToggler::createWithStandardSprites(
                this,
                menu_selector(SplitRecordsSettingsPopup::onToggleShowFinal),
                0.7f
            );
            m_showFinalToggle->setPosition({194.0f, 72.0f});
            m_showFinalToggle->toggle(speedrun::ui::records::showFinal());
            buttonMenu->addChild(m_showFinalToggle);

            auto columnsLabel = cocos2d::CCLabelBMFont::create("Columns", "goldFont.fnt");
            columnsLabel->setAnchorPoint({0.0f, 0.5f});
            columnsLabel->setScale(0.5f);
            columnsLabel->setPosition({22.0f, 42.0f});
            m_mainLayer->addChild(columnsLabel);

            auto oneColumnSprite = ButtonSprite::create(
                "1",
                26,
                true,
                "goldFont.fnt",
                "GJ_button_01.png",
                18.0f,
                0.6f
            );
            m_oneColumnButton = CCMenuItemSpriteExtra::create(
                oneColumnSprite,
                this,
                menu_selector(SplitRecordsSettingsPopup::onOneColumn)
            );
            m_oneColumnButton->setPosition({158.0f, 42.0f});
            buttonMenu->addChild(m_oneColumnButton);

            auto twoColumnSprite = ButtonSprite::create(
                "2",
                26,
                true,
                "goldFont.fnt",
                "GJ_button_01.png",
                18.0f,
                0.6f
            );
            m_twoColumnButton = CCMenuItemSpriteExtra::create(
                twoColumnSprite,
                this,
                menu_selector(SplitRecordsSettingsPopup::onTwoColumns)
            );
            m_twoColumnButton->setPosition({206.0f, 42.0f});
            buttonMenu->addChild(m_twoColumnButton);
            this->updateColumnButtons();

            auto closeSprite = ButtonSprite::create("Close");
            closeSprite->setScale(0.6f);
            auto closeButton = CCMenuItemSpriteExtra::create(
                closeSprite,
                this,
                menu_selector(SplitRecordsSettingsPopup::onClose)
            );
            closeButton->setPosition({125.0f, 16.0f});
            buttonMenu->addChild(closeButton);

            return true;
        }

        void onToggleShowFinal(cocos2d::CCObject*) {
            speedrun::ui::records::setShowFinal(!speedrun::ui::records::showFinal());
            this->notifyChanged();
        }

        void onOneColumn(cocos2d::CCObject*) {
            speedrun::ui::records::setColumns(1);
            this->updateColumnButtons();
            this->notifyChanged();
        }

        void onTwoColumns(cocos2d::CCObject*) {
            speedrun::ui::records::setColumns(2);
            this->updateColumnButtons();
            this->notifyChanged();
        }

    public:
        static SplitRecordsSettingsPopup* create(std::function<void()> onChanged) {
            auto ret = new SplitRecordsSettingsPopup();
            if (ret && ret->setup(std::move(onChanged))) {
                ret->autorelease();
                return ret;
            }

            delete ret;
            return nullptr;
        }
    };
}

namespace speedrun::ui {
    void showSplitRecordsSettingsPopup(std::function<void()> onChanged) {
        if (auto popup = SplitRecordsSettingsPopup::create(std::move(onChanged))) {
            popup->show();
        }
    }
}
