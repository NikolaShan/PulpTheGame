//
//  SkipLevelDialog.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 26/07/15.
//
//

#include "SkipLevelDialog.h"

#include "2d/CCLabel.h"
#include "2d/CCScene.h"
#include "cocosbuilder/CocosBuilder.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "Utils/PLAudioEngine.h"
#include "Utils/StringUtils.h" 

#include "Utils/GuiUtils.h"
#include "Utils/AppUtils.h"
#include "Utils/ExpirableValueStore.h"
#include "Resources/Resources.h"
#include "Resources/Strings.h"
#include "UI/CCBButton.h"
#include "UI/CCBAnimationRunner.h"
#include "UI/UIProgressHUD.h"
#include "Vkontakte.h"
#include "Facebook.h"
#include "IAPurchasesManager.h"
#include "Twitter.h"
#include "AdMobAds.h"
#include "CrashlyticsTracker.h"

#include "Game/GameManager.h"
#include "AppSettings.h"
#include "Analytics.h"
#include "FirebaseAnalytics.h"
#include "ContentProviders.h"
#include "ShareBanner.h"
#include "AchievementsTracker.h"
#include "SchedulerHelper.h"

USING_NS_CC;
using namespace cocosbuilder;

const std::string SkipLevelDialog::SHARE_TO_FB_KEY = "share_fb";
const std::string SkipLevelDialog::SHARE_TO_VK_KEY = "shre_vk";
const std::string SkipLevelDialog::INVITE_FB_KEY = "invite_fb";
const std::string SkipLevelDialog::SHARE_TO_TWITTER_KEY = "share_tw";

void SkipLevelDialog::showWithCompletion(std::function<void (bool skipped)> completion, cocos2d::Node *scene) {
    if (GameManager::getInstance()->getAvailableLevelSkips() > 0) {
        showWithCcb(R::UI::Dialogs::SKIP_FOR_SKIP, completion, scene);
    } else {
        auto pm = IA::PurchasesManager::getInstance();
        if (pm->isProductsLoaded()) {
            showSkipWithOptions(completion, scene);
        } else {
            static int LOAD_CALLBACK_TARGET = 0;
            auto progressHud = ProgressHUD::create();
            progressHud->setBlackout();
            progressHud->showLoadingWithStatus("");
            progressHud->retain();
            
            pm->requestProducts([progressHud, pm, completion, scene](EventCustom *) {
                if (pm->isProductsLoaded()) {
                    progressHud->dismiss();
                    showSkipWithOptions(completion, scene);
                } else {
                    progressHud->showFailureWithStatus(LocalizedString(R::Strings::ERROR));
                }
                progressHud->release();
                
            }, &LOAD_CALLBACK_TARGET);
        }
    }
}

void SkipLevelDialog::showSkipForVideo(std::function<void (bool)> completion, cocos2d::Node *scene) {
    showWithCcb(R::UI::Dialogs::SKIP_FOR_VIDEO, completion, scene);
}

void SkipLevelDialog::showSkipWithOptions(std::function<void (bool)> completion, cocos2d::Node *scene) {
    const std::string ccb = R::UI::Dialogs::SKIP_WITH_OPTIONS;
    showWithCcb(ccb, completion, scene);
}

void SkipLevelDialog::showWithCcb(const std::string &ccbFile, std::function<void (bool skipped)> completion, cocos2d::Node *scene) {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("SkipLevelDialog", SkipLevelDialogLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    SkipLevelDialog *rootNode = dynamic_cast<SkipLevelDialog *>(
                                                                ccbReader->readNodeGraphFromFile(UI_FILE(ccbFile)));
    ccbReader->release();
    rootNode->completion = completion;
    
    scene->addChild(rootNode);
}

SkipLevelDialog::SkipLevelDialog() {
    GuiUtils::setupEscapePressHandler(this, [this]() {
        cancel();
    });
}

SkipLevelDialog::~SkipLevelDialog() {
    
}

bool SkipLevelDialog::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    SETUP_CUSTOM_CCBBUTTON("CancelButton", LocalizedString(R::Strings::SkipLevelDialog::CANCEL), dismiss);
    SETUP_LABEL("Title", LocalizedString(R::Strings::SkipLevelDialog::TITLE));
    SETUP_LABEL("Message", LocalizedString(R::Strings::SkipLevelDialog::MESSAGE_OPTIONS));
    SETUP_LABEL("MessageFreeSkips", LocalizedString(R::Strings::SkipLevelDialog::MESSAGE_FREE_SKIPS));
    SETUP_LABEL("MessageSkipAds", LocalizedString(R::Strings::SkipLevelDialog::MESSAGE_SKIP_ADS));
    
    if (memberVariableName == "ContentNode") {
        const auto sizeInDpi = AppUtils::getScreenSizeInDpi();
        if (sizeInDpi.width > 470.0f) {
            const float scale = 470.0f / sizeInDpi.width;
            node->setScale(scale);
        }
        return true;
    }
    
    SETUP_CUSTOM_CCBBUTTON("SkipForSkipsButton", LocalizedString(R::Strings::SkipLevelDialog::SKIP), skipForSkips);
    SETUP_CUSTOM_CCBBUTTON("BuySkipsButton", getTitleForBuySkips("level_skip"), buySkips1);
    SETUP_CUSTOM_CCBBUTTON("Buy3SkipsButton", getTitleForBuySkips("level_skip3"), buySkips3);
    SETUP_CUSTOM_CCBBUTTON("SkipForVideoButton", LocalizedString(R::Strings::SkipLevelDialog::WATCH_ADS), skipForRewardedVideo);
    
    if (memberVariableName == "SkipForInvite") {
        CCBButton *button = dynamic_cast<CCBButton *>(node);
        button->setIcon(R::Images::Dialogs::INVITE_FRIENDS_IMG);
        button->setCallback([this]() { skipForInviteFriend(); });
        button->setEnabled(canSkipForInviteFriends());
        return true;
    }
    
    if (memberVariableName == "SkipForShare") {
        CCBButton *button = dynamic_cast<CCBButton *>(node);
        if (Social::Vkontakte::getInstance()->isVkCountry()) {
            button->setIcon(R::Images::Dialogs::VKONTAKTE_IMG);
            button->setCallback([this]() { skipForShareInVkontakte(); });
            button->setEnabled(canShareTo(SHARE_TO_VK_KEY));
        } else if (canShareTo(SHARE_TO_TWITTER_KEY) && Social::TwitterManager::getInstance()->canTweet()) {
            button->setIcon(R::Images::Dialogs::TWITTER_IMG);
            button->setCallback([this]() { skipForShareInTwitter(); });
            button->setEnabled(true);
        } else {
            button->setIcon(R::Images::Dialogs::FACEBOOK_IMG);
            button->setCallback([this]() { skipForShareInFacebook(); });
            button->setEnabled(canShareTo(SHARE_TO_FB_KEY));
        }
        return true;
    }
    
    return false;
}

void SkipLevelDialog::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch *touch, Event *event) {
        return true;
    };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void SkipLevelDialog::dismiss() {
    if (isHiding) return;
    isHiding = true;
    
    CCBAnimationRunner::run(this, "Dismiss", [this]() {
        removeFromParent();
    });
}

void SkipLevelDialog::cancel() {
    completion(false);
    dismiss();
}

void SkipLevelDialog::performCompletionWithResult(bool result) {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    auto completion = this->completion;
    SchedulerHelper::getInstance().schedule([result, completion](){
        completion(result);
    }, 0.5f);
#else
    completion(result);
#endif
}

void SkipLevelDialog::performLevelSkip(const std::string &source) {
    auto gm = GameManager::getInstance();
    const int levelToSkip = gm->getCurrentLevelNumber();
    gm->setLevelCompleted(levelToSkip);
    gm->startNextLevel();
    
    Analytics::getInstance()->sendEvent("skip_level",
                                        {{"level", ::StringUtils::toString(levelToSkip)},
                                        {"using", source}});
}

void SkipLevelDialog::skipForRewardedVideo() {
    std::function<void(void)> preshow = [this]() {
        retain();
    };
    std::function<void(bool)> aftershow = [this](bool success) {
        if (success) {
            performLevelSkip("rewarded_video");
        }
        performCompletionWithResult(success);
        release();
    };
    
    auto rewardedVideo = Advertise::AdMobRewardedVideo::getInstance();
    if (rewardedVideo->getCurrentStatus() == Advertise::Status::LOADED) {
        preshow();
        rewardedVideo->present(aftershow);
    } else {
        auto progressHud = ProgressHUD::create();
        progressHud->setBlackout();
        progressHud->showLoadingWithStatus("");
        progressHud->retain();
        retain();
        rewardedVideo->load([this, preshow, aftershow, progressHud, rewardedVideo](bool result) {
            if (result) {
                progressHud->dismiss();
                
                preshow();
                rewardedVideo->present(aftershow);
                
            } else {
                progressHud->showFailureWithStatus(LocalizedString(R::Strings::ERROR));
                performCompletionWithResult(false);
            }

            progressHud->release();
            release();
        });
    }
    
    dismiss();
}

void SkipLevelDialog::skipForShareInVkontakte() {
    const std::string title = LocalizedString(R::Strings::Share::DIRECT_TITLE);
    const std::string message = FormattedLocalizedString(R::Strings::Share::DIRECT_MESSAGE, GameManager::getInstance()->getCurrentLevelNumber());
    retain();
    dismiss();
    Social::Vkontakte::getInstance()->share(message, title, AppSettings::getInstance()->getShareUrl(), [this](bool success) {
        if (success) {
            auto valueStore = ExpirableValueStore::getInstance();
            valueStore->setValue(SkipLevelDialog::SHARE_TO_VK_KEY, Value(true), 4 * TimeUtils::SECONDS_IN_DAY);
            performLevelSkip("share_in_vkontakte");
            performCompletionWithResult(true);
            AchievementsTracker::getInstance()->onSocialAction();
        } else {
            performCompletionWithResult(false);
        }
        release();
    });
}

void SkipLevelDialog::skipForShareInFacebook() {
    const std::string title = LocalizedString(R::Strings::Share::DIRECT_TITLE);
    const std::string message = FormattedLocalizedString(R::Strings::Share::DIRECT_MESSAGE, GameManager::getInstance()->getCurrentLevelNumber());
    retain();
    dismiss();
    Social::Facebook::getInstance()->shareLink(AppSettings::getInstance()->getShareUrl(), title, "", message, [this](bool success) {
        if (success) {
            auto valueStore = ExpirableValueStore::getInstance();
            valueStore->setValue(SkipLevelDialog::SHARE_TO_FB_KEY, Value(true), 4 * TimeUtils::SECONDS_IN_DAY);
            performLevelSkip("share_in_facebook");
            performCompletionWithResult(true);
            AchievementsTracker::getInstance()->onSocialAction();
        } else {
            performCompletionWithResult(false);
        }
        release();
    });
}

void SkipLevelDialog::skipForShareInTwitter() {
    CrashlyticsTracker::log("Start twitter share for skips");

    retain();
    dismiss();
    
    const int level = GameManager::getInstance()->getCurrentLevelNumber();
    ShareBanner *banner = ShareBanner::createFromCcb();
    banner->setupForLevelShare(level);
    banner->takeScreenshot([this](const std::string &filePath) {
        Social::TwitterManager::TweetData tweet;
        tweet.text = FormattedLocalizedString(R::Strings::Share::DIRECT_MESSAGE, GameManager::getInstance()->getCurrentLevelNumber());
        tweet.url = AppSettings::getInstance()->getShareUrl();
        tweet.imagePath = filePath;
        
        Social::TwitterManager::getInstance()->tweet(tweet, [this](bool success) {
            if (success) {
                auto valueStore = ExpirableValueStore::getInstance();
                valueStore->setValue(SkipLevelDialog::SHARE_TO_TWITTER_KEY, Value(true), 3 * TimeUtils::SECONDS_IN_DAY);
                performLevelSkip("share_in_twitter");
                performCompletionWithResult(true);
                AchievementsTracker::getInstance()->onSocialAction();
            } else {
                performCompletionWithResult(false);
            }
            release();
            CrashlyticsTracker::log("End twitter share for skips");
        });
    });
}

bool SkipLevelDialog::canShareTo(const std::string &key) const {
    auto valueStore = ExpirableValueStore::getInstance();
    return valueStore->getValueForKey(key).isNull();
}

bool SkipLevelDialog::canSkipForInviteFriends() const {
    if (!Social::Facebook::getInstance()->canInviteFriends()) {
        return false;
    }
    
    auto valueStore = ExpirableValueStore::getInstance();
    auto prevValue = valueStore->getValueForKey(SkipLevelDialog::INVITE_FB_KEY);
    const int countOfInvitesToday = prevValue.isNull() ? 0 : prevValue.asInt();
    const int maxSkipsPerDay = AppSettings::getInstance()->getInt("skips_per_day_for_invite");
    if (countOfInvitesToday >= maxSkipsPerDay) {
        return false;
    }
    
    return true;
}

void SkipLevelDialog::skipForInviteFriend() {
    dismiss();
    retain();
    
    Social::Facebook::getInstance()->inviteFriends(AppSettings::getInstance()->getFacebookInviteLink(), [this](bool success) {
        if (success) {
            auto valueStore = ExpirableValueStore::getInstance();
            auto prevValue = valueStore->getValueForKey(SkipLevelDialog::INVITE_FB_KEY);
            const int countOfInvitesToday = prevValue.isNull() ? 0 : prevValue.asInt();
            valueStore->setValue(SkipLevelDialog::INVITE_FB_KEY, Value(countOfInvitesToday + 1), TimeUtils::SECONDS_IN_DAY);
            performLevelSkip("invite_facebook");
            performCompletionWithResult(true);
            AchievementsTracker::getInstance()->onSocialAction();
        } else {
            performCompletionWithResult(false);
        }
        release();
    });
}

void SkipLevelDialog::skipForSkips() {
    const int availableSkips = GameManager::getInstance()->getAvailableLevelSkips();
    if (availableSkips > 0) {
        GameManager::getInstance()->setAvailableLevelSkips(availableSkips - 1);
        performLevelSkip("ForSkips");
        performCompletionWithResult(true);
    } else {
        performCompletionWithResult(false);
        dismiss();
    }
}

void SkipLevelDialog::buySkips1() {
    buySkips("level_skip");
}

void SkipLevelDialog::buySkips3() {
    buySkips("level_skip3");
}

void SkipLevelDialog::buySkips(const std::string &name) {
    retain();
    dismiss();
    
    IA::PurchasesManager::getInstance()->buyProduct(name, [this](bool success) {
        if (success) {
            const int availableSkips = GameManager::getInstance()->getAvailableLevelSkips();
            GameManager::getInstance()->setAvailableLevelSkips(availableSkips - 1);
            performLevelSkip("Purchase");
            performCompletionWithResult(true);
        } else {
            performCompletionWithResult(false);
        }
        release();
    });
}

std::string SkipLevelDialog::getTitleForBuySkips(const std::string &name) const {
    auto iap = IA::PurchasesManager::getInstance()->getProduct(name);
    if (iap) {
        const std::string price = iap->getLocalizedPrice();
        const int skipsCount = iap->getRewardsInfo().at(IAPContentProvider::SKIPS_REWARD).asInt();
        return FormattedLocalizedString(R::Strings::SkipLevelDialog::BUY_SKIPS, skipsCount, price);
    }
    return "";
}

bool SkipLevelDialog::canSkipForRewardedVideo() {
    return true;
}
