package com.planemo.libs.support;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import com.planemo.libs.MultiplexerActivity;
import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

@SuppressWarnings("unused")
public class AppUtils {
    @SuppressWarnings("unused")
    private static final String TAG = AppUtils.class.getName();
    private static final String PREFS_NAME = "UtilsPrefs";

    public static String getPackageName() {
        return Cocos2dxHelper.getCocos2dxPackageName();
    }

    public static boolean isAppInstalled(String packageName) {
        PackageManager pm = MultiplexerActivity.getInstance().getPackageManager();
        boolean app_installed;
        try {
            pm.getPackageInfo(packageName, PackageManager.GET_ACTIVITIES);
            app_installed = true;
        } catch (PackageManager.NameNotFoundException e) {
            app_installed = false;
        }
        return app_installed;
    }

    public static String getPreferredLanguage() {
        return Locale.getDefault().getLanguage().toLowerCase();
    }

    public static String getVersionName() {
        try {
            return MultiplexerActivity.getInstance().getPackageManager().getPackageInfo(getPackageName(), 0).versionName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            return "1.0";
        }
    }

    public static boolean isTablet() {
        DisplayMetrics metrics = new DisplayMetrics();
        MultiplexerActivity.getInstance().getWindowManager().getDefaultDisplay().getMetrics(metrics);

        float yInches = metrics.heightPixels / metrics.ydpi;
        float xInches = metrics.widthPixels / metrics.xdpi;
        double diagonalInches = Math.sqrt(xInches * xInches + yInches * yInches);
        return diagonalInches >= 6.5;
    }

    public static int[] getScreenSizeInDp() {
        Display display = MultiplexerActivity.getInstance().getWindowManager().getDefaultDisplay();
        DisplayMetrics outMetrics = new DisplayMetrics();
        display.getMetrics(outMetrics);
        float density = MultiplexerActivity.getInstance().getResources().getDisplayMetrics().density;
        float dpHeight = outMetrics.heightPixels / density;
        float dpWidth = outMetrics.widthPixels / density;
        return new int[]{(int) dpWidth, (int) dpHeight};
    }

    public static int getBuildNumber() {
        try {
            return MultiplexerActivity.getInstance().getPackageManager().getPackageInfo(getPackageName(), 0).versionCode;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            return 1;
        }
    }

    private static final String REFERRER_KEY = "referrer";
    public static void setReferrer(Context context, String referrer) {
        if (referrer != null) {
            SharedPreferences prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);
            prefs.edit().putString(REFERRER_KEY, referrer).commit();
        }
    }

    public static String getReferrer() {
        return MultiplexerActivity.getInstance()
                .getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
                .getString(REFERRER_KEY, "");
    }


}
