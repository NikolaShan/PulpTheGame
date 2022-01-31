package com.planemo.libs.ads;


import android.content.Context;
import android.os.Bundle;
import android.support.annotation.IntegerRes;
import android.util.Log;
import com.chartboost.sdk.CBLocation;
import com.chartboost.sdk.Chartboost;
import com.chartboost.sdk.ChartboostDelegate;
import com.chartboost.sdk.Model.CBError;
import com.google.ads.AdRequest;
import com.google.android.gms.ads.mediation.MediationAdRequest;
import com.google.android.gms.ads.mediation.customevent.CustomEventInterstitial;
import com.google.android.gms.ads.mediation.customevent.CustomEventInterstitialListener;

import java.util.Random;

@SuppressWarnings("unused")
public class AdMobChartboostAdapter extends ChartboostDelegate implements CustomEventInterstitial {
    @SuppressWarnings("unused")
    private static final String TAG = AdMobChartboostAdapter.class.getName();
    private String mLocation;
    private CustomEventInterstitialListener mListener;

    public AdMobChartboostAdapter() {
        Log.v(TAG, "Create Chartboost adapter");
        mLocation = CBLocation.LOCATION_GAME_SCREEN;
        ChartboostMultiplexerDelegate.getInstance().addDelegate(this);
    }

    @Override
    public void requestInterstitialAd(Context context,
                                      CustomEventInterstitialListener customEventInterstitialListener,
                                      String serverParameter,
                                      MediationAdRequest mediationAdRequest,
                                      Bundle bundle) {
        mListener = customEventInterstitialListener;
        String location = CBLocation.LOCATION_GAME_SCREEN;

        if (serverParameter != null) {
            String[] components = serverParameter.split(";");
            for (String component : components) {
                String[] keyValue = component.split("=");
                if (keyValue.length == 2) {
                    String key = keyValue[0];
                    String value = keyValue[1];

                    if (key.equalsIgnoreCase("location")) {
                        location = value;
                    }
                }
            }
        }

        mLocation = location;
        if (Chartboost.hasInterstitial(location)) {
            didCacheInterstitial(location);
        } else {
            Chartboost.cacheInterstitial(location);
        }
    }

    @Override
    public void showInterstitial() {
        Chartboost.showInterstitial(mLocation);
    }

    @Override
    public void onDestroy() {
        Log.v(TAG, "Destroy chartboost adapter");
        ChartboostMultiplexerDelegate.getInstance().removeDelegate(this);
    }

    @Override
    public void onPause() {
        //nothing to do
    }

    @Override
    public void onResume() {
        //nothing to do
    }

    @Override
    public void didCacheInterstitial(String location) {
        if (mLocation != null && mLocation.equals(location)) {
            mListener.onAdLoaded();
        }
    }

    @Override
    public void didFailToLoadInterstitial(String location, CBError.CBImpressionError error) {
        if (mLocation != null && mLocation.equals(location)) {
            AdRequest.ErrorCode amerror;
            switch (error) {
                case TOO_MANY_CONNECTIONS:
                case NETWORK_FAILURE:
                case NO_HOST_ACTIVITY:
                case INVALID_RESPONSE:
                case ASSETS_DOWNLOAD_FAILURE:
                case INTERNET_UNAVAILABLE:
                    amerror = AdRequest.ErrorCode.NETWORK_ERROR;
                    break;

                case NO_AD_FOUND:
                case VIDEO_UNAVAILABLE:
                    amerror = AdRequest.ErrorCode.NO_FILL;
                    break;

                default:
                    amerror = AdRequest.ErrorCode.INTERNAL_ERROR;
                    break;
            }
            mListener.onAdFailedToLoad(amerror.ordinal());
        }
    }

    @Override
    public void didClickInterstitial(String location) {
        if (mLocation != null && mLocation.equals(location)) {
            mListener.onAdClicked();
        }
    }

    @Override
    public void didCloseInterstitial(String location) {
        if (mLocation != null && mLocation.equals(location)) {
            mListener.onAdClosed();
        }
    }

    @Override
    public void didDismissInterstitial(String location) {
        if (mLocation != null && mLocation.equals(location)) {
            mListener.onAdClosed();
        }
        Log.v(TAG, "Remove on dismiss");
        ChartboostMultiplexerDelegate.getInstance().removeCurrentDelegate();
    }

    @Override
    public void didDisplayInterstitial(String location) {
        if (mLocation != null && mLocation.equals(location)) {
            mListener.onAdOpened();
        }
    }
}
