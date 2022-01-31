package com.planemo.libs.analytics;

import android.os.Bundle;
import com.google.firebase.analytics.FirebaseAnalytics;
import org.cocos2dx.lib.Cocos2dxHelper;
import java.util.Map;

public class FirebaseAnalyticsManager {

    private static FirebaseAnalyticsManager mInstance;
    private static FirebaseAnalytics mFirebaseAnalytics;


    public static FirebaseAnalyticsManager getInstance(){
        if(mInstance == null) {
            mInstance = new FirebaseAnalyticsManager();
            mFirebaseAnalytics = FirebaseAnalytics.getInstance(Cocos2dxHelper.getActivity());
        }
        return mInstance;
    }

    public void start() {
        //nothing to do
    }

    public void sendEvent(String event, Map<String, String> params){
        if(mFirebaseAnalytics != null && params != null){
            Bundle bundle = new Bundle();
            for(Map.Entry<String, String> entry : params.entrySet()){
                bundle.putString(entry.getKey(), entry.getValue());
            }
            mFirebaseAnalytics.logEvent(event, bundle);
        }
    }
}
