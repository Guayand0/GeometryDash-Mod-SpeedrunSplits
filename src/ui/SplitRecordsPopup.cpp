#include "SplitRecordsPopup.hpp"

#include "../config/ModConfig.hpp"
#include "../list/SplitRecords.hpp"
#include "../utils/Utils.hpp"

#include <Geode/Geode.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/Scrollbar.hpp>

#include <algorithm>
#include <limits>

using namespace geode::prelude;

namespace {
    constexpr char const* kPauseButtonMenuId = "speedrun-splits-pause-menu";
    constexpr float kPauseButtonMarginLeft = 42.0f;
    constexpr float kPauseButtonMarginBottom = 38.0f;
    constexpr float kPopupWidth = 320.0f;
    constexpr float kPopupHeight = 230.0f;
    constexpr float kScrollWidth = 270.0f;
    constexpr float kScrollHeight = 135.0f;
    constexpr float kRowHeight = 24.0f;
    constexpr float kColumnWidth = (kScrollWidth - 20.0f) / 2.0f;

    class SplitRecordsPopup final : public geode::Popup {
    protected:
        struct RowData {
            size_t m_index = 0;
            bool m_isFinal = false;
            CCMenuItemToggler* m_toggle = nullptr;
        };

        GJGameLevel* m_level = nullptr;
        geode::ScrollLayer* m_scrollLayer = nullptr;
        geode::Scrollbar* m_scrollbar = nullptr;
        cocos2d::CCLabelBMFont* m_emptyLabel = nullptr;
        cocos2d::CCLabelBMFont* m_checkpointCountLabel = nullptr;
        CCMenuItemToggler* m_selectAllToggle = nullptr;
        CCMenuItemSpriteExtra* m_deleteAllButton = nullptr;
        CCMenuItemSpriteExtra* m_deleteSelectedButton = nullptr;
        std::vector<RowData> m_rows;

        void queueActionButtonRefresh() {
            this->stopActionByTag(0x51504F50);
            auto action = cocos2d::CCSequence::create(
                cocos2d::CCDelayTime::create(0.0f),
                cocos2d::CCCallFunc::create(this, callfunc_selector(SplitRecordsPopup::updateActionButtons)),
                nullptr
            );
            action->setTag(0x51504F50);
            this->runAction(action);
        }

        void addRecordCell(
            speedrun::list::StoredSplitRecord const& record,
            size_t recordIndex,
            cocos2d::CCNode* parent,
            cocos2d::CCMenu* menu,
            cocos2d::CCPoint const& position
        ) {
            auto cellNode = cocos2d::CCNode::create();
            cellNode->setContentSize({kColumnWidth, kRowHeight});
            cellNode->setAnchorPoint({0.0f, 0.5f});
            cellNode->setPosition(position);
            parent->addChild(cellNode);

            auto toggler = CCMenuItemToggler::createWithStandardSprites(
                this,
                menu_selector(SplitRecordsPopup::onToggleRow),
                0.6f
            );
            toggler->setTag(static_cast<int>(recordIndex));
            toggler->setPosition({
                position.x + 10.0f,
                position.y + kRowHeight / 2.0f
            });
            toggler->toggle(false);
            menu->addChild(toggler);

            auto pbText = record.m_hasPb
                ? speedrun::utils::formatTime(record.m_pb)
                : std::string("........");
            auto rowText = fmt::format("{} | PB {}", record.m_name, pbText);

            auto label = cocos2d::CCLabelBMFont::create(rowText.c_str(), "chatFont.fnt");
            label->setAnchorPoint({0.0f, 0.5f});
            label->setScale(0.45f);
            label->setPosition({26.0f, kRowHeight / 2.0f + 1.5f});
            cellNode->addChild(label);

            RowData rowData;
            rowData.m_index = record.m_index;
            rowData.m_isFinal = record.m_isFinal;
            rowData.m_toggle = toggler;
            m_rows.push_back(rowData);
        }

        bool setup(GJGameLevel* level) {
            m_level = level;

            if (!this->init(kPopupWidth, kPopupHeight)) {
                return false;
            }

            this->setTitle("Split Records");

            auto infoLabel = cocos2d::CCLabelBMFont::create(
                "View and delete split PBs for this level.",
                "chatFont.fnt"
            );
            infoLabel->setScale(0.45f);
            infoLabel->setPosition({kPopupWidth / 2.0f, kPopupHeight - 34.0f});
            m_mainLayer->addChild(infoLabel);

            m_scrollLayer = geode::ScrollLayer::create({kScrollWidth, kScrollHeight});
            m_scrollLayer->setPosition({
                (kPopupWidth - kScrollWidth) / 2.0f,
                kPopupHeight - 52.0f - kScrollHeight
            });
            m_mainLayer->addChild(m_scrollLayer);

            m_scrollbar = geode::Scrollbar::create(m_scrollLayer);
            m_scrollbar->setPosition({
                m_scrollLayer->getPositionX() + kScrollWidth + 8.0f,
                m_scrollLayer->getPositionY() + kScrollHeight / 2.0f
            });
            m_mainLayer->addChild(m_scrollbar);

            m_emptyLabel = cocos2d::CCLabelBMFont::create(
                "No split records for this level yet.",
                "chatFont.fnt"
            );
            m_emptyLabel->setScale(0.5f);
            m_emptyLabel->setPosition({kPopupWidth / 2.0f, 106.0f});
            m_mainLayer->addChild(m_emptyLabel);

            m_checkpointCountLabel = cocos2d::CCLabelBMFont::create("CPs: 0", "goldFont.fnt");
            m_checkpointCountLabel->setAnchorPoint({1.0f, 0.5f});
            m_checkpointCountLabel->setScale(0.45f);
            m_checkpointCountLabel->setPosition({kPopupWidth - 16.0f, kPopupHeight - 34.0f});
            m_mainLayer->addChild(m_checkpointCountLabel);

            auto buttonMenu = cocos2d::CCMenu::create();
            buttonMenu->setPosition({0.0f, 0.0f});
            m_mainLayer->addChild(buttonMenu);

            auto allLabel = cocos2d::CCLabelBMFont::create("All:", "goldFont.fnt");
            allLabel->setAnchorPoint({0.0f, 0.5f});
            allLabel->setScale(0.45f);
            allLabel->setPosition({16.0f, kPopupHeight - 34.0f});
            m_mainLayer->addChild(allLabel);

            m_selectAllToggle = CCMenuItemToggler::createWithStandardSprites(
                this,
                menu_selector(SplitRecordsPopup::onToggleAll),
                0.6f
            );
            m_selectAllToggle->setPosition({54.0f, kPopupHeight - 34.0f});
            m_selectAllToggle->toggle(false);
            buttonMenu->addChild(m_selectAllToggle);

            auto trashSpr = geode::CircleButtonSprite::createWithSpriteFrameName(
                "GJ_trashBtn_001.png",
                0.75f,
                geode::CircleBaseColor::Red,
                geode::CircleBaseSize::SmallAlt
            );
            m_deleteAllButton = CCMenuItemSpriteExtra::create(
                trashSpr,
                this,
                menu_selector(SplitRecordsPopup::onDeleteAll)
            );
            m_deleteAllButton->setPosition({kPopupWidth / 2.0f - 32.0f, 26.0f});
            buttonMenu->addChild(m_deleteAllButton);

            auto minusLabel = cocos2d::CCLabelBMFont::create("-", "goldFont.fnt");
            minusLabel->setScale(0.9f);
            auto minusSpr = geode::CircleButtonSprite::create(
                minusLabel,
                geode::CircleBaseColor::Red,
                geode::CircleBaseSize::SmallAlt
            );
            m_deleteSelectedButton = CCMenuItemSpriteExtra::create(
                minusSpr,
                this,
                menu_selector(SplitRecordsPopup::onDeleteSelected)
            );
            m_deleteSelectedButton->setPosition({kPopupWidth / 2.0f + 32.0f, 26.0f});
            buttonMenu->addChild(m_deleteSelectedButton);

            auto trashText = cocos2d::CCLabelBMFont::create("Delete All", "chatFont.fnt");
            trashText->setScale(0.45f);
            trashText->setPosition({kPopupWidth / 2.0f - 32.0f, 8.0f});
            m_mainLayer->addChild(trashText);

            auto minusText = cocos2d::CCLabelBMFont::create("Delete Selected", "chatFont.fnt");
            minusText->setScale(0.45f);
            minusText->setPosition({kPopupWidth / 2.0f + 32.0f, 8.0f});
            m_mainLayer->addChild(minusText);

            this->reloadRows();
            return true;
        }

    public:
        static SplitRecordsPopup* create(GJGameLevel* level) {
            auto ret = new SplitRecordsPopup();
            if (ret && ret->setup(level)) {
                ret->autorelease();
                return ret;
            }

            delete ret;
            return nullptr;
        }

        void reloadRows() {
            m_rows.clear();
            m_scrollLayer->m_contentLayer->removeAllChildrenWithCleanup(true);

            auto records = speedrun::list::loadStoredSplitRecords(m_level);
            auto hasRows = !records.empty();
            auto checkpointCount = records.size();

            m_scrollLayer->setVisible(hasRows);
            m_emptyLabel->setVisible(!hasRows);
            if (m_scrollbar) {
                m_scrollbar->setVisible(hasRows);
            }
            if (m_checkpointCountLabel) {
                m_checkpointCountLabel->setString(fmt::format("CPs: {}", checkpointCount).c_str());
            }

            if (!hasRows) {
                this->updateActionButtons();
                return;
            }

            auto visualRowCount = (records.size() + 1) / 2;
            auto contentHeight = std::max(
                kScrollHeight,
                6.0f + static_cast<float>(visualRowCount) * kRowHeight
            );
            m_scrollLayer->setContentLayerSize({kScrollWidth, contentHeight});

            auto startY = contentHeight - 14.0f;

            for (size_t rowIndex = 0; rowIndex < visualRowCount; ++rowIndex) {
                auto leftIndex = rowIndex * 2;
                auto rightIndex = leftIndex + 1;

                auto rowNode = cocos2d::CCNode::create();
                rowNode->setContentSize({kScrollWidth - 8.0f, kRowHeight});
                rowNode->setAnchorPoint({0.0f, 0.5f});
                rowNode->setPosition({6.0f, startY - static_cast<float>(rowIndex) * kRowHeight});
                m_scrollLayer->m_contentLayer->addChild(rowNode);

                auto rowMenu = cocos2d::CCMenu::create();
                rowMenu->setPosition({0.0f, 0.0f});
                rowNode->addChild(rowMenu);

                this->addRecordCell(
                    records[leftIndex],
                    leftIndex,
                    rowNode,
                    rowMenu,
                    cocos2d::CCPoint{0.0f, 0.0f}
                );

                if (rightIndex < records.size()) {
                    this->addRecordCell(
                        records[rightIndex],
                        rightIndex,
                        rowNode,
                        rowMenu,
                        cocos2d::CCPoint{kColumnWidth + 12.0f, 0.0f}
                    );
                }
            }

            m_scrollLayer->scrollToTop();
            this->updateActionButtons();
        }

        void updateActionButtons() {
            auto hasRows = !m_rows.empty();
            auto allSelected = hasRows;

            for (auto const& row : m_rows) {
                if (!row.m_toggle || !row.m_toggle->isToggled()) {
                    allSelected = false;
                    break;
                }
            }

            if (m_deleteAllButton) {
                m_deleteAllButton->setEnabled(hasRows);
                m_deleteAllButton->setOpacity(hasRows ? 255 : 90);
            }

            if (m_deleteSelectedButton) {
                m_deleteSelectedButton->setEnabled(hasRows);
                m_deleteSelectedButton->setOpacity(hasRows ? 255 : 90);
            }

            if (m_selectAllToggle) {
                m_selectAllToggle->setEnabled(hasRows);
                m_selectAllToggle->setOpacity(hasRows ? 255 : 90);
                m_selectAllToggle->toggle(hasRows && allSelected);
            }
        }

        void onToggleRow(cocos2d::CCObject* sender) {
            auto toggler = static_cast<CCMenuItemToggler*>(sender);
            if (!toggler) {
                return;
            }

            this->queueActionButtonRefresh();
        }

        void onToggleAll(cocos2d::CCObject* sender) {
            if (!sender) {
                return;
            }

            auto allSelected = !m_rows.empty();
            for (auto const& row : m_rows) {
                if (!row.m_toggle || !row.m_toggle->isToggled()) {
                    allSelected = false;
                    break;
                }
            }

            auto shouldSelectAll = !allSelected;
            for (auto const& row : m_rows) {
                if (!row.m_toggle) {
                    continue;
                }

                row.m_toggle->toggle(shouldSelectAll);
            }

            this->queueActionButtonRefresh();
        }

        void onDeleteAll(cocos2d::CCObject*) {
            if (m_rows.empty()) {
                return;
            }

            geode::createQuickPopup(
                "Confirm Delete",
                "Delete all split records for this level? This cannot be undone.",
                "Cancel",
                "Delete",
                [this](FLAlertLayer*, bool btn2) {
                    if (!btn2) {
                        return;
                    }

                    speedrun::list::clearAllStoredSplitRecords(m_level);
                    this->reloadRows();
                }
            );
        }

        void onDeleteSelected(cocos2d::CCObject*) {
            std::vector<size_t> checkpointIndices;
            auto clearFinal = false;

            for (auto const& row : m_rows) {
                if (!row.m_toggle || !row.m_toggle->isToggled()) {
                    continue;
                }

                if (row.m_isFinal) {
                    clearFinal = true;
                }
                else {
                    checkpointIndices.push_back(row.m_index);
                }
            }

            if (checkpointIndices.empty() && !clearFinal) {
                geode::createQuickPopup(
                    "No Selection",
                    "No checkpoint selected.",
                    "OK",
                    nullptr,
                    nullptr
                );
                return;
            }

            geode::createQuickPopup(
                "Confirm Delete",
                "Delete the selected split PBs? This cannot be undone.",
                "Cancel",
                "Delete",
                [this, checkpointIndices, clearFinal](FLAlertLayer*, bool btn2) {
                    if (!btn2) {
                        return;
                    }

                    speedrun::list::clearSelectedStoredSplitRecords(
                        m_level,
                        checkpointIndices,
                        clearFinal
                    );
                    this->reloadRows();
                }
            );
        }
    };
}

namespace speedrun::ui {
    void showSplitRecordsPopup(GJGameLevel* level) {
        if (!level) {
            return;
        }

        if (auto popup = SplitRecordsPopup::create(level)) {
            popup->show();
        }
    }
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
