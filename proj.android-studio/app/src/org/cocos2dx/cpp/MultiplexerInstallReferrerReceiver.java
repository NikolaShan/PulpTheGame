package org.cocos2dx.cpp;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.planemo.libs.support.AppUtils;

/**
 * Created by Mike on 13.09.15.
 **/
public class MultiplexerInstallReferrerReceiver extends BroadcastReceiver {
    @SuppressWarnings("unused")
    private static final String TAG = MultiplexerInstallReferrerReceiver.class.getName();

    @Override
    public void onReceive(Context context, Intent intent) {
        String referrer = intent.getExtras().getString("referrer", null);
        if (referrer != null) {
            AppUtils.setReferrer(context, referrer.toLowerCase());
        }

        com.mobileapptracker.Tracker tuneReceiver = new com.mobileapptracker.Tracker();
        tuneReceiver.onReceive(context, intent);
    }
}