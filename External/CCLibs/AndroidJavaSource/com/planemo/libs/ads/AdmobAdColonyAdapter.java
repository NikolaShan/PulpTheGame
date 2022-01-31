package com.planemo.libs.ads;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.mediation.MediationAdRequest;
import com.google.android.gms.ads.mediation.customevent.CustomEventInterstitial;
import com.google.android.gms.ads.mediation.customevent.CustomEventInterstitialListener;
import com.jirbo.adcolony.AdColonyAd;
import com.jirbo.adcolony.AdColonyAdListener;
import com.jirbo.adcolony.AdColonyVideoAd;
import com.jirbo.adcolony.AdColonyVideoDialog;
import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

import java.util.logging.LogRecord;


/**
 * Created by edward on 28/07/15.
 *
 */
public class AdmobAdColonyAdapter implements CustomEventInterstitial, AdColonyAdListener {

    public static final String TAG  = AdmobAdColonyAdapter.class.getName();
    private CustomEventInterstitialListener mListener;
    private AdColonyVideoAd mVideoInterstitial;
    private  int mCountForCheckingAdStatus = 5;

    @Override
    public void requestInterstitialAd(Context context, CustomEventInterstitialListener customEventInterstitialListener, String serverParameters, MediationAdRequest mediationAdRequest, Bundle bundle) {
        try {
            mListener = customEventInterstitialListener;

            if(!isNeedShow(serverParameters) && mListener != null){
                mListener.onAdFailedToLoad(AdRequest.ERROR_CODE_NO_FILL);
                return;
            }

            String defZoneId = AdColonyManager.getInstance().getDefaultInterstitialZoneId();
            if (defZoneId != null) {
                mVideoInterstitial = new AdColonyVideoAd(defZoneId).withListener(this);

                //Check status of Ads with this solution because AdColony - DNO EBANOE and it not have correct ads listener
                Timer timer = new Timer();
                final TimerTask task = new TimerTask() {
                    @Override
                    public void run () {
                       if(--mCountForCheckingAdStatus > 0) {
                            if(mVideoInterstitial.isReady()) {
                                Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        mListener.onAdLoaded();
                                    }
                                });
                                cancel();
                            }
                       }else {
                           mCountForCheckingAdStatus = 5;
                           Cocos2dxHelper.getActivity().runOnUiThread(new Runnable() {
                               @Override
                               public void run() {
                                   mListener.onAdFailedToLoad(AdRequest.ERROR_CODE_NO_FILL);
                               }
                           });
                           cancel();
                       }
                    }
                };
                // Advertising is cached at the start in AdColonyManager
                timer.schedule (task, 1000, 1000);

            }
        }catch (Exception ex){
            ex.printStackTrace();
            Log.e(TAG, "Error requestInterstitialAd");
        }

    }


    private boolean isNeedShow(String serverParameter){
        try {
            if (serverParameter != null) {
                String[] components = serverParameter.split(";");
                for (String component : components) {
                    String[] keyValue = component.split("=");
                    if (keyValue.length == 2) {
                        String key = keyValue[0];
                        String value = keyValue[1];
                        if (key.equalsIgnoreCase("chance")) {
                            Random random = new Random();
                            int chance = Integer.valueOf(value);
                            if (random.nextInt(100) > chance) {
                                return false;
                            }
                        }
                    }
                }
            }
        }   catch(Exception ex){
            return true;
        }
        return true;
    }

    @Override
    public void showInterstitial() {
        if(mVideoInterstitial != null && mVideoInterstitial.isReady())
            mVideoInterstitial.show();
    }

    @Override
    public void onDestroy() {

    }

    @Override
    public void onPause() {
        //Implemented in AdColonyManager
    }

    @Override
    public void onResume() {
        //Implemented in AdColonyManager
    }

    @Override
    public void onAdColonyAdAttemptFinished(AdColonyAd adColonyAd) {
        if(adColonyAd.shown() || adColonyAd.skipped() || adColonyAd.canceled()) {
            mListener.onAdClosed();
        }
    }

    @Override
    public void onAdColonyAdStarted(AdColonyAd adColonyAd) {
        if(mVideoInterstitial != null && adColonyAd != null && mListener != null)
            mListener.onAdOpened();
    }
}
