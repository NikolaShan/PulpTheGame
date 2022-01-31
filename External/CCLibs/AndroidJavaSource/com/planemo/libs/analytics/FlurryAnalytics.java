package com.planemo.libs.analytics;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import com.flurry.android.FlurryAgent;
import com.planemo.libs.MultiplexerActivity;

import java.util.Map;

/**
 * Created by edward on 15.06.15.
 */
@SuppressWarnings("unused")
public class FlurryAnalytics {

    private static FlurryAnalytics mInstance;


    private FlurryAnalytics(String api) {
        ActivityLifecycleListener lifecycleTracker = new ActivityLifecycleListener(api);
        MultiplexerActivity.getInstance().addLifecycleListener(lifecycleTracker);
    }

    public static void init(String apiKey) {
        mInstance = new FlurryAnalytics(apiKey);
    }


    public static FlurryAnalytics getInstance() {
        return mInstance;
    }

    public static void logEvent(String event) {
        FlurryAgent.logEvent(event);
    }

    public static void logEvent(String event, Map<String, String> params) {
        FlurryAgent.logEvent(event, params);
    }

    public static void logTimedEvent(String event) {
        FlurryAgent.logEvent(event, true);
    }

    public static void logTimedEvent(String event, Map<String, String> params) {
        FlurryAgent.logEvent(event, params, true);
    }

    public static void endTimedEvent(String event) {
        FlurryAgent.endTimedEvent(event);
    }

    public static void endTimedEvent(String event, Map<String, String> params) {
        FlurryAgent.endTimedEvent(event, params);
    }

    public static void logPageView(String pageName) {
        logEvent(pageName);
        FlurryAgent.onPageView();
    }

    private class ActivityLifecycleListener implements MultiplexerActivity.LifecycleListener {
        private String mApiKey;

        public ActivityLifecycleListener(String apiKey) {
            mApiKey = apiKey;
        }

        @Override
        public void onCreate(Activity activity, Bundle savedInstanceState) {
            // configure Flurry
            FlurryAgent.setLogEnabled(false);
            // init Flurry
            FlurryAgent.init(activity, mApiKey);
        }

        @Override
        public void onStart(Activity activity) {
            FlurryAgent.onStartSession(activity);
        }

        @Override
        public void onStop(Activity activity) {
            FlurryAgent.onEndSession(activity);
        }

        @Override
        public void onResume(Activity activity) {
        }

        @Override
        public void onPause(Activity activity) {
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
}
