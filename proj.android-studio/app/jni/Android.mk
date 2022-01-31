LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../../External/Chipmunk2D)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
					../../../Classes/AppDelegate.cpp \
					../../../Classes/AppSettings.cpp \
					../../../Classes/AdsAnalytics.cpp \
					../../../Classes/ChipmunkDebugNode.cpp \
					../../../Classes/ContentProviders.cpp \
					../../../Classes/GameHelpers.cpp \
					../../../Classes/GameScene.cpp \
					../../../Classes/InfiniteGameScene.cpp \
					../../../Classes/Segment.cpp \
					../../../Classes/BulletNode.cpp \
					../../../Classes/LevelSelectionScene.cpp \
					../../../Classes/MainMenuScene.cpp \
					../../../Classes/Pseudo3DNode.cpp \
					../../../Classes/RateDialog.cpp \
					../../../Classes/TexturedBackground.cpp \
					../../../Classes/RemoveAdsDialog.cpp \
					../../../Classes/SegmentMaker.cpp \
					../../../Classes/SkipLevelDialog.cpp \
					../../../Classes/SecondChanceScheduler.cpp \
					../../../Classes/SecondChanceDialog.cpp \
					../../../Classes/PauseDialog.cpp \
					../../../Classes/GameDialog.cpp \
					../../../Classes/SettingsDialog.cpp \
					../../../Classes/CCBToggle.cpp \
					../../../Classes/Game/GameManager.cpp \
					../../../Classes/Game/JsonLevelParser.cpp \
					../../../Classes/AchievementsTracker.cpp \
					../../../Classes/Resources/Resources.cpp \
					../../../Classes/Resources/Strings.cpp \
					../../../Classes/Resources/ConfigKeys.cpp \
					../../../Classes/ShareBanner.cpp \
					../../../Classes/CCBNodeContainer.cpp \
					../../../Classes/ExitConfirmationDialog.cpp \
					../../../Classes/SchedulerHelper.cpp \
					../../../External/CCLibs/Android/CrashlyticsTracker.cpp \
					../../../External/CCLibs/Android/AppUtils.cpp \
					../../../External/CCLibs/Android/JniSupport.cpp \
					../../../External/CCLibs/Android/Ads/AdMobAds.cpp \
					../../../External/CCLibs/Android/Analytics/FirebaseAnalytics.cpp \
					../../../External/CCLibs/Android/Analytics/TuneAnalytics.cpp \
					../../../External/CCLibs/Android/GameCenter/GameCenter.cpp \
					../../../External/CCLibs/Android/Notifications/NotificationsManager.cpp \
					../../../External/CCLibs/Android/Share/ShareManager.cpp \
					../../../External/CCLibs/Android/Social/Facebook.cpp \
					../../../External/CCLibs/Android/Social/Vkontakte.cpp \
					../../../External/CCLibs/Android/Social/Twitter.cpp \
					../../../External/CCLibs/Android/Store/IAStore.cpp \
					../../../External/CCLibs/Shared/Advertise.cpp \
					../../../External/CCLibs/Shared/Facebook.cpp \
					../../../External/CCLibs/Shared/IADefaultGuiDelegate.cpp \
					../../../External/CCLibs/Shared/IAProduct.cpp \
					../../../External/CCLibs/Shared/IAPurchasesManager.cpp \
					../../../External/CCLibs/Shared/SocialBasicTypes.cpp \
					../../../External/CCLibs/Shared/Vkontakte.cpp \
					../../../External/CCLibs/Shared/FirebaseAnalytics.cpp \
					../../../External/CCLibs/Android/RemoteConfig.cpp \
					../../../External/CCLibs/Include/UI/CCBAnimationRunner.cpp \
					../../../External/CCLibs/Include/UI/CCBButton.cpp \
					../../../External/CCLibs/Include/UI/CollectionViewCCBItem.cpp \
					../../../External/CCLibs/Include/UI/IndexPath.cpp \
					../../../External/CCLibs/Include/UI/UICollectionView.cpp \
					../../../External/CCLibs/Include/UI/UICollectionViewItem.cpp \
					../../../External/CCLibs/Include/UI/CCBDialog.cpp \
					../../../External/CCLibs/Include/UI/UIProgressHUD.cpp \
					../../../External/CCLibs/Include/UI/WebSprite.cpp \
					../../../External/CCLibs/Include/Utils/BaseAppSettings.cpp \
					../../../External/CCLibs/Include/Utils/Collections.cpp \
					../../../External/CCLibs/Include/Utils/EventDispatcherHelper.cpp \
					../../../External/CCLibs/Include/Utils/ExpirableValueStore.cpp \
					../../../External/CCLibs/Include/Utils/FilePath.cpp \
					../../../External/CCLibs/Include/Utils/FileUrlCache.cpp \
					../../../External/CCLibs/Include/Utils/format.cc \
					../../../External/CCLibs/Include/Utils/GuiUtils.cpp \
					../../../External/CCLibs/Include/Utils/JsonValueConverter.cpp \
					../../../External/CCLibs/Include/Utils/Localization.cpp \
					../../../External/CCLibs/Include/Utils/PLAudioEngine.cpp \
					../../../External/CCLibs/Include/Utils/StringUtils.cpp \
					../../../External/CCLibs/Include/Utils/RateScheduler.cpp \
					../../../External/Chipmunk2D/src/chipmunk.c \
					../../../External/Chipmunk2D/src/cpArbiter.c \
					../../../External/Chipmunk2D/src/cpArray.c \
					../../../External/Chipmunk2D/src/cpBBTree.c \
					../../../External/Chipmunk2D/src/cpBody.c \
					../../../External/Chipmunk2D/src/cpCollision.c \
					../../../External/Chipmunk2D/src/cpConstraint.c \
					../../../External/Chipmunk2D/src/cpDampedRotarySpring.c \
					../../../External/Chipmunk2D/src/cpDampedSpring.c \
					../../../External/Chipmunk2D/src/cpGearJoint.c \
					../../../External/Chipmunk2D/src/cpGrooveJoint.c \
					../../../External/Chipmunk2D/src/cpHashSet.c \
					../../../External/Chipmunk2D/src/cpHastySpace.c \
					../../../External/Chipmunk2D/src/cpMarch.c \
					../../../External/Chipmunk2D/src/cpPinJoint.c \
					../../../External/Chipmunk2D/src/cpPivotJoint.c \
					../../../External/Chipmunk2D/src/cpPolyline.c \
					../../../External/Chipmunk2D/src/cpPolyShape.c \
					../../../External/Chipmunk2D/src/cpRatchetJoint.c \
					../../../External/Chipmunk2D/src/cpRobust.c \
					../../../External/Chipmunk2D/src/cpRotaryLimitJoint.c \
					../../../External/Chipmunk2D/src/cpShape.c \
					../../../External/Chipmunk2D/src/cpSimpleMotor.c \
					../../../External/Chipmunk2D/src/cpSlideJoint.c \
					../../../External/Chipmunk2D/src/cpSpace.c \
					../../../External/Chipmunk2D/src/cpSpaceComponent.c \
					../../../External/Chipmunk2D/src/cpSpaceDebug.c \
					../../../External/Chipmunk2D/src/cpSpaceHash.c \
					../../../External/Chipmunk2D/src/cpSpaceQuery.c \
					../../../External/Chipmunk2D/src/cpSpaceStep.c \
					../../../External/Chipmunk2D/src/cpSpatialIndex.c \
					../../../External/Chipmunk2D/src/cpSweep1D.c \

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../Classes \
                    $(LOCAL_PATH)/../../../External \
                    $(LOCAL_PATH)/../../../External/Chipmunk2D/include/ \
                    $(LOCAL_PATH)/../../../External/CCLibs/Include

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dxandroid_static


# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,platform/android)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
