package com.planemo.libs.ads;

import android.app.Activity;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.planemo.libs.MultiplexerActivity;

@SuppressWarnings("unused")
public class AdMobBanner extends AdListener {

    //
    // Pause / Resume banner depending on Activity
    //
    private class BannerLifecycleTracker extends MultiplexerActivity.AbstractLifecycleListener {
        private final AdView mAdView;

        public BannerLifecycleTracker(AdView adView) {
            mAdView = adView;
        }

        @Override
        public void onPause(Activity activity) {
            mAdView.pause();
        }

        @Override
        public void onResume(Activity activity) {
            mAdView.resume();
        }

        @Override
        public void onDestroy(Activity activity) {
            mAdView.destroy();
        }
    }

    private final String mAdUnitId;
    private AdView mAdView;
    private BannerLifecycleTracker mLifecycleTracker;
    private float yAnchor;
    private float yPercent;

    public AdMobBanner(String adUnitId) {
        mAdUnitId = adUnitId;
    }

    public void present(float xPercent, float yPercent, float yAnchor) {
        this.yPercent = yPercent;
        this.yAnchor = yAnchor;

        final MultiplexerActivity activity = MultiplexerActivity.getInstance();
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mAdView == null) {
                    mAdView = new AdView(activity);
                    mAdView.setAdUnitId(mAdUnitId);
                    mAdView.setAdSize(AdSize.SMART_BANNER);
                }else if (mAdView.getParent() != null) {
                    //present method call more than once
                    return;
                }

                FrameLayout layout = activity.getRootLayout();
                FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                        ViewGroup.LayoutParams.WRAP_CONTENT);
                layout.addView(mAdView, layoutParams);

                if (mLifecycleTracker == null) {
                    mLifecycleTracker = new BannerLifecycleTracker(mAdView);
                    activity.addLifecycleListener(mLifecycleTracker);
                }
                mAdView.setAdListener(AdMobBanner.this);
                mAdView.setVisibility(View.INVISIBLE);
                mAdView.loadAd(new AdRequest.Builder().addTestDevice("CCAF300DF1CEFB98D53C02D2721A869D").build());
            }
        });
    }

    public void remove() {
        final MultiplexerActivity activity = MultiplexerActivity.getInstance();
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mAdView != null) {
                    activity.getRootLayout().removeView(mAdView);
                }

                if (mLifecycleTracker != null) {
                    activity.removeLifecycleListener(mLifecycleTracker);
                    mLifecycleTracker = null;
                }
            }
        });
    }

    @Override
    public void onAdLoaded() {
        if (mAdView != null) {
            FrameLayout layout = MultiplexerActivity.getInstance().getRootLayout();
            FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                    ViewGroup.LayoutParams.WRAP_CONTENT);
            layoutParams.gravity = Gravity.BOTTOM;
            layoutParams.bottomMargin = (int) (layout.getHeight() * yPercent - yAnchor * mAdView.getHeight());
            mAdView.setLayoutParams(layoutParams);
            mAdView.setVisibility(View.VISIBLE);
        }
    }
}
