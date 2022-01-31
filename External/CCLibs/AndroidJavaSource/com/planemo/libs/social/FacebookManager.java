package com.planemo.libs.social;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import com.facebook.*;
import com.facebook.appevents.AppEventsLogger;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.facebook.share.Sharer;
import com.facebook.share.model.AppInviteContent;
import com.facebook.share.model.ShareLinkContent;
import com.facebook.share.widget.AppInviteDialog;
import com.facebook.share.widget.ShareDialog;
import com.planemo.libs.MultiplexerActivity;
import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.ArrayList;
import java.util.List;


/**
 * Created by edward on 23/07/15.
 *
 */
@SuppressWarnings("unused")
public class FacebookManager {
    public static final String TAG = FacebookManager.class.getName();
    private static FacebookManager mInstance;
    private CallbackManager mCallbackManager;
    private FacebookNativeListener mNativeFacebookListener;
    private ShareDialog mShareDialog;


    private FacebookManager() {
        ActivityLifecycleListener lifecycleTracker = new ActivityLifecycleListener();
        MultiplexerActivity.getInstance().addLifecycleListener(lifecycleTracker);
        mNativeFacebookListener = new FacebookNativeListener();
    }


    public static void init() {
        Log.v(TAG, "init");
        mInstance = new FacebookManager();
    }

    public static FacebookManager getInstance() {
        if (mInstance == null)
            mInstance = new FacebookManager();
        return mInstance;
    }

    public void loginWithReadPermissions(List<String> permissions) {
        Log.v(TAG, "start login");
        //TODO test
        LoginManager.getInstance().logInWithReadPermissions(Cocos2dxHelper.getActivity(), permissions); //Arrays.asList("public_profile", "user_friends")
    }

    public void logout() {
        Log.v(TAG, "logout");
        //TODO test
        LoginManager.getInstance().logOut();
    }

    public boolean isLoggedIn() {
        Log.v(TAG, "isLoggedIn");
        //TODO test
        return AccessToken.getCurrentAccessToken() != null;
    }


    public Object[] getUserInfo() {
        //TODO test
        Log.v(TAG, "getUserInfo");
        List<String> list = new ArrayList<>();
        Profile profile = Profile.getCurrentProfile();
        if(profile != null){
            list.add(profile.getName());
            list.add(profile.getFirstName());
            list.add(profile.getLastName());
            list.add(profile.getProfilePictureUri(300, 300).toString());
        }
        return list.toArray();
    }

    public void shareLink(String url, String title, String imageUrl, String contentDescription) {
        if (ShareDialog.canShow(ShareLinkContent.class)) {
            ShareLinkContent.Builder  builder = new ShareLinkContent.Builder();
            if(url != null)
                builder.setContentUrl(Uri.parse(url));
            if(title != null)
                builder.setContentTitle(title);
            //fix Facebook error Image Url must be an http:// or https:// url
            if(imageUrl != null && imageUrl.length() > 0 && imageUrl.startsWith("http"))
                builder.setImageUrl(Uri.parse(imageUrl));
            if(contentDescription != null)
                builder.setContentDescription(contentDescription);
            mShareDialog.show(builder.build());
        }
    }

    public boolean canInviteFriends() {
        return  AppInviteDialog.canShow();
    }

    public void inviteFriends(String appLinkUrl) {
        AppInviteContent content = new AppInviteContent.Builder()
                .setApplinkUrl(appLinkUrl)
                .build();
        AppInviteDialog dialog = new AppInviteDialog(Cocos2dxHelper.getActivity());

        dialog.registerCallback(mCallbackManager,
                new FacebookCallback<AppInviteDialog.Result>() {
                    @Override
                    public void onSuccess(AppInviteDialog.Result result) {
                        if (result != null) {
                            mNativeFacebookListener.onInviteCallback(true);
                        }
                    }

                    @Override
                    public void onCancel() {
                        mNativeFacebookListener.onInviteCallback(false);
                    }

                    @Override
                    public void onError(FacebookException e) {
                        mNativeFacebookListener.onInviteCallback(false);
                    }
                });


        if (dialog.canShow()) {
            dialog.show(Cocos2dxHelper.getActivity(), content);
        }
    }

    private class ActivityLifecycleListener extends MultiplexerActivity.AbstractLifecycleListener {

        public ActivityLifecycleListener() {

        }

        @Override
        public void onCreate(Activity activity, Bundle savedInstanceState) {
            // Initialize the SDK before executing any other operations,
            // especially, if you're using Facebook UI elements.
            Log.v(TAG, "FacebookSdk.sdkInitialize");
            FacebookSdk.sdkInitialize(activity);

            mCallbackManager = CallbackManager.Factory.create();
            LoginManager.getInstance().registerCallback(mCallbackManager,
                    new FacebookCallback<LoginResult>() {
                        @Override
                        public void onSuccess(LoginResult loginResult) {
                            Log.v(TAG, "LoginManager onSuccess " + loginResult);
                            if(loginResult != null && loginResult.getRecentlyGrantedPermissions() != null) {
                                mNativeFacebookListener.onLoginCallbackSuccess(loginResult.getRecentlyGrantedPermissions().toArray());
                            }
                        }

                        @Override
                        public void onCancel() {
                            Log.v(TAG, "LoginManager onCancel ");
                            mNativeFacebookListener.onLoginCallbackCancellation();
                        }

                        @Override
                        public void onError(FacebookException exception) {
                            Log.v(TAG, "LoginManager onError " + exception.getMessage());
                            mNativeFacebookListener.onLoginCallbackFailure(exception.getMessage());
                        }
                    });

            mShareDialog = new ShareDialog(activity);
            // this part is optional
            mShareDialog.registerCallback(mCallbackManager, new FacebookCallback<Sharer.Result>() {
                @Override
                public void onSuccess(Sharer.Result result) {
                    Log.v(TAG, "Sharer.Result onSuccess " + result);
                    mNativeFacebookListener.onShareCompletionCallback(true);
                }

                @Override
                public void onCancel() {
                    Log.v(TAG, "Sharer.Result onCancel ");
                    mNativeFacebookListener.onShareCompletionCallback(false);
                }

                @Override
                public void onError(FacebookException error) {
                    Log.v(TAG, "Sharer.Result error " + error);
                    mNativeFacebookListener.onShareCompletionCallback(false);
                }
            });
        }

        @Override
        public void onResume(Activity activity) {
            //For app installs with facebook ads. getApplicationId  should only be called after sdkInitialize
            AppEventsLogger.activateApp(activity,  FacebookSdk.getApplicationId());
        }

        @Override
        public boolean onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {
            if (mCallbackManager != null)
                return mCallbackManager.onActivityResult(requestCode, resultCode, data);
            return false;
        }
    }


    public static class Listener {
        public native void onInviteCallback(boolean isSuccess);
        public native void onShareCompletionCallback(boolean isSuccess);
        public native void onLoginCallbackFailure(String error);
        public native void onLoginCallbackCancellation();
        public native void onLoginCallbackSuccess(Object[] grantedPermissions);
        public native void onGetUserInfo(String name, String firstName, String lastName, String imageUrl);
    }

    public class FacebookNativeListener {
        private Listener nativeListener = new Listener();

        public void onInviteCallback(final boolean isSuccess) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onInviteCallback(isSuccess);
                }
            });
        }

        public void onShareCompletionCallback(final boolean isSuccess) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onShareCompletionCallback(isSuccess);
                }
            });
        }

        public void onLoginCallbackFailure(final String error) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onLoginCallbackFailure(error);
                }
            });
        }

        public void onLoginCallbackCancellation() {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onLoginCallbackCancellation();
                }
            });
        }

        public void onLoginCallbackSuccess(final Object[] grantedPermissions) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onLoginCallbackSuccess(grantedPermissions);
                }
            });
        }

        public void onGetUserInfo(final String name, final String firstName, final String lastName, final String imageUrl) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onGetUserInfo(name, firstName, lastName, imageUrl);
                }
            });
        }
    }
}
