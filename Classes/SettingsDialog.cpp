//
//  SettingsDialog.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 03/08/16.
//
//

#include "SettingsDialog.h"

#include "cocosbuilder/CocosBuilder.h"

#include "Utils/GuiUtils.h"
#include "UI/CCBAnimationRunner.h"
#include "CCBToggle.h"
#include "Resources/Resources.h"

#include "AppSettings.h"
#include "MainMenuScene.h"

USING_NS_CC;
using namespace cocosbuilder;

SettingsDialog *SettingsDialog::createFromCcb(const Size &size) {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("SettingsDialog", SettingsDialogLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    SettingsDialog *node = dynamic_cast<SettingsDialog *>(ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Dialogs::SETTINGS), nullptr, size));
    ccbReader->release();
    return node;
}

SettingsDialog::SettingsDialog() {
    
}

SettingsDialog::~SettingsDialog() {
    
}
    
bool SettingsDialog::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    SETUP_LABEL("Title", LocalizedString(R::Strings::SettingsDialog::TITLE))
    CCBToggle *btn = dynamic_cast<CCBToggle *>(node);
    if (btn) {
        if (memberVariableName == "ColorblindMode") {
            btn->setTitle(LocalizedString(R::Strings::SettingsDialog::COLORBLIND_OPTION));
        } else if (memberVariableName == "Music") {
            btn->setTitle(LocalizedString(R::Strings::SettingsDialog::MUSIC_OPTION));
        } else if (memberVariableName == "Sounds") {
            btn->setTitle(LocalizedString(R::Strings::SettingsDialog::SOUNDS_OPTION));
        } else if (memberVariableName == "NightMode") {
            btn->setTitle(LocalizedString(R::Strings::SettingsDialog::NIGHT_MODE_OPTION));
        }
        options.insert(memberVariableName, btn);
        return true;
    }
    
    SETUP_CUSTOM_CCBBUTTON("OkayButton", LocalizedString(R::Strings::SettingsDialog::OKAY_BUTTON), dismiss)
    
    return GameDialog::onAssignCCBMemberVariable(target, memberVariableName, node);
}

void SettingsDialog::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {
    GameDialog::onNodeLoaded(node, nodeLoader);
    scheduleOnce([this](float dt) {
        for (auto entry: options) {
            CCBToggle *btn = entry.second;
            if (entry.first == "ColorblindMode") {
                btn->setSelected(AppSettings::getInstance()->isColorblindMode(), false);
                btn->setCallback([this, btn]() {
                    this->toggleColorblindMode(btn);
                });
            } else if (entry.first == "Music") {
                btn->setSelected(AppSettings::getInstance()->isMusicEnabled(), false);
                btn->setCallback([this, btn]() {
                    this->toggleMusic(btn);
                });
            } else if (entry.first == "Sounds") {
                btn->setSelected(AppSettings::getInstance()->isSoundsEnabled(), false);
                btn->setCallback([this, btn]() {
                    this->toggleSounds(btn);
                });
            } else if (entry.first == "NightMode") {
                btn->setSelected(AppSettings::getInstance()->isNightModeEnabled(), false);
                btn->setCallback([this, btn]() {
                    this->toggleNightMode(btn);
                });
            }
        }
    }, 0.01f, "AnimatorFix");
}

void SettingsDialog::toggleNightMode(CCBToggle *btn) {
    const bool enabled = !AppSettings::getInstance()->isNightModeEnabled();
    AppSettings::getInstance()->setNightModeEnabled(enabled);
    Scene *scene = MainMenuScene::createWithCurrentStyle();
    Director::getInstance()->replaceScene(scene);
    //scene->scheduleOnce([scene](float) {
        SettingsDialog *dialog = SettingsDialog::createFromCcb(scene->getContentSize());
        dialog->willShowInside(scene);
        scene->addChild(dialog);
        CCBAnimationRunner::run(dialog, "None", nullptr);
    //}, 1.0f, "dialog");
}

void SettingsDialog::toggleColorblindMode(CCBToggle *btn) {
    const bool enabled = !AppSettings::getInstance()->isColorblindMode();
    AppSettings::getInstance()->setColorblindMode(enabled);
    btn->setSelected(enabled, true);
}

void SettingsDialog::toggleMusic(CCBToggle *btn) {
    const bool enabled = !AppSettings::getInstance()->isMusicEnabled();
    AppSettings::getInstance()->setMusicEnabled(enabled);
    btn->setSelected(enabled, true);
}

void SettingsDialog::toggleSounds(CCBToggle *btn) {
    const bool enabled = !AppSettings::getInstance()->isSoundsEnabled();
    AppSettings::getInstance()->setSoundsEnabled(enabled);
    btn->setSelected(enabled, true);
}

