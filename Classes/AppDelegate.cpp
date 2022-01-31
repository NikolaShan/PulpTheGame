#include "AppDelegate.h"

#include "Utils/PLAudioEngine.h"
#include "cocosbuilder/CocosBuilder.h"

#include "UI/CCBButton.h"
#include "UI/UIProgressHUD.h"
#include "UI/WebSprite.h"
#include "Utils/Localization.h"
#include "Utils/TimeUtils.h"
#include "Utils/RateScheduler.h"
#include "Utils/StringUtils.h"
#include "Resources/Resources.h"
#include "MainMenuScene.h"
#include "GameCenter.h"
#include "ContentProviders.h"
#include "IAPurchasesManager.h"
#include "AppSettings.h"
#include "NotificationsManager.h"
#include "Pseudo3DNode.h"
#include "Facebook.h"
#include "Vkontakte.h"
#include "Analytics.h"
#include "FirebaseAnalytics.h"
#include "TuneAnalytics.h"
#include "AdMobAds.h"
#include "Game/GameManager.h"
#include "AchievementsTracker.h"
#include "AppTurbo.h"
#include "AppTurboDialog.h"
#include "TexturedBackground.h"
#include "CCBToggle.h"
#include "Resources/ConfigKeys.h"
#include "AdsAnalytics.h"

USING_NS_CC;
using namespace cocosbuilder;
using namespace Advertise;

static void startCCLibs() {
    Localization::setAvailableLanguages({"en", "de", "es", "zh", "ru", "fr", "it", "ja", "pt", "nl", "ko"});
    FirebaseAnalytics::getInstance()->start();
    
    auto appSettings = AppSettings::getInstance();
    
    // Analytics
#if COCOS2D_DEBUG != 1
    Analytics::getInstance()->addProvider(FirebaseAnalytics::getInstance()->makeProvider());
#endif
    
    TuneAnalytics::getInstance()->start(appSettings->getString(R::Config::Analytics::Tune::ADVERTISER_ID),
                                        appSettings->getString(R::Config::Analytics::Tune::CONVERSION_KEY),
                                        false);
    
    // Game Center
    GameKit::GameCenter::getInstance()->start();
    
    // IA Store
    IA::PurchasesManager::getInstance()->setContentProvider(std::make_shared<IAPContentProvider>());
    IA::PurchasesManager::getInstance()->registerProducts(AppUtils::getBundleIdentifier(), R::IA_PURCHASES_PLIST);
    
    // Notifications setup
    NotificationsManager::getInstance()->startWithNotificationsHandler(std::make_shared<NotificationHandler>(), false);
    NotificationsManager::getInstance()->cancelLocalNotifications({1, 2});
    
    // Social
    Social::Facebook::getInstance()->start();
    Social::Vkontakte::getInstance()->startWithAppId(appSettings->getVkAppId());
    
    // Ads
    AdMobManager::start(appSettings->getInstance()->getString(R::Config::Admob::APP_ID));
    
    auto adsManager = AdsManager::getInstance();
    AdsPresentReporter *adsReporter = new AdsPresentReporter(adsManager->getAppearListener());
    adsManager->setAppearListener(adsReporter);

    if (!AppUtils::isSlowDevice()) {
        auto banner = Advertise::AdMobBanner::create(appSettings->getAdMobBannerId());
        adsManager->setBannerProvider(banner);
    }
    
    std::shared_ptr<Advertise::Interstitial> interstitial = AdMobInterstitial::create(appSettings->getAdMobInterstitialId());
    auto interstitialScheduler = std::make_shared<Advertise::InterstitialScheduler>();
    interstitialScheduler->setIntersititalProvider(interstitial);
    interstitialScheduler->setupWithAppSettings(appSettings);
    adsManager->setInterstitialScheduler(interstitialScheduler);
    
    AdMobRewardedVideo::getInstance()->setAdUnitId(appSettings->getAdMobRewardedVideoId());
    AdMobRewardedVideo::getInstance()->load();
    
    // Rate Scheduler
    auto rateScheduler = RateScheduler::getInstance();
    rateScheduler->setPromptForNewVersionIfUserRated(true);
    rateScheduler->setDaysUntilPrompt(0.75);
    rateScheduler->setUsesUntilPrompt(2);
    rateScheduler->setRemindPeriod(1.5);
    rateScheduler->setEventsUntilPrompt(14);
    
#ifdef REVIEW_MODE
    adsManager->setAdsEnabled(false);
#endif
}

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() {
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs() {
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages() {
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    std::srand(static_cast<unsigned>(time(nullptr)));
    
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("dulp");
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // Set the design resolution
    float designWidth = 320;
    if (AppUtils::isTablet()) {
       designWidth = 350;
    }
    glview->setDesignResolutionSize(designWidth, 480, ResolutionPolicy::FIXED_WIDTH);
    setupScaleFactor(designWidth);
    
    register_all_packages();    
    
    ValueMap lookupDict;
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    lookupDict[R::UI::MainMenu::BOTTOM_BUTTONS] = Value(R::UI::MainMenu::BOTTOM_BUTTONS_ANDROID);
    lookupDict[R::UiNight::MainMenu::BOTTOM_BUTTONS] = Value(R::UiNight::MainMenu::BOTTOM_BUTTONS_ANDROID);
#endif
    FileUtils::getInstance()->setFilenameLookupDictionary(lookupDict);
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    startCCLibs();
#endif
    IA::PurchasesManager::getInstance()->requestProducts();
    
    // create a scene. it's an autorelease object
    registerCustomNodeLoaders();
    preloadAudio();
    preloadImages();
    AchievementsTracker::getInstance()->start();
    
    auto scene = MainMenuScene::createScene(MenuStyle::MAIN_MENU);
   
    // run
    director->runWithScene(scene);
    
    // Present banner
    AdsManager::getInstance()->presentBanner(0.5, 0, 0);
    PLAudioEngine::getInstance()->playBackgroundMusic(R::Audio::MUSIC, true);
    AdsSessionCounter::onAppBecameActive();
    
    syncProgress();
    
#if COCOS2D_DEBUG == 1 && CC_TARGET_PLATFORM == CC_PLATFORM_IOS
//    auto gm = GameManager::getInstance();
//    for (int i = 1; i <= gm->getLevelsCount(); ++i) {
//        CCLOG("Validating level %d", i);
//        gm->getLevel(i);
//    }
#endif
    
    return true;
}

void AppDelegate::setupScaleFactor(const float designWidth) {
    auto director = Director::getInstance();
    auto fileUtils = FileUtils::getInstance();
    auto glView = director->getOpenGLView();
    
    const float scaleFactor = glView->getFrameSize().width / designWidth;
    std::vector<std::string> resolutionOrder;
    float contentScaleFactor = 1.0;
    resolutionOrder.push_back("resources-iphone");
    if (scaleFactor > 1.7) {
        resolutionOrder.push_back("resources-iphonehd");
        contentScaleFactor = 2.0;
    }
    
    if (scaleFactor > 2.7) {
        resolutionOrder.push_back("resources-ipad");
        contentScaleFactor = 3.0;
    }
    
    if (scaleFactor > 3.7) {
        resolutionOrder.push_back("resources-ipadhd");
        contentScaleFactor = 4.0;
    }
    
    std::reverse(resolutionOrder.begin(),resolutionOrder.end());
    
    director->setContentScaleFactor(contentScaleFactor);
    fileUtils->setSearchResolutionsOrder(resolutionOrder);
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    AdsSessionCounter::onAppResignActive();
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("ResignActive");
    Director::getInstance()->stopAnimation();

    PLAudioEngine::getInstance()->pauseBackgroundMusic();
    
    const std::string notificationMessage = LocalizedString(R::Strings::LocalNotification::MESSAGE);
    const std::string notificationAction = LocalizedString(R::Strings::LocalNotification::ACTION);
    
    //first notification in 4-8 days
    const float days = 4 + rand() % 4;
    NotificationsManager::getInstance()->scheduleLocalNotification(1,
        notificationMessage, notificationAction, TimeUtils::SECONDS_IN_DAY * days, {{"Key", Value(1)}});
    
    //second notification in 20 days
    NotificationsManager::getInstance()->scheduleLocalNotification(2,
        notificationMessage, notificationAction, TimeUtils::SECONDS_IN_DAY * 20, {{"Key", Value(2)}});
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    AdsSessionCounter::onAppBecameActive();
    AdsManager::getInstance()->resetInterstitialCountShown();
    
    Director::getInstance()->startAnimation();

    PLAudioEngine::getInstance()->resumeBackgroundMusic();
    
    // Cancel all notifications
    NotificationsManager::getInstance()->cancelLocalNotifications({1, 2});
    
    FirebaseAnalytics::getInstance()->sendEvent(FirebaseAnalytics::EventAppOpen, {});
}

void AppDelegate::registerCustomNodeLoaders() {
    ProgressHUD::setDefaultCcb(R::UI::Support::PROGRESS_HUD);
    CCBButton::setDefaultSound(R::Audio::BUTTON);
    
    auto library = NodeLoaderLibrary::getInstance();
    library->registerNodeLoader("CCBButton", CCBButtonLoader::loader());
    library->registerNodeLoader("CCBToggle", CCBToggleLoader::loader());
    library->registerNodeLoader("Pseudo3DNode", Pseudo3DNodeLoader::loader());
    library->registerNodeLoader("WebSprite", WebSpriteLoader::loader());
    library->registerNodeLoader("Background", TexturedBackgroundLoader::loader());
}

void AppDelegate::preloadAudio() {
    PLAudioEngine *engine = PLAudioEngine::getInstance();
    engine->preload(R::Audio::BUTTON);
    engine->preload(R::Audio::LOSE);
    engine->preload(R::Audio::WIN);
    engine->preload(R::Audio::TAP);
}

void AppDelegate::preloadImages() {
    TextureCache *cache = Director::getInstance()->getTextureCache();
    Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::AI88);
    cache->addImage(R::Images::Support::CIRCLE_IMG);
    cache->addImage(R::Images::Menu::ROUND_BUTTON_BACKGROUND_IMG);
    cache->addImage(R::Images::Menu::FACEBOOK_IMG);
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    cache->addImage(R::Images::Menu::ACHIEVEMENTS_ANDROID_IMG);
    cache->addImage(R::Images::Menu::GOOGLE_GAMEPAD_IMG);
#endif
    cache->addImage(R::Images::Menu::LEADERBOARD_IMG);
    cache->addImage(R::Images::Menu::NO_ADS_IMG);
    cache->addImage(R::Images::Menu::PLAY_BUTTON_IMG);
    cache->addImage(R::Images::Menu::PLAY_BUTTON_SHADOW_IMG);
    cache->addImage(R::Images::Menu::RATE_IMG);
    cache->addImage(R::Images::Menu::SHARE_IMG);
    cache->addImage(R::Images::Menu::SOUND_OFF_IMG);
    cache->addImage(R::Images::Menu::SOUND_ON_IMG);
    
    Texture2D::setDefaultAlphaPixelFormat(Texture2D::PixelFormat::RGBA8888);
}

void AppDelegate::syncProgress() {
    auto gamecenter = GameKit::GameCenter::getInstance();
    gamecenter->addAuthCallback([gamecenter](bool authenticated) {
        if (authenticated) {
            const std::string leaderboard = AppSettings::getInstance()->getLeaderboardName();
            gamecenter->getLocalPlayerScoreForLeaderboard(leaderboard, [](int score) {
                auto gm = GameManager::getInstance();
                const int completedCount = gm->getCompletedLevelsCount();
                if (score > completedCount) {
                    gm->setLevelCompleted(score);
                    gm->setCurrentLevel(score + 1);
                }
            });
        }
    }, this);
}

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif
    //this method called on Activity::onCreate method from android main thread, which is called before applicationDidFinishLaunching
    void Java_com_planemo_libs_MultiplexerActivity_onCreate(JNIEnv *env, jobject thiz) {
        startCCLibs();
    }
    
#ifdef __cplusplus
}
#endif


#endif
