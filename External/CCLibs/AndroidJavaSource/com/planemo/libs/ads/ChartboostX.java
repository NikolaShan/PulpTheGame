package com.planemo.libs.ads;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import com.chartboost.sdk.Chartboost;
import com.chartboost.sdk.ChartboostDelegate;
import com.chartboost.sdk.Model.CBError;
import com.planemo.libs.MultiplexerActivity;

import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.HashSet;
import java.util.Set;

@SuppressWarnings("unused")
public class ChartboostX {
    private static final String TAG = ChartboostX.class.getName();

    private Set<String> mCachedInterstitials = new HashSet<>();
    private Set<String> mCachedMoreApps = new HashSet<>();
    private Set<String> mCachedRewardedVideos = new HashSet<>();

    //
    // Forward chartboost delegate events to C++ part (run them on GL thread)
    //

    public static class Listener {
        public native void didCacheInterstitial(String location);

        public native void didFailToLoadInterstitial(String location, int errorCode);

        public native void didCacheMoreApps(String location);

        public native void didFailToLoadMoreApps(String location, int errorCode);

        public native void didCacheRewardedVideo(String location);

        public native void didFailToLoadRewardedVideo(String location, int errorCode);

        public native void didCompleteRewardedVideo(String location, int reward);

        public native void didCloseInterstitial();

        public native void didCloseRewardedVideo();
    }

    private class ChartboostNativeListener extends ChartboostDelegate {
        private final Listener mNativeListener = new Listener();

        ChartboostNativeListener() {
        }

        @Override
        public void didCacheInterstitial(final String location) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mCachedInterstitials.add(location);
                    mNativeListener.didCacheInterstitial(location);
                }
            });
        }

        @Override
        public void didFailToLoadInterstitial(final String location, final CBError.CBImpressionError error) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mNativeListener.didFailToLoadInterstitial(location, error.ordinal());
                }
            });
        }

        @Override
        public void didCacheMoreApps(final String location) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mCachedMoreApps.add(location);
                    mNativeListener.didCacheMoreApps(location);
                }
            });
        }

        @Override
        public void didFailToLoadMoreApps(final String location, final CBError.CBImpressionError error) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mNativeListener.didFailToLoadMoreApps(location, error.ordinal());
                }
            });
        }

        @Override
        public void didCacheRewardedVideo(final String location) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mCachedRewardedVideos.add(location);
                    mNativeListener.didCacheRewardedVideo(location);
                }
            });
        }

        @Override
        public void didFailToLoadRewardedVideo(final String location, final CBError.CBImpressionError error) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mNativeListener.didFailToLoadRewardedVideo(location, error.ordinal());
                }
            });
        }

        @Override
        public void didCompleteRewardedVideo(final String location, final int reward) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mNativeListener.didCompleteRewardedVideo(location, reward);
                }
            });
        }

        @Override
        public void didCloseInterstitial(String location) {
            super.didCloseInterstitial(location);
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mNativeListener.didCloseInterstitial();
                }
            });
        }

        @Override
        public void didCloseRewardedVideo(String location) {
            super.didCloseRewardedVideo(location);
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mNativeListener.didCloseRewardedVideo();
                }
            });
        }
    }


    //
    // Track lifecycle of activity and manage Chartboost respectively
    //

    private class ActivityLifecycleListener implements MultiplexerActivity.LifecycleListener {
        private final String mAppId;
        private final String mAppSignature;

        ActivityLifecycleListener(String appId, String appSignature) {
            mAppId = appId;
            mAppSignature = appSignature;
        }

        @Override
        public void onCreate(Activity activity, Bundle savedInstanceState) {
            Chartboost.startWithAppId(activity, mAppId, mAppSignature);
            Chartboost.setImpressionsUseActivities(true);
            Chartboost.setDelegate(ChartboostMultiplexerDelegate.getInstance());
            Chartboost.onCreate(activity);
        }

        @Override
        public void onStart(Activity activity) {
            Chartboost.onStart(activity);
        }

        @Override
        public void onStop(Activity activity) {
            Chartboost.onStop(activity);
        }

        @Override
        public void onResume(Activity activity) {
            Chartboost.onPause(activity);
        }

        @Override
        public void onPause(Activity activity) {
            Chartboost.onResume(activity);
        }

        @Override
        public void onDestroy(Activity activity) {
            Chartboost.onDestroy(activity);
        }

        @Override
        public boolean onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {
            return false;
        }

        @Override
        public void onNewIntent(Activity activity, Intent intent) {
            //nothing to do
        }

        @Override
        public boolean onBackPressed(Activity activity) {
            return Chartboost.onBackPressed();
        }
    }


    // Keep these instances with strong reference
    private final ChartboostNativeListener mNativeListener;
    private final ActivityLifecycleListener mLifecycleTracker;

    //
    // Startup
    //

    private static ChartboostX sInstance;

    public static void start(String appId, String appSignature) {
        sInstance = new ChartboostX(appId, appSignature);
    }

    public static ChartboostX getInstance() {
        return sInstance;
    }

    private ChartboostX(String appId, String appSignature) {
        mNativeListener = new ChartboostNativeListener();
        ChartboostMultiplexerDelegate.getInstance().addDelegate(mNativeListener);

        mLifecycleTracker = new ActivityLifecycleListener(appId, appSignature);
        MultiplexerActivity.getInstance().addLifecycleListener(mLifecycleTracker);
    }

    //
    // Interstitial
    //

    public boolean hasInterstitial(String location) {
        return mCachedInterstitials.contains(location);
    }

    public void cacheInterstitial(final String location) {
        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Chartboost.cacheInterstitial(location);
            }
        });
    }

    public void presentInterstitial(final String location) {
        mCachedInterstitials.remove(location);
        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Chartboost.showInterstitial(location);
            }
        });
    }


    //
    // More apps
    //

    public boolean hasMoreApps(String location) {
        return mCachedMoreApps.contains(location);
    }

    public void cacheMoreApps(final String location) {
        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Chartboost.cacheMoreApps(location);
            }
        });
    }

    public void presentMoreApps(final String location) {
        mCachedMoreApps.remove(location);
        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Chartboost.showMoreApps(location);
            }
        });
    }


    //
    // Rewarded video
    //

    public boolean hasRewardedVideo(String location) {
        return mCachedRewardedVideos.contains(location);
    }

    public void cacheRewardedVideo(final String location) {
        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Chartboost.cacheRewardedVideo(location);
            }
        });
    }

    public void presentRewardedVideo(final String location) {
        mCachedRewardedVideos.remove(location);
        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Chartboost.showRewardedVideo(location);
            }
        });
    }
}
