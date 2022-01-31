package com.planemo.libs;


import android.app.Activity;
import android.support.annotation.NonNull;
import android.util.Log;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.remoteconfig.FirebaseRemoteConfig;
import com.google.firebase.remoteconfig.FirebaseRemoteConfigSettings;
import com.nexxstudio.dulp.BuildConfig;

import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class RemoteConfig {
    private FirebaseRemoteConfig mFirebaseRemoteConfig = FirebaseRemoteConfig.getInstance();
    private static RemoteConfig sInstance = null;

    public static RemoteConfig getInstance() {
        if (sInstance == null) {
            sInstance = new RemoteConfig();
        }
        return sInstance;
    }

    private RemoteConfig() {
        FirebaseRemoteConfigSettings configSettings = new FirebaseRemoteConfigSettings.Builder()
               // .setDeveloperModeEnabled(BuildConfig.DEBUG)
                .build();
        mFirebaseRemoteConfig.setConfigSettings(configSettings);
    }

    public void fetch(float expirationInSeconds) {
        mFirebaseRemoteConfig.fetch()
                .addOnCompleteListener(new OnCompleteListener<Void>() {
                    @Override
                    public void onComplete(@NonNull Task<Void> task) {
                        if (task.isSuccessful()) {
                            // After config data is successfully fetched, it must be activated before newly fetched
                            // values are returned.
                            mFirebaseRemoteConfig.activateFetched();
                        } else {
                        }
                    }
                });
    }

    public void setDefaults(String json) {
        Map<String, Object> defaults = ValueJsonToMapConverter.convert(json);
        mFirebaseRemoteConfig.setDefaults(defaults);
    }

    public String getStringValue(String key) {
        return mFirebaseRemoteConfig.getString(key);
    }

    public float getFloatValue(String key) {
        return (float) mFirebaseRemoteConfig.getDouble(key);
    }

    public int getIntValue(String key) {
        return (int) mFirebaseRemoteConfig.getLong(key);
    }

    public boolean getBoolValue(String key) {
        return mFirebaseRemoteConfig.getBoolean(key);
    }

    private static class ValueJsonToMapConverter {
        static Map<String, Object> convert(String valueJson) {
            Map<String, Object> ret = new HashMap<>();
            try {
                JSONObject obj = new JSONObject(valueJson);
                Iterator<String> keys = obj.keys();
                while( keys.hasNext() ) {
                    String key = keys.next();
                    Object jval = obj.get(key);
                    Object val = convert(jval);
                    ret.put(key, val);
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
            return ret;
        }

        static Object convert(Object jval) {
            if (jval instanceof JSONArray) {
                throw new RuntimeException("There should no be array types");
            } else if (jval instanceof JSONObject) {
                throw new RuntimeException("There should no be object types");
            } else {
                return jval;
            }
        }
    }

}
