package com.planemo.libs.social;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import com.planemo.libs.MultiplexerActivity;
import com.vk.sdk.*;
import com.vk.sdk.api.*;
import com.vk.sdk.api.model.VKApiUser;
import com.vk.sdk.api.model.VKList;
import com.vk.sdk.dialogs.VKShareDialog;

import org.cocos2dx.lib.Cocos2dxHelper;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;


/**
 * Created by edward on 24/07/15.
 */
public class VkManager {
    public static final String TAG = VkManager.class.getName();
    private static VkManager mInstance;
    private VkNativeListener mVkNativeListener;
    private boolean mShowShareDialog;
    private String mShareText;
    private String mShareLinkTitle;
    private String mShareLink;


    private static final String[] defScopePermission = new String[]{
            VKScope.WALL
    };

    private VkManager() {
        ActivityLifecycleListener lifecycleTracker = new ActivityLifecycleListener();
        MultiplexerActivity.getInstance().addLifecycleListener(lifecycleTracker);
        mVkNativeListener = new VkNativeListener();
    }

    public static void init() {
        Log.v(TAG, "init");
        mInstance = new VkManager();
    }

    public static VkManager getInstance() {
        if (mInstance == null)
            mInstance = new VkManager();
        return mInstance;
    }


    public void loginWithPermissions(List<String> permissions) {
        //TODO test
        if (permissions != null) {
            String[] permissionsArray = new String[permissions.size()];
            permissions.toArray(permissionsArray);
            VKSdk.login(Cocos2dxHelper.getActivity(), permissionsArray);
        }
    }

    public void logout() {
        //TODO test
        VKSdk.logout();
    }

    public boolean isLoggedIn() {
        //TODO test
        return VKSdk.isLoggedIn();
    }


    public boolean isVkCountry() {
        //TODO test
        String l = Locale.getDefault().getLanguage().toLowerCase();
        String[] countries = {"ru", "kz", "az", "ab", "al", "am", "by", "bg", "ge", "lt", "ua", "md", "tj", "ee"};
        for (String str : countries) {
            if (l.equals(str))
                return true;
        }
        return false;
    }

    public void requestUserInfo() {
        //TODO test
        Log.v(TAG, "getUserInfo");
        final List<String> list = new ArrayList<>();
        final VKRequest request = VKApi.users().get(VKParameters.from(VKApiConst.FIELDS, "photo_100"));
        request.executeWithListener(new VKRequest.VKRequestListener() {
            @Override
            public void onComplete(VKResponse response) {
                VKApiUser user = ((VKList<VKApiUser>) response.parsedModel).get(0);
                if (user != null) {
                    list.add(user.first_name);
                    list.add(user.last_name);
                    list.add(user.photo_100);
                    String email = VKSdk.getAccessToken().email;
                    mVkNativeListener.onUserInfoCallback(user.first_name, user.last_name, user.photo_100, email != null ? email : "");
                }
            }
        });
    }


    public void share(String text, String linkTitle, String link) {
        if (isLoggedIn()) {
            new VKShareDialog()
                    .setText(text)
                    .setAttachmentLink(linkTitle, link)
                    .setShareDialogListener(new VKShareDialog.VKShareDialogListener() {
                        @Override
                        public void onVkShareComplete(int postId) {
                            mVkNativeListener.onShareCompletionCallback(true);
                        }

                        @Override
                        public void onVkShareCancel() {
                            mVkNativeListener.onShareCompletionCallback(false);
                        }
                    }).show(Cocos2dxHelper.getActivity().getFragmentManager(), "DIALOG_SHARE");
        } else {
            mShareText = text;
            mShareLinkTitle = linkTitle;
            mShareLink = link;
            mShowShareDialog = true;
            defaultLogin();
        }

    }

    private void defaultLogin() {
        VKSdk.login(Cocos2dxHelper.getActivity(), defScopePermission);
    }

    private class ActivityLifecycleListener extends MultiplexerActivity.AbstractLifecycleListener {

        public ActivityLifecycleListener() {

        }

        @Override
        public void onCreate(Activity activity, Bundle savedInstanceState) {
            VKSdk.initialize(activity);
            VKUIHelper.setApplicationContext(activity);
        }

        @Override
        public boolean onActivityResult(Activity activity, int requestCode, int resultCode, Intent data) {
            VKSdk.onActivityResult(requestCode, resultCode, data, new VKCallback<VKAccessToken>() {
                @Override
                public void onResult(VKAccessToken res) {
                    //Succes TODO tempEmptyArray?
                    Object[] tempEmptyArray = new Object[0];
                    mVkNativeListener.onLoginCallbackSuccess(tempEmptyArray);
                    if (mShowShareDialog && mShareLinkTitle != null && mShareLink != null && mShareText != null) {
                        share(mShareText, mShareLinkTitle, mShareLink);
                        mShowShareDialog = false;
                    }
                }

                @Override
                public void onError(VKError error) {
                    if (error.errorCode == VKError.VK_CANCELED) {
                        mVkNativeListener.onLoginCallbackCancellation();
                    } else {
                        mVkNativeListener.onLoginCallbackFailure(error.errorMessage);
                    }
                }
            });
            return false;
        }
    }


    public static class Listener {
        public native void onUserInfoCallback(String firstName, String lastName, String avatarUrl, String email);

        public native void onShareCompletionCallback(boolean isSuccess);

        public native void onLoginCallbackFailure(String error);

        public native void onLoginCallbackSuccess(Object[] array);

        public native void onLoginCallbackCancellation();
    }

    public class VkNativeListener {
        private Listener nativeListener = new Listener();

        public void onUserInfoCallback(final String firstName, final String lastName, final String avatarUrl, final String email) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onUserInfoCallback(firstName, lastName, avatarUrl, email);
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

        public void onLoginCallbackSuccess(final Object[] grantedPermissions) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onLoginCallbackSuccess(grantedPermissions);
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

        public void onShareCompletionCallback(final boolean isSuccess) {
            Cocos2dxHelper.runOnGLThread(new Runnable() {
                @Override
                public void run() {
                    nativeListener.onShareCompletionCallback(isSuccess);
                }
            });
        }
    }
}