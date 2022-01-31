package com.planemo.libs.ads;

import android.os.Looper;
import android.util.Log;
import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.InterstitialAd;
import com.planemo.libs.MultiplexerActivity;
import org.cocos2dx.lib.Cocos2dxHelper;

@SuppressWarnings("unused")
public class AdMobInterstitial extends AdListener {
    @SuppressWarnings("unused")
    private static final String TAG = AdMobInterstitial.class.getName();

    private InterstitialAd mInterstitial;
    private final String mAdUnitId;
    private boolean mPresentWhenLoaded;
    private boolean isReady;
    private AdmobNativeListener mNativeListener;


    public AdMobInterstitial(String adUnitId, long pointerToNativeBannerObject) {
        mAdUnitId = adUnitId;
        isReady = false;
        mPresentWhenLoaded = false;
        mNativeListener = new AdmobNativeListener(pointerToNativeBannerObject);
    }

    public void load() {
        mPresentWhenLoaded = false;

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mInterstitial == null) {
                    isReady = false;
                    mInterstitial = new InterstitialAd(MultiplexerActivity.getInstance());
                    mInterstitial.setAdUnitId(mAdUnitId);
                    mInterstitial.setAdListener(AdMobInterstitial.this);

                    AdRequest adRequest = new AdRequest.Builder()
                            .addTestDevice("CCAF300DF1CEFB98D53C02D2721A869D")
                            .build();

                    mInterstitial.loadAd(adRequest);
                }
            }
        });
    }

    public void present() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (mInterstitial == null) {
                    load();
                }
                if (mInterstitial.isLoaded()) {
                    mInterstitial.show();
                    mInterstitial = null;
                } else {
                    mPresentWhenLoaded = true;
                }
            }
        });
    }

    public boolean isReady() {
        return mInterstitial != null && isReady;
    }

    public boolean isLoading() {
        return mInterstitial != null && !isReady;
    }

    private void runOnUiThread(Runnable runnable) {
        if (Looper.myLooper() == Looper.getMainLooper()) {
            runnable.run();
        } else {
            MultiplexerActivity.getInstance().runOnUiThread(runnable);
        }
    }

    @Override
    public void onAdFailedToLoad(int errorCode) {
        isReady = false;
        if (mInterstitial != null) {
            mInterstitial.setAdListener(null);
            mInterstitial = null;
        }
        Log.v(TAG, "admob onAdFailedToLoad");
    }

    @Override
    public void onAdLoaded() {
        isReady = true;
        if (mPresentWhenLoaded) {
            present();
        }
        Log.v(TAG, "admob onAdLoaded");
    }

    @Override
    public void onAdClosed() {
        Log.v(TAG, "admob onAdClosed");
        if (mNativeListener != null) {
            mNativeListener.onAdWillHide();
        }
    }

    @Override
    public void onAdOpened() {
        Log.v(TAG, "admob onAdOpened");
        if (mNativeListener != null) {
            mNativeListener.onAdWillShow();
        }
    }

    public void prepareForRelease() {
        if(mNativeListener != null) {
            mNativeListener.resetNativePointer();
            mNativeListener = null;
        }
        mInterstitial = null;
    }

    public static class Listener {
        public native void onAdWillShow(long pointerToNative);
        public native void onAdWillHide(long pointerToNative);        
    }

    public class AdmobNativeListener {
        private long mPointerToNativeBannerObject;
        private Listener nativeListener = new Listener();


        AdmobNativeListener(long pointerNative){
            mPointerToNativeBannerObject = pointerNative;
        }

        //Reset pointer, if c++ object has been destroyed
        public void resetNativePointer() {
            mPointerToNativeBannerObject = 0;
        }

        public void onAdWillShow() {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    if(mPointerToNativeBannerObject != 0)
                        nativeListener.onAdWillShow(mPointerToNativeBannerObject);
                }
            });
        }

        public void onAdWillHide() {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    if(mPointerToNativeBannerObject != 0)
                        nativeListener.onAdWillHide(mPointerToNativeBannerObject);
                }
            });
        }
    }
}
