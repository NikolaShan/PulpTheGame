package com.planemo.libs.notifications;

/**
 * Created by edward on 05/07/15.
 *
 */

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import com.nexxstudio.dulp.R;

import org.cocos2dx.cpp.AppActivity;

import java.util.Random;


public class AlarmReceiver extends BroadcastReceiver {
    private boolean isReceive;

    @Override
    public void onReceive(Context context, Intent intent) {
        try {
            if (!isReceive) { //onReceive called twice
                String message = intent.getStringExtra(NotificationsScheduler.KEY_MESSAGE);
                String info = intent.getStringExtra(NotificationsScheduler.KEY_INFO);
                sendNotification(context.getApplicationContext(), message, info);
                isReceive = true;
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }


    private void sendNotification(Context ctx, String message, String info) {
        Intent intent = new Intent(ctx, AppActivity.class);
        intent.putExtra(NotificationsScheduler.KEY_INFO, info);
        intent.putExtra(NotificationsScheduler.KEY_NOTIFICATION_ID, new Random().nextLong());
        PendingIntent pIntent = PendingIntent.getActivity(ctx, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);

        Notification.Builder builder = new Notification.Builder(ctx);
        builder.setContentIntent(pIntent);
        builder.setSmallIcon(R.mipmap.notification_icon);
        builder.setContentText(ctx.getString(R.string.app_name));
        builder.setContentTitle(message); //Title font is bigger than message font
        builder.setWhen(System.currentTimeMillis());
        Notification notification = builder.getNotification();
        notification.flags |= Notification.FLAG_AUTO_CANCEL;
        NotificationManager nm = (NotificationManager) ctx.getSystemService(Context.NOTIFICATION_SERVICE);
        nm.notify(1, notification);
    }
}