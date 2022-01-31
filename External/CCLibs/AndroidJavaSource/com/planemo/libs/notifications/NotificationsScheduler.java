package com.planemo.libs.notifications;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;

import com.planemo.libs.MultiplexerActivity;

import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.Calendar;
import java.util.concurrent.TimeUnit;

/**
 * Created by edward on 04/07/15.
 */

@SuppressWarnings("unused")
public class NotificationsScheduler {
    public static final String TAG = NotificationsScheduler.class.getName();
    public static final String KEY_MESSAGE = "notification_message";
    public static final String KEY_INFO = "notification_info";
    public static final String KEY_NOTIFICATION_ID = "notification_id";
    public static final String PREF_NAME = "local_pref";

    private static NotificationsScheduler mInstance;
    private NotificationsNativeListener mNativeListener;


    private NotificationsScheduler() {
        ActivityLifecycleListener lifecycleTracker = new ActivityLifecycleListener();
        MultiplexerActivity.getInstance().addLifecycleListener(lifecycleTracker);
        mNativeListener = new NotificationsNativeListener();
    }

    public static void init() {
        mInstance = new NotificationsScheduler();
    }

    public static NotificationsScheduler getInstance() {
        if (mInstance == null)
            mInstance = new NotificationsScheduler();
        return mInstance;
    }

    public void scheduleLocalNotification(int id, String message, int delayInSeconds, String infoStr) {
        Calendar calendar = Calendar.getInstance();
        long triggerTime = calendar.getTimeInMillis() + TimeUnit.SECONDS.toMillis(delayInSeconds);
        Intent intent = new Intent(Cocos2dxHelper.getActivity(), AlarmReceiver.class);
        intent.putExtra(KEY_MESSAGE, message);
        intent.putExtra(KEY_INFO, infoStr);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(Cocos2dxHelper.getActivity(), id, intent, PendingIntent.FLAG_UPDATE_CURRENT);
        AlarmManager alarmManager = (AlarmManager) Cocos2dxHelper.getActivity().getSystemService(Context.ALARM_SERVICE);
        alarmManager.set(AlarmManager.RTC, triggerTime, pendingIntent);
    }

    public void cancelLocalNotifications(int[] ids) {
        Context ctx = Cocos2dxHelper.getActivity();
        Intent intent = new Intent(ctx, AlarmReceiver.class);
        AlarmManager alarmManager = (AlarmManager) ctx.getSystemService(Context.ALARM_SERVICE);
        for (int codeRequest : ids) {
            PendingIntent pendingIntent = PendingIntent.getBroadcast(ctx, codeRequest, intent, 0);
            if (pendingIntent != null) {
                alarmManager.cancel(pendingIntent);
            }
        }
    }

    private class ActivityLifecycleListener extends MultiplexerActivity.AbstractLifecycleListener {
        @Override
        public void onStart(Activity activity) {
            super.onStart(activity);
            handleIntent(activity, activity.getIntent());
        }

        @Override
        public void onNewIntent(Activity activity, Intent intent) {
            handleIntent(activity, intent);
        }

        private void handleIntent(Activity activity, Intent intent) {
            Bundle extras = intent.getExtras();
            //data passed via intent
            //but that intent may be used few times in some circumstances
            //so we want to check that last used intent is not the same
            if (extras != null && extras.containsKey(KEY_INFO) && mNativeListener != null) {
                long notificationId = intent.getLongExtra(KEY_NOTIFICATION_ID, 0);
                String notificationInfo = extras.getString(KEY_INFO, "");
                SharedPreferences prefs = activity.getSharedPreferences(PREF_NAME, Context.MODE_PRIVATE);
                if (prefs.getLong(KEY_NOTIFICATION_ID, 0) != notificationId) {
                    mNativeListener.onReceiveNotification(notificationInfo);
                    prefs.edit().putLong(KEY_NOTIFICATION_ID, notificationId).apply();
                }
            }
        }
    }

    public static class Listener {
        public native void onReceiveNotification(String info);
    }

    public class NotificationsNativeListener {
        private Listener nativeListener = new Listener();

        public void onReceiveNotification(final String info) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onReceiveNotification(info);
                }
            });
        }
    }
}
