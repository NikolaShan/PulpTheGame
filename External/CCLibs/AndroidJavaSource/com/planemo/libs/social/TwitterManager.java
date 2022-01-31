package com.planemo.libs.social;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.net.Uri;

import com.planemo.libs.MultiplexerActivity;
import com.planemo.libs.support.ShareManager;

import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.List;
import java.util.Map;


public class TwitterManager {
    public static final String TAG = TwitterManager.class.getName();
    private static TwitterManager mInstance;




    public static TwitterManager getInstance() {
        if (mInstance == null) {
            mInstance = new TwitterManager();
        }
        return mInstance;
    }

    public void tweet(Map<String, String> tweetData) {

        if(tweetData != null) {
            String message = tweetData.get("text") + " " + tweetData.get("url");
            String imagePath = tweetData.get("imagePath");

            Intent tweetIntent = new Intent(Intent.ACTION_SEND);
            tweetIntent.setType("image/*");
            tweetIntent.putExtra(Intent.EXTRA_TEXT, message);
            tweetIntent.putExtra(Intent.EXTRA_STREAM, ShareManager.getUriWithImageForIntentSharing(imagePath));

            PackageManager packManager = MultiplexerActivity.getInstance().getPackageManager();
            List<ResolveInfo> resolvedInfoList = packManager.queryIntentActivities(tweetIntent, PackageManager.MATCH_DEFAULT_ONLY);

            boolean resolved = false;
            for (ResolveInfo resolveInfo : resolvedInfoList) {
                if (resolveInfo.activityInfo.packageName.startsWith("com.twitter.android")) {
                    tweetIntent.setClassName(
                            resolveInfo.activityInfo.packageName,
                            resolveInfo.activityInfo.name);
                    resolved = true;
                    break;
                }
            }
            if (resolved) {
                MultiplexerActivity.getInstance().startActivity(tweetIntent);
            } else {
                Intent i = new Intent();
                i.putExtra(Intent.EXTRA_TEXT, message);
                i.setAction(Intent.ACTION_VIEW);
                i.setData(Uri.parse("https://twitter.com/intent/tweet?text=message"));
                MultiplexerActivity.getInstance().startActivity(i);
            }
        }
    }

    public static class Listener {
        public native void onTweetResult(boolean success);
    }

    public class TwitterNativeListener {
        private Listener nativeListener = new Listener();

        public void onTweetResult(final boolean success) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onTweetResult(success);
                }
            });
        }
    }
}