package com.planemo.libs.ads;


import android.app.Activity;
import android.os.Handler;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.heyzap.sdk.ads.BannerAdView;
import com.heyzap.sdk.ads.HeyzapAds;
import com.heyzap.sdk.ads.IncentivizedAd;
import com.heyzap.sdk.ads.InterstitialAd;
import com.planemo.libs.MultiplexerActivity;
import com.planemo.libs.support.AppUtils;
import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.TimeUnit;

@SuppressWarnings("unused")
public class HeyzapX {
    @SuppressWarnings("unused")
    private static final String TAG = HeyzapX.class.getName();

    private NativeListener mNativeListener = new NativeListener();
    private MultiplexerActivity.LifecycleListener mLifecycleTracker;
    private AdStatusListener mInterstitialStatusListener = new InterstitialAdStatusListener();
    private IncentivizedAdStatusListener mRewardsListener = new IncentivizedAdStatusListener();

    private Set<String> mLoadingInterstitials = new HashSet<>();
    private Set<String> mReadyInterstitials = new HashSet<>();

    private Set<String> mLoadingRewardedVideos = new HashSet<>();
    private Set<String> mReadyRewardedVideos = new HashSet<>();

    private static HeyzapX sInstance = new HeyzapX();
    public static HeyzapX getInstance() {
        return sInstance;
    }

    public void start(String publisherId) {
        HeyzapAds.start(publisherId, MultiplexerActivity.getInstance(), HeyzapAds.DISABLE_AUTOMATIC_FETCH);
        mLifecycleTracker = new HeyzapActivityLifecycleListener();
        MultiplexerActivity.getInstance().addLifecycleListener(mLifecycleTracker);

        InterstitialAd.setOnStatusListener(mInterstitialStatusListener);
        IncentivizedAd.setOnStatusListener(mRewardsListener);

        IncentivizedAd.setOnIncentiveResultListener(mRewardsListener);
    }

    public void loadInterstitial(final String tag) {
        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (tag.isEmpty()) {
                    InterstitialAd.fetch();
                } else {
                    InterstitialAd.fetch(tag);
                }
            }
        });
    }

    public void presentInterstitial(final String tag) {
        mReadyInterstitials.remove(tag);

        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (tag.isEmpty()) {
                    InterstitialAd.display(Cocos2dxHelper.getActivity());
                } else {
                    InterstitialAd.display(Cocos2dxHelper.getActivity(), tag);
                }
            }
        });
    }

    public boolean isInterstitialReady(String tag) {
        return mReadyInterstitials.contains(tag);
    }

    public boolean isInterstitialLoading(String tag) {
        return mLoadingInterstitials.contains(tag);
    }


    public void loadRewardedVideo(final String tag) {
        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (tag.isEmpty()) {
                    IncentivizedAd.fetch();
                } else {
                    IncentivizedAd.fetch(tag);
                }
            }
        });
    }

    public void presentRewardedVideo(final String tag) {
        mReadyRewardedVideos.remove(tag);

        Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (tag.isEmpty()) {
                    IncentivizedAd.display(Cocos2dxHelper.getActivity());
                } else {
                    IncentivizedAd.display(Cocos2dxHelper.getActivity(), tag);
                }
            }
        });
    }

    public boolean isRewardedVideoReady(String tag) {
        return mReadyRewardedVideos.contains(tag);
    }

    public boolean isRewardedVideoLoading(String tag) {
        return mLoadingRewardedVideos.contains(tag);
    }


    private class HeyzapActivityLifecycleListener extends MultiplexerActivity.AbstractLifecycleListener {
        @Override
        public boolean onBackPressed(Activity activity) {
            return HeyzapAds.onBackPressed();
        }
    }

    public abstract class AdStatusListener implements HeyzapAds.OnStatusListener {
        @Override
        public void onShow(String s) {
            mNativeListener.onAdWillShow();
        }

        @Override
        public void onClick(String s) {

        }

        @Override
        public void onFailedToShow(String s) {

        }

        @Override
        public void onAudioStarted() {

        }

        @Override
        public void onAudioFinished() {

        }
    }

    public class InterstitialAdStatusListener extends AdStatusListener {
        @Override
        public void onAvailable(String s) {
            Log.v(TAG, "On interstitial ready: " + s);
            mLoadingInterstitials.remove(s);
            mReadyInterstitials.add(s);
        }

        @Override
        public void onFailedToFetch(String s) {
            Log.v(TAG, "On failed to fetch interstitial: " + s);
            mLoadingInterstitials.remove(s);
        }

        @Override
        public void onHide(String s) {
            mNativeListener.onAdWillHide(false);
        }
    }

    public class IncentivizedAdStatusListener extends AdStatusListener implements HeyzapAds.OnIncentiveResultListener {

        @Override
        public void onAvailable(String s) {
            mLoadingRewardedVideos.remove(s);
            mReadyRewardedVideos.add(s);
            mNativeListener.onRewardedVideoLoadCompletion(true);
        }

        @Override
        public void onFailedToFetch(String s) {
            mLoadingRewardedVideos.remove(s);
            mNativeListener.onRewardedVideoLoadCompletion(false);
        }

        @Override
        public void onComplete(String s) {
            mNativeListener.onRewardedVideoViewCompletion(true);;
        }

        @Override
        public void onIncomplete(String s) {
            mNativeListener.onRewardedVideoViewCompletion(false);
        }

        @Override
        public void onHide(String s) {
            mNativeListener.onAdWillHide(true);
        }
    }

    public static class Listener {
        public native void onAdWillShow();
        public native void onAdWillHide(boolean rewardedVideo);
        public native void onRewardedVideoLoadCompletion(boolean success);
        public native void onRewardedVideoViewCompletion(boolean success);
    }

    private static class NativeListener {
        private Listener mNativeListener = new Listener();
        
        public void onAdWillShow() {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mNativeListener.onAdWillShow();
                }
            });
        }

        public void onAdWillHide(final boolean rewardedVideo) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mNativeListener.onAdWillHide(rewardedVideo);
                }
            });
        }

        public void onRewardedVideoLoadCompletion(final boolean success) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mNativeListener.onRewardedVideoLoadCompletion(success);
                }
            });

        }

        public void onRewardedVideoViewCompletion(final boolean success) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    mNativeListener.onRewardedVideoViewCompletion(success);
                }
            });
        }
    }

    /**
     * Banner ad
     */
    public static class BannerX implements HeyzapAds.BannerListener {
        private final String mTag;
        private BannerAdView mBannerAdView;
        private MultiplexerActivity.LifecycleListener mLifecycleTracker;
        private float yAnchor;
        private float yPercent;
        private List<HeyzapAds.CreativeSize> mPreferredBannerSizes = new ArrayList<>();
        private int mPreferredBannerNumber = 0;
        private Handler mHandler = new Handler();

        public BannerX(String tag) {
            mTag = tag;
            if (AppUtils.isTablet()) {
                mPreferredBannerSizes.add(HeyzapAds.CreativeSize.SMART_BANNER);
                mPreferredBannerSizes.add(HeyzapAds.CreativeSize.BANNER_HEIGHT_90);
                mPreferredBannerSizes.add(HeyzapAds.CreativeSize.LEADERBOARD);
                mPreferredBannerSizes.add(HeyzapAds.CreativeSize.FULL_BANNER);
                mPreferredBannerSizes.add(HeyzapAds.CreativeSize.BANNER_320_50);
            } else {
                mPreferredBannerSizes.add(HeyzapAds.CreativeSize.SMART_BANNER);
                mPreferredBannerSizes.add(HeyzapAds.CreativeSize.BANNER_320_50);
            }
        }

        public void present(float xPercent, float yPercent, float yAnchor) {
            this.yAnchor = yAnchor;
            this.yPercent = yPercent;

            final MultiplexerActivity activity = MultiplexerActivity.getInstance();
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (mBannerAdView == null) {
                        mBannerAdView = new BannerAdView(activity);
                        HeyzapAds.BannerOptions bannerOptions = mBannerAdView.getBannerOptions();
                        HeyzapAds.CreativeSize size = getPreferredBannerSize();

                        bannerOptions.setAdmobBannerSize(size);
                        if (AppUtils.isTablet()) {
                            bannerOptions.setFacebookBannerSize(HeyzapAds.CreativeSize.BANNER_HEIGHT_90);
                        } else {
                            bannerOptions.setFacebookBannerSize(HeyzapAds.CreativeSize.BANNER_HEIGHT_50);
                        }

                    } else if (mBannerAdView.getParent() != null) {
                        //present method call more than once
                        return;
                    }

                    FrameLayout layout = activity.getRootLayout();
                    FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                            ViewGroup.LayoutParams.WRAP_CONTENT);
                    layoutParams.gravity = Gravity.BOTTOM;
                    layout.addView(mBannerAdView, layoutParams);

                    if (mLifecycleTracker == null) {
                        mLifecycleTracker = new ActivityLifecycleListener();
                        activity.addLifecycleListener(mLifecycleTracker);
                    }
                    mBannerAdView.setVisibility(View.INVISIBLE);
                    mBannerAdView.setBannerListener(BannerX.this);
                    Log.v(TAG, "Load new banner");
                    if (mTag.isEmpty()) {
                        mBannerAdView.load();
                    } else {
                        mBannerAdView.load(mTag);
                    }
                }
            });
        }

        public void remove() {
            Log.v(TAG, "Remove banner");
            mHandler.removeCallbacks(null);
            final MultiplexerActivity activity = MultiplexerActivity.getInstance();
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (mBannerAdView != null) {
                        activity.getRootLayout().removeView(mBannerAdView);
                    }

                    if (mLifecycleTracker != null) {
                        activity.removeLifecycleListener(mLifecycleTracker);
                        mLifecycleTracker = null;
                    }
                }
            });
        }

        @Override
        public void onAdError(BannerAdView var1, HeyzapAds.BannerError var2) {
            Log.v(TAG, "Error loading heyzap banner: " + var2.getErrorMessage());
            retryWithNextBannerSize();
        }

        @Override
        public void onAdLoaded(BannerAdView var1) {
            Log.v(TAG, "On banner loaded");
            if (mBannerAdView != null) {
                FrameLayout layout = MultiplexerActivity.getInstance().getRootLayout();
                FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.WRAP_CONTENT);
                layoutParams.gravity = Gravity.BOTTOM;
                layoutParams.bottomMargin = (int) (layout.getHeight() * yPercent - yAnchor * mBannerAdView.getHeight());
                mBannerAdView.setLayoutParams(layoutParams);
                mBannerAdView.setVisibility(View.VISIBLE);
            }
        }

        @Override
        public void onAdClicked(BannerAdView var1) {

        }

        private class ActivityLifecycleListener extends MultiplexerActivity.AbstractLifecycleListener {
            @Override
            public void onDestroy(Activity activity) {
                if (mBannerAdView != null) {
                    mBannerAdView.destroy();
                    mBannerAdView = null;
                }
            }
        }

        private void retryWithNextBannerSize() {
            if (mBannerAdView != null) {
                MultiplexerActivity.getInstance().getRootLayout().removeView(mBannerAdView);
                mBannerAdView = null;
            }
            mPreferredBannerNumber += 1;
            if (mPreferredBannerNumber >= mPreferredBannerSizes.size()) {
                mPreferredBannerNumber = 0;
                mHandler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        present(0.5f, yPercent, yAnchor);
                    }
                }, TimeUnit.SECONDS.toMillis(20));
            } else {
                present(0.5f, yPercent, yAnchor);
            }
        }

        private HeyzapAds.CreativeSize getPreferredBannerSize() {
            return mPreferredBannerSizes.get(mPreferredBannerNumber);
        }
    }
}
