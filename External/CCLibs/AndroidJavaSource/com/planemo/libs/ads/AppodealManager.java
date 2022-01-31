package com.planemo.libs.ads;


import android.app.Activity;
import android.content.Context;
import android.util.Log;
import com.appodeal.ads.Appodeal;
import com.appodeal.ads.InterstitialCallbacks;
import com.appodeal.ads.RewardedVideoCallbacks;

import com.appodeal.ads.SkippableVideoCallbacks;
import com.planemo.libs.MultiplexerActivity;
import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.List;

@SuppressWarnings("unused")
public class AppodealManager {
    private static final String TAG = AppodealManager.class.getName();

    private static AppodealManager sInstance;
    private AppodealNativeListener mNativeListener;
    private String[] networksToDisable = null;

    public static AppodealManager getInstance() {
        if (sInstance == null) {
            sInstance = new AppodealManager();
        }
        return sInstance;
    }

    public void initialize(String appKey) {
        Log.v(TAG, "Init appodeal");
       // Appodeal.setTesting(true);
        Appodeal.confirm(Appodeal.SKIPPABLE_VIDEO);
        Activity activity = MultiplexerActivity.getInstance();
        for (String network: networksToDisable) {
            Appodeal.disableNetwork(activity, network);
        }
        Appodeal.initialize(activity, appKey,
                Appodeal.INTERSTITIAL | Appodeal.BANNER | Appodeal.SKIPPABLE_VIDEO
                        | Appodeal.REWARDED_VIDEO);

        mNativeListener = new AppodealNativeListener();

        Appodeal.setInterstitialCallbacks(new InterstitialCallbacks() {
            @Override
            public void onInterstitialLoaded(boolean b) {

            }

            @Override
            public void onInterstitialFailedToLoad() {

            }

            @Override
            public void onInterstitialShown() {
                mNativeListener.onAdWillShow();
            }

            @Override
            public void onInterstitialClicked() {

            }

            @Override
            public void onInterstitialClosed() {
                mNativeListener.onAdWillHide(false);
            }
        });

        Appodeal.setSkippableVideoCallbacks(new SkippableVideoCallbacks() {

            @Override
            public void onSkippableVideoLoaded() {

            }

            @Override
            public void onSkippableVideoFailedToLoad() {

            }

            @Override
            public void onSkippableVideoShown() {
                mNativeListener.onAdWillShow();
            }

            @Override
            public void onSkippableVideoFinished() {

            }

            @Override
            public void onSkippableVideoClosed(boolean b) {
                mNativeListener.onAdWillHide(false);
            }
        });

        Appodeal.setRewardedVideoCallbacks(new RewardedVideoCallbacks() {
            private boolean isVideoFinished = false;

            @Override
            public void onRewardedVideoLoaded() {

            }

            @Override
            public void onRewardedVideoFailedToLoad() {

            }

            @Override
            public void onRewardedVideoShown() {
                mNativeListener.onAdWillShow();
            }

            @Override
            public void onRewardedVideoFinished(int i, String s) {
                isVideoFinished = true;
            }

            @Override
            public void onRewardedVideoClosed(boolean b) {
                mNativeListener.onAdWillHide(true);
                mNativeListener.onRewardedVideoCompletion(isVideoFinished);
            }
        });
    }

    //Call before calling super.onCreate
    public void disableNetworks(String[] networks) {
        networksToDisable = networks;
    }

    public void presentBannerAtPosition(float xPercent, float yPercent, float yAnchor) {
        final int position = Appodeal.BANNER_BOTTOM;

        final MultiplexerActivity activity = MultiplexerActivity.getInstance();
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Appodeal.show(activity, position);
            }
        });

    }

    public void removeBanner() {
        final MultiplexerActivity activity = MultiplexerActivity.getInstance();
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Appodeal.hide(activity, Appodeal.BANNER);
            }
        });
    }

    public boolean isInterstitialLoading() {
        return true;
    }

    public boolean isInterstitialReady() {
        return true;
    }

    public boolean isRewardedVideoReady() { return true; }

    public void loadInterstitial() {
        //nothing to do
    }

    public void loadRewardedVideo() {
        //nothing to do
    }

    public void presentInterstitial() {
        Log.v(TAG, "presentInterstitial");
        showAd(Appodeal.INTERSTITIAL);
    }

    public void presentInterstitialStaticOrVideo() {
        Log.v(TAG, "presentInterstitialStaticOrVideo");
        showAd(Appodeal.INTERSTITIAL | Appodeal.SKIPPABLE_VIDEO);
    }

    public void presentVideoInterstitial() {
        Log.v(TAG, "presentVideoInterstitial");
        showAd(Appodeal.SKIPPABLE_VIDEO);
    }

    public void presentRewardedVideo() {
        Log.v(TAG, "presentRewardedVideo");
        showAd(Appodeal.REWARDED_VIDEO);
    }

    private void showAd(final int adType) {
        MultiplexerActivity.getInstance().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Appodeal.show(MultiplexerActivity.getInstance(), adType);
            }
        });
    }

    public static class Listener {
        public native void onAdWillShow();
        public native void onAdWillHide(boolean rewardedVideo);
        public native void onRewardedVideoCompletion(boolean success);
    }

    public class AppodealNativeListener {
        private Listener nativeListener = new Listener();

        public void onAdWillShow() {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onAdWillShow();
                }
            });
        }

        public void onAdWillHide(final boolean rewardedVideo) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onAdWillHide(rewardedVideo);
                }
            });
        }

        public void onRewardedVideoCompletion(final boolean success) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onRewardedVideoCompletion(success);
                }
            });
        }
    }
}
