package com.planemo.libs.ads;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import com.jirbo.adcolony.*;
import com.planemo.libs.MultiplexerActivity;
import com.planemo.libs.support.AppUtils;
import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.List;


/**
 * Created by edward on 26/07/15.
 *
 */
public class AdColonyManager {
    public static final String TAG = AdColonyManager.class.getName();
    private static AdColonyManager mInstance;
    private AdColonyNativeListener mAdColonyNativeListener;
    private String mAppId;
    private String mDefaultInterstitialZoneId;
    private String[] mZonesId;


    private AdColonyManager() {
        ActivityLifecycleListener lifecycleTracker = new ActivityLifecycleListener();
        MultiplexerActivity.getInstance().addLifecycleListener(lifecycleTracker);
        mAdColonyNativeListener = new AdColonyNativeListener();
    }

    public static void init(String appId, List<String> zonesIds) {
        Log.v(TAG, "init");
        mInstance = new AdColonyManager();
        mInstance.mAppId = appId;
        if(zonesIds != null){
            mInstance.mZonesId = new String[zonesIds.size()];
            zonesIds.toArray(mInstance.mZonesId);
        }
    }

    public static AdColonyManager getInstance() {
        if (mInstance == null)
            mInstance = new AdColonyManager();
        return mInstance;
    }

    public boolean isVirtualCurrencyRewardAvailableForZone(String zoneId){
        //For test video returned "unknown"
        String status = AdColony.statusForZone(zoneId);
        return status.equals("active") || status.equals("unknown");

    }

    public int getVirtualCurrencyRewardsAvailableTodayForZone(String zoneId){
        return AdColony.getRemainingV4VCForZone(zoneId);
    }

    public void playRewardedVideoForZone(String zoneId){
        Log.v(TAG, "playRewardedVideoForZone id = " + zoneId);
        AdColonyV4VCAd ad = new AdColonyV4VCAd(zoneId).withResultsDialog().withListener(new AdColonyAdListener() {
            @Override
            public void onAdColonyAdAttemptFinished(AdColonyAd adColonyAd) {
                // You can ping the AdColonyAd object here for more information:
                // ad.shown() - returns true if the ad was successfully shown.
                // ad.notShown() - returns true if the ad was not shown at all (i.e. if onAdColonyAdStarted was never triggered)
                // ad.skipped() - returns true if the ad was skipped due to an interval play setting
                // ad.canceled() - returns true if the ad was cancelled (either programmatically or by the user)
                // ad.noFill() - returns true if the ad was not shown due to no ad fill.
                Log.v(TAG, "onAdColonyAdAttemptFinished " + adColonyAd.shown() + " " + adColonyAd.canceled() + " " + adColonyAd.getAvailableViews() +
                        "  " + adColonyAd.noFill() + "  " + adColonyAd.notShown() + " " + adColonyAd.skipped());
                if (adColonyAd.shown()) {
                    mAdColonyNativeListener.onCompletionCallback(true);
                } else {
                    mAdColonyNativeListener.onCompletionCallback(false);
                }
            }

            @Override
            public void onAdColonyAdStarted(AdColonyAd adColonyAd) {
                Log.v(TAG, "onAdColonyAdStarted");
            }

        });
        ad.show();
    }

    public void setDefaultInterstitialZoneID(String zoneID){
        Log.v(TAG, "setDefaultInterstitialZoneID " + zoneID);
        mDefaultInterstitialZoneId = zoneID;
    }

    public String getDefaultInterstitialZoneId(){
        return mDefaultInterstitialZoneId;
    }

    private class ActivityLifecycleListener implements MultiplexerActivity.LifecycleListener {

        public ActivityLifecycleListener() {

        }

        @Override
        public void onCreate(Activity activity, Bundle savedInstanceState) {
            if(mAppId != null && mZonesId != null) {
                String client_options = AppUtils.getVersionName() + ",store:google"; //If Amazon - amazon
                AdColony.configure(activity, client_options, mAppId, mZonesId);

                AdColonyV4VCListener listener = new AdColonyV4VCListener()
                {
                    public void onAdColonyV4VCReward(AdColonyV4VCReward reward)
                    {
                        if(reward != null && reward.success())
                        {
                            //Reward user
                            Log.v(TAG, "Reward user");
                            mAdColonyNativeListener.onHandleRewardCallback(reward.name(), reward.amount());
                        }
                    }
                };

                AdColony.addV4VCListener(listener);
                AdColony.addAdAvailabilityListener(new AdColonyAdAvailabilityListener() {
                    @Override
                    public void onAdColonyAdAvailabilityChange(boolean b, String s) {
                        Log.v(TAG, "onAdColonyAdAvailabilityChange b " + b + " s " + s);
                    }
                });
            }
        }

        @Override
        public void onStart(Activity activity) {
        }

        @Override
        public void onStop(Activity activity) {
        }

        @Override
        public void onResume(Activity activity) {
            AdColony.resume(activity);
        }

        @Override
        public void onPause(Activity activity) {
            AdColony.pause();
        }

        @Override
        public void onDestroy(Activity activity) {
        }

        @Override
        public boolean onBackPressed(Activity activity) {
            return false;
        }

        @Override
        public boolean onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {
            return false;
        }
    }


    public static class Listener {
        public native void onCompletionCallback(boolean isSuccess);
        public native void onHandleRewardCallback(String currencyName, int amount);
    }

    public class AdColonyNativeListener {
        private Listener nativeListener = new Listener();

        public void onCompletionCallback(final boolean isSuccess) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onCompletionCallback(isSuccess);
                }
            });
        }

        public void onHandleRewardCallback(final String currencyName, final int amount) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onHandleRewardCallback(currencyName, amount);
                }
            });
        }
    }
}