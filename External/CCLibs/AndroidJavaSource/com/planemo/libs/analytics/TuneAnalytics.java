package com.planemo.libs.analytics;

import android.app.Activity;
import com.mobileapptracker.MATEvent;
import com.mobileapptracker.MobileAppTracker;
import com.planemo.libs.MultiplexerActivity;

/**
 * Created by Mike on 11/09/15.
 **/
@SuppressWarnings("unused")
public class TuneAnalytics {
    @SuppressWarnings("unused")
    private static final String TAG = TuneAnalytics.class.getName();
    private MobileAppTracker mobileAppTracker = null;
    private LifecycleTracker lifecycleTracker;

    private static TuneAnalytics sInstance = new TuneAnalytics();
    public static TuneAnalytics getInstance() {
        return sInstance;
    }

    public void start(String advertiserId, String conversionKey) {
        mobileAppTracker = MobileAppTracker.init(MultiplexerActivity.getInstance(), advertiserId, conversionKey);
        lifecycleTracker = new LifecycleTracker();
        MultiplexerActivity.getInstance().addLifecycleListener(lifecycleTracker);
    }

    public void setExistingUser() {
        mobileAppTracker.setExistingUser(true);
    }

    public void measureLevelAchieved(int levelNumber) {
        mobileAppTracker.measureEvent(new MATEvent(MATEvent.LEVEL_ACHIEVED).withLevel(levelNumber));
    }

    private class LifecycleTracker extends MultiplexerActivity.AbstractLifecycleListener {
        @Override
        public void onResume(Activity activity) {
            mobileAppTracker.setReferralSources(activity);
            mobileAppTracker.measureSession();
        }
    }
}
