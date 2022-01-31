package com.planemo.libs.analytics;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import com.google.android.gms.analytics.GoogleAnalytics;
import com.google.android.gms.analytics.HitBuilders;
import com.google.android.gms.analytics.Logger;
import com.google.android.gms.analytics.Tracker;
import com.planemo.libs.MultiplexerActivity;

import java.util.Locale;


@SuppressWarnings("unused")
public class GoogleAnalyticsManager {
    public static final String TAG = GoogleAnalyticsManager.class.getName();
    private static GoogleAnalytics mAnalytics;
    private static Tracker mTracker;
    private static GoogleAnalyticsManager mInstance;

    public static GoogleAnalyticsManager getInstance() {
        return mInstance;
    }


    private GoogleAnalyticsManager(String trackerId) {
        ActivityLifecycleListener lifecycleTracker = new ActivityLifecycleListener(trackerId);
        MultiplexerActivity.getInstance().addLifecycleListener(lifecycleTracker);
    }

    public static void startWithTrackerId(String trackerId) {
        mInstance = new GoogleAnalyticsManager(trackerId);
    }


    public void set(String key, String value) {
        if (mTracker != null) {
            mTracker.set(key, value);
        }
    }

    public void setEnableDebugLogging(boolean enable) {
        if (mAnalytics != null && enable) {
            mAnalytics.getLogger().setLogLevel(Logger.LogLevel.VERBOSE);
        }
    }

    public void setDispatchInterval(float seconds) {
        if (mAnalytics != null) {
            mAnalytics.setLocalDispatchPeriod((int) seconds);
        }
    }

    public void enableAdvertisingIdCollection(boolean enable) {
        if (mTracker != null) {
            mTracker.enableAdvertisingIdCollection(enable);
        }
    }

    public void setScreen(String screenName) {
        if (mTracker != null) {
            mTracker.setScreenName(screenName);
        }
    }

    public void sendEvent(String category, String action, String label, int value) {
        if (mTracker != null) {
            // All subsequent hits will be send with screen name from method setScreen(...)
            mTracker.send(new HitBuilders.EventBuilder()
                    .setCategory(category)
                    .setAction(action)
                    .setLabel(label)
                    .setValue(value)
                    .build());
        }
    }

    public void sendEvent(String category, String action, String label) {
        if (mTracker != null) {
            // All subsequent hits will be send with screen name from method setScreen(...)
            mTracker.send(new HitBuilders.EventBuilder()
                    .setCategory(category)
                    .setAction(action)
                    .setLabel(label)
                    .build());
        }
    }

    public void sendTiming(String category, long valueMs, String name, String label) {
        if (mTracker != null) {
            // Build and send timing.
            mTracker.send(new HitBuilders.TimingBuilder()
                    .setCategory(category)
                    .setValue(valueMs)
                    .setVariable(name)
                    .setLabel(label)
                    .build());
        }
    }

    private class ActivityLifecycleListener extends MultiplexerActivity.AbstractLifecycleListener {
        private String mTrackerID;

        public ActivityLifecycleListener(String trackerID) {
            mTrackerID = trackerID;

        }

        @Override
        public void onCreate(Activity activity, Bundle savedInstanceState) {
            mAnalytics = GoogleAnalytics.getInstance(activity);
            mTracker = mAnalytics.newTracker(mTrackerID);
            mTracker.enableExceptionReporting(true);
            mTracker.enableAutoActivityTracking(true);
        }
    }
}
