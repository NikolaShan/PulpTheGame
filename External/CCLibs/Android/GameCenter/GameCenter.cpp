#include <jni.h>
#include <map>
#include "platform/android/jni/JniHelper.h"
#include "base/ccMacros.h"

#include "GameCenter.h"

USING_NS_CC;

namespace GameKit {
	static const std::string CB_CLASS_NAME = "com/planemo/libs/gamecenter/GameCenterManager";
   
    class AndroidGameCenter: public GameCenter {
    public:
        std::function<void(bool success)> authCallback = nullptr;
        using LocalPlayerScoreCallback = std::function<void(int score)>;
        std::map<std::string, LocalPlayerScoreCallback> localPlayerScoreCallbacks;

    private:
        jobject getJavaInstance() const {
            JniMethodInfo methodInfo;
            const std::string methodSignature = "()L" + CB_CLASS_NAME + ";";
            if (! JniHelper::getStaticMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "getInstance", methodSignature.c_str())) {
                CCASSERT(false, "ERROR: Can't find method for GameCenter::getInstance");
            }
            jobject jGameCenter = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            return jGameCenter;
        }

    public:
        virtual void start() override {
            JNIEnv *env = JniHelper::getEnv();
            JniMethodInfo startMethodInfo;
            if (! JniHelper::getStaticMethodInfo(startMethodInfo, CB_CLASS_NAME.c_str(), "init", "()V")) {
                CCASSERT(false, "ERROR: Can't find signature for GameCenter::init");
            }   
            env->CallStaticVoidMethod(startMethodInfo.classID, startMethodInfo.methodID);
            env->DeleteLocalRef(startMethodInfo.classID);
        }

        virtual void setShowCompletionBanner(bool showCompletionBanner) override {
            
        }
        
        virtual void authenticateLocalPlayer(std::function<void(bool success)> callback) override {
            authCallback = callback;
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "beginUserInitiatedSignIn", "()V")) {
               CCASSERT(false, "Can't find signature for GameCenter::beginUserInitiatedSignIn");
               return;
            }   
                        
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }

        virtual void signOut() override {
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "signOut", "()V")) {
               CCASSERT(false, "Can't find signature for GameCenter::signOut");
               return;
            }   
                        
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        
        virtual bool isLocalPlayerAuthenticated() const override {
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "isAlreadySign", "()Z")) {
                CCASSERT(false, "Can't find signature for GameCenter::isLocalPlayerAuthenticated");
                return false;
            }

            bool ret = methodInfo.env->CallBooleanMethod(getJavaInstance(), methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            return ret;
        }
        
        virtual void showAchievements() override {
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "showAchievementsBoard", "()V")) {
                CCASSERT(false, "Can't find signature for GameCenter::showAchievementsBoard");
                return;
            }        
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);            
        }
        
        virtual void impl_submitScore(int64_t s, const std::string &leaderboardName) override {
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "submitScore", "(ILjava/lang/String;)V")) {
                CCASSERT(false, "Can't find signature for GameCenter::submitScore");
                return;
            }

            jint score = (jint)s;
            jstring jleaderboardName = methodInfo.env->NewStringUTF(leaderboardName.c_str());
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, score, jleaderboardName);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jleaderboardName);
        }
        
        virtual void impl_reportAchievements(const std::vector<std::string> &identifiers, const std::vector<int> &stepsDone, const std::vector<int> &stepsRequired) override {
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "reportAchievement", "(Ljava/lang/String;II)V")) {
                CCASSERT(false, "Can't find signature for GameCenter::reportAchievement");
                return;
            }

            const size_t count = std::min({identifiers.size(), stepsDone.size(), stepsRequired.size()});
            for (size_t i = 0; i < count; ++i) {
                const std::string identifier = identifiers[i];
                const int steps = stepsDone[i];
                const int required = stepsRequired[i];

                jstring jidentifier = methodInfo.env->NewStringUTF(identifier.c_str());
                methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jidentifier, steps, required);
                methodInfo.env->DeleteLocalRef(jidentifier);
            }
            
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        
        virtual void impl_showLeaderBoard(const std::string &identifier) override {
            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "showLeaderBoard", "(Ljava/lang/String;)V")) {
                CCASSERT(false, "Can't find signature for GameCenter::showLeaderBoard");
                return;
            }
            jstring jidentifier = methodInfo.env->NewStringUTF(identifier.c_str());
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jidentifier);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jidentifier);            
        }

        virtual void getLocalPlayerScoreForLeaderboard(const std::string &leaderboardName, std::function<void(int highscore)> result) override {
            localPlayerScoreCallbacks[leaderboardName] = result;

            JniMethodInfo methodInfo;
            if (! JniHelper::getMethodInfo(methodInfo, CB_CLASS_NAME.c_str(), "loadLocalPlayerScoreForLeaderBoard", "(Ljava/lang/String;)V")) {
                CCASSERT(false, "Can't find signature for GameCenter::loadLocalPlayerScoreForLeaderBoard");
                return;
            }
            jstring jidentifier = methodInfo.env->NewStringUTF(leaderboardName.c_str());
            methodInfo.env->CallVoidMethod(getJavaInstance(), methodInfo.methodID, jidentifier);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jidentifier);     
        }

        void notifyAuthStateChanged(bool authenticated) {
            onAuthStateChanged(authenticated);
        }
    };
    
    GameCenter *GameCenter::getInstance() {
        static GameCenter *instance = new AndroidGameCenter;
        return instance;
    }
}

#ifdef __cplusplus
extern "C" {
#endif

	void Java_com_planemo_libs_gamecenter_GameCenterManager_00024Listener_onSignInSucceeded(JNIEnv *env, jobject thiz, jboolean isSuccess) {
	    GameKit::AndroidGameCenter *gk = dynamic_cast<GameKit::AndroidGameCenter *>(GameKit::GameCenter::getInstance());
        if (gk->authCallback) {
            gk->authCallback((bool)isSuccess);
            gk->authCallback = nullptr;
        }
        gk->notifyAuthStateChanged((bool)isSuccess);
    }

    void Java_com_planemo_libs_gamecenter_GameCenterManager_00024Listener_onLocalPlayerScoreLoaded(JNIEnv *env, jobject thiz, jstring jleaderboardName, jlong jscore) {
        GameKit::AndroidGameCenter *gk = dynamic_cast<GameKit::AndroidGameCenter *>(GameKit::GameCenter::getInstance());
        const std::string leaderboardName = JniHelper::jstring2string(jleaderboardName);
        const int score = static_cast<int>(jscore);
        CCLOG("On score loaded: %s = %d", leaderboardName.c_str(), score);
        auto it = gk->localPlayerScoreCallbacks.find(leaderboardName);
        if (it != gk->localPlayerScoreCallbacks.end()) {
            (*it).second(score);
            gk->localPlayerScoreCallbacks.erase(it);
        }
    }


#ifdef __cplusplus
}
#endif




