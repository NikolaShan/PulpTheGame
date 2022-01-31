package com.planemo.libs.gamecenter;


import android.app.Activity;
import android.content.Intent;

import android.content.SharedPreferences;
import android.os.Bundle;

import android.support.annotation.NonNull;
import android.util.Log;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.ResultCallback;
import com.google.android.gms.games.Games;
import com.google.android.gms.games.GamesActivityResultCodes;
import com.google.android.gms.games.leaderboard.LeaderboardVariant;
import com.google.android.gms.games.leaderboard.Leaderboards;
import com.planemo.libs.MultiplexerActivity;
import org.cocos2dx.lib.Cocos2dxHelper;

/**
 * Created by edward on 20.06.15.
 */
@SuppressWarnings("unused")
public class GameCenterManager {
    public static final String TAG = GameCenterManager.class.getName();

    public static final int CLIENT_GAMES = GameHelper.CLIENT_GAMES;
    public static final int CLIENT_APPSTATE = GameHelper.CLIENT_APPSTATE;
    public static final int CLIENT_PLUS = GameHelper.CLIENT_PLUS;
    public static final int CLIENT_ALL = GameHelper.CLIENT_ALL;
    private static final int ACHIEVEMENTS_REQUEST_ID = 5501;
    private static final int LEADERBOARD_REQUEST_ID = 5502;
    private static final String IS_SIGNED = "isSigned";
    private static final String GAME_CENTER_PREFS = "GameCenter";
    protected int mRequestedClients = CLIENT_GAMES;

    private GameHelper mHelper;
    private Activity mActivity;
    private GameCenterNativeListener mNativeListener;

    private static GameCenterManager mInstance;

    private Runnable mOnSignInRunnable;

    private GameCenterManager() {
        mActivity = MultiplexerActivity.getInstance();
        ActivityLifecycleListener lifecycleTracker = new ActivityLifecycleListener();
        MultiplexerActivity.getInstance().addLifecycleListener(lifecycleTracker);
        mNativeListener = new GameCenterNativeListener();
    }

    public static void init() {
        mInstance = new GameCenterManager();
    }

    public static GameCenterManager getInstance() {
        return mInstance;
    }

    private GameHelper getGameHelper(Activity activity) {
        if (mHelper == null) {
            mHelper = new GameHelper(activity, mRequestedClients);
            mHelper.enableDebugLog(false);
        }
        return mHelper;
    }

    public GoogleApiClient getApiClient() {
        return mHelper.getApiClient();
    }

    public void beginUserInitiatedSignIn() {
        if (mHelper != null) {
            mHelper.beginUserInitiatedSignIn();
        }
    }

    public boolean isAlreadySign() {
        try {
            return isSignedIn();
        } catch (Exception e) {
            e.printStackTrace();
            SharedPreferences pref = mActivity.getSharedPreferences(GAME_CENTER_PREFS, Activity.MODE_PRIVATE);
            return pref.getBoolean(IS_SIGNED, false);
        }
    }

    public boolean isSignedIn() {
        return mHelper != null && mHelper.isSignedIn();
    }

    public void signOut() {
        if (mHelper != null) {
            mHelper.signOut();

            cacheSignedIn(false);
        }
    }

    public void reconnectClient() {
        mHelper.reconnectClient();
    }

    public boolean hasSignInError() {
        return mHelper.hasSignInError();
    }

    public GameHelper.SignInFailureReason getSignInError() {
        return mHelper.getSignInError();
    }

    private void cacheSignedIn(boolean signedIn) {
        SharedPreferences pref = mActivity.getSharedPreferences(GAME_CENTER_PREFS, Activity.MODE_PRIVATE);
        SharedPreferences.Editor edit = pref.edit();
        edit.putBoolean(IS_SIGNED, signedIn);
        edit.apply();
    }

    /**
     * Leaderboards
     */

    public void submitScore(int score, String leaderboardName) {
        if (getApiClient().isConnected()) {
            Games.Leaderboards.submitScore(getApiClient(), leaderboardName, score);
        }
    }

    public void showLeaderBoard(final String leaderboardName) {
        if (leaderboardName == null)
            return;
        if (getApiClient().isConnected() && mHelper != null && mHelper.isSignedIn()) {
            mActivity.startActivityForResult((Games.Leaderboards.getLeaderboardIntent
                    (getApiClient(), leaderboardName)), LEADERBOARD_REQUEST_ID);
        } else if (mHelper != null && !mHelper.isConnecting()) {
            mOnSignInRunnable = new Runnable() {
                @Override
                public void run() {
                    showLeaderBoard(leaderboardName);
                }
            };
            mHelper.beginUserInitiatedSignIn();
        }
    }

    public void loadLocalPlayerScoreForLeaderBoard(final String leaderboard) {
        if (leaderboard == null || !getApiClient().isConnected()) {
            return;
        }
        Games.Leaderboards.loadCurrentPlayerLeaderboardScore(getApiClient(),
                leaderboard,
                LeaderboardVariant.TIME_SPAN_ALL_TIME,
                LeaderboardVariant.COLLECTION_PUBLIC
        ).setResultCallback(new ResultCallback<Leaderboards.LoadPlayerScoreResult>() {
            @Override
            public void onResult(@NonNull Leaderboards.LoadPlayerScoreResult loadPlayerScoreResult) {
                if (loadPlayerScoreResult.getStatus().isSuccess() && loadPlayerScoreResult.getScore() != null) {
                    mNativeListener.onLocalPlayerScoreLoaded(leaderboard, loadPlayerScoreResult.getScore().getRawScore());
                }
            }
        });

    }

    /**
     * Achievements
     */

    public void reportAchievement(final String achievementID, final int stepsDone, final int stepsRequired) {
        Log.v(TAG, "test achieve " + achievementID + " " + stepsDone + "/" + stepsRequired);
        if (getApiClient().isConnected()) {
            if (stepsRequired <= 0) {
                Games.Achievements.unlock(getApiClient(), achievementID);
            } else {
                Games.Achievements.setSteps(getApiClient(), achievementID, stepsDone);
            }
        }
    }

    public void showAchievementsBoard() {
        Log.v(TAG, "testshowAchievementsBoard ");
        if (getApiClient().isConnected() && mHelper != null && mHelper.isSignedIn()) {
            mActivity.startActivityForResult(
                    Games.Achievements.getAchievementsIntent(getApiClient()),
                    ACHIEVEMENTS_REQUEST_ID
            );
        } else if (mHelper != null && !mHelper.isConnecting()) {
            mOnSignInRunnable = new Runnable() {
                @Override
                public void run() {
                    showAchievementsBoard();
                }
            };
            mHelper.beginUserInitiatedSignIn();
        }
    }


    private class ActivityLifecycleListener extends MultiplexerActivity.AbstractLifecycleListener implements GameHelper.GameHelperListener {
        public ActivityLifecycleListener() {
        }

        @Override
        public void onCreate(Activity activity, Bundle savedInstanceState) {
            mActivity = activity;
            Log.v(TAG, "onCreate");
            if (mHelper == null) {
                mHelper = getGameHelper(activity);
                mHelper.setConnectOnStart(true);
                mHelper.setup(this);
            }
        }

        @Override
        public void onResume(Activity activity) {
            super.onResume(activity);
            cacheSignedIn(isSignedIn());
        }

        @Override
        public void onStart(Activity activity) {
            if (mHelper != null)
                mHelper.onStart(activity);
        }

        @Override
        public void onStop(Activity activity) {
            if (mHelper != null)
                mHelper.onStop();
        }

        @Override
        public boolean onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {
            if (mHelper != null && mHelper.onActivityResult(requestCode, resultCode, data)) {
                return true;
            }
            if (resultCode == GamesActivityResultCodes.RESULT_RECONNECT_REQUIRED) {
                if (mHelper != null) {
                    mHelper.disconnect();
                }
                cacheSignedIn(false);
                return true;
            }
            return false;
        }

        @Override
        public void onSignInFailed() {
            cacheSignedIn(false);

            mOnSignInRunnable = null;
            mNativeListener.onSignInSucceeded(false);
            Log.v(TAG, "onSignInFailed ");
        }

        @Override
        public void onSignInSucceeded() {
            cacheSignedIn(true);

            mNativeListener.onSignInSucceeded(true);

            if (mOnSignInRunnable != null) {
                mOnSignInRunnable.run();
                mOnSignInRunnable = null;
            }

            Log.v(TAG, "onSignInSucceeded");
        }
    }


    public static class Listener {
        public native void onSignInSucceeded(boolean isSuccess);
        public native void onLocalPlayerScoreLoaded(String leaderboard, long score);
    }

    public class GameCenterNativeListener {
        private Listener nativeListener = new Listener();

        public void onSignInSucceeded(final boolean isSuccess) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onSignInSucceeded(isSuccess);
                }
            });
        }

        public void onLocalPlayerScoreLoaded(final String leaderboard, final long score) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onLocalPlayerScoreLoaded(leaderboard, score);
                }
            });
        }
    }

}