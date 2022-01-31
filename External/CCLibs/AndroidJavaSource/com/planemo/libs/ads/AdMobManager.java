package com.planemo.libs.ads;

import com.google.android.gms.ads.MobileAds;

import org.cocos2dx.lib.Cocos2dxHelper;


public class AdMobManager {
    public static void start(String appId) {
        MobileAds.initialize(Cocos2dxHelper.getActivity().getApplicationContext(), appId);
    }
}
