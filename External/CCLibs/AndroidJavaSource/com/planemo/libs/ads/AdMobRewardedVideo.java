package com.planemo.libs.ads;

import android.app.Activity;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.reward.RewardItem;
import com.google.android.gms.ads.reward.RewardedVideoAd;
import com.google.android.gms.ads.reward.RewardedVideoAdListener;
import com.planemo.libs.MultiplexerActivity;

import org.cocos2dx.lib.Cocos2dxHelper;

public class AdMobRewardedVideo {
    private static AdMobRewardedVideo sInstance = null;
    private RewardedVideoAd mAd;

    public static AdMobRewardedVideo getInstance() {
        if (sInstance == null) {
            sInstance = new AdMobRewardedVideo();
        }
        return sInstance;
    }

    private AdMobRewardedVideo() {
        mAd = MobileAds.getRewardedVideoAdInstance(Cocos2dxHelper.getActivity());
        mAd.setRewardedVideoAdListener(new NativeListenerForwarder());
        MultiplexerActivity.getInstance().addLifecycleListener(new LifecycleHandler());
    }

    public void load(final String adUnitId) {
        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mAd.loadAd(adUnitId, new AdRequest.Builder().build());
            }
        });
    }

    public void present() {
        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mAd.isLoaded()) {
                    mAd.show();
                }
            }
        });
    }

    public static class Listener {
        public native void onAdOpened();
        public native void onAdClosed();
        public native void onAdLoaded();
        public native void onAdFailedToLoad();
        public native void onRewarded();
    }

    private class NativeListenerForwarder implements RewardedVideoAdListener {
        Listener nativeListener = new Listener();

        @Override
        public void onRewardedVideoAdLoaded() {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onAdLoaded();
                }
            });
        }

        @Override
        public void onRewardedVideoAdOpened() {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onAdOpened();
                }
            });
        }

        @Override
        public void onRewardedVideoStarted() {

        }

        @Override
        public void onRewardedVideoAdClosed() {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onAdClosed();
                }
            });
        }

        @Override
        public void onRewarded(RewardItem rewardItem) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onRewarded();
                }
            });
        }

        @Override
        public void onRewardedVideoAdLeftApplication() {

        }

        @Override
        public void onRewardedVideoAdFailedToLoad(int i) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onAdFailedToLoad();
                }
            });
        }
    }

    private class LifecycleHandler extends MultiplexerActivity.AbstractLifecycleListener {
        @Override
        public void onResume(Activity activity) {
            mAd.resume(activity);
        }

        @Override
        public void onPause(Activity activity) {
            mAd.pause(activity);
        }

        @Override
        public void onDestroy(Activity activity) {
            mAd.destroy(activity);
        }
    }
}
