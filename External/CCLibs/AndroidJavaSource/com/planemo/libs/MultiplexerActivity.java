package com.planemo.libs;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

import android.util.AttributeSet;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.FrameLayout;
import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.ArrayList;
import java.util.List;

public class MultiplexerActivity extends Cocos2dxActivity {
    private static final String TAG = MultiplexerActivity.class.getName();

    public static MultiplexerActivity getInstance() {
        return (MultiplexerActivity) Cocos2dxHelper.getActivity();
    }

    public interface LifecycleListener {
        void onCreate(Activity activity, Bundle savedInstanceState);

        void onStart(Activity activity);

        void onStop(Activity activity);

        void onResume(Activity activity);

        void onPause(Activity activity);

        void onDestroy(Activity activity);

        boolean onBackPressed(Activity activity);

        boolean onActivityResult(Activity activity, int requestCode, int resultCode, Intent data);

        void onNewIntent(Activity activity, Intent intent);
    }

    public static abstract class AbstractLifecycleListener implements LifecycleListener {
        @Override
        public void onCreate(Activity activity, Bundle savedInstanceState) {
        }

        @Override
        public void onStart(Activity activity) {
        }

        @Override
        public void onStop(Activity activity) {
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

        @Override
        public void onNewIntent(Activity activity, Intent intent) {

        }
    }

    private List<LifecycleListener> mLifecycleListeners = new ArrayList<>();

    public void addLifecycleListener(LifecycleListener listener) {
        mLifecycleListeners.add(listener);
    }

    public void removeLifecycleListener(LifecycleListener listener) {
        mLifecycleListeners.remove(listener);
    }

    public native void onCreate();

    @Override
    public View onCreateView(String name, Context context, AttributeSet attrs) {
        Log.v(TAG, "OnCreateView");
        return super.onCreateView(name, context, attrs);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.v(TAG, "OnCreate");

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        this.onCreate();
        for (LifecycleListener listener : mLifecycleListeners) {
            listener.onCreate(this, savedInstanceState);
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        for (LifecycleListener listener : mLifecycleListeners) {
            listener.onStart(this);
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        for (LifecycleListener listener : mLifecycleListeners) {
            listener.onStop(this);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        for (LifecycleListener listener : mLifecycleListeners) {
            listener.onPause(this);
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        for (LifecycleListener listener : mLifecycleListeners) {
            listener.onResume(this);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        for (LifecycleListener listener : mLifecycleListeners) {
            if (listener.onActivityResult(this, requestCode, resultCode, data)) {
                return;
            }
        }
    }

    @Override
    public void onBackPressed() {
        for (LifecycleListener listener : mLifecycleListeners) {
            if (listener.onBackPressed(this)) {
                return;
            }
        }
        super.onBackPressed();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        for (LifecycleListener listener : mLifecycleListeners) {
            listener.onDestroy(this);
        }
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        for (LifecycleListener listener : mLifecycleListeners) {
            listener.onNewIntent(this, intent);
        }
    }

    public FrameLayout getRootLayout() {
        return mFrameLayout;
    }
}
