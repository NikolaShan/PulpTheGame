package com.planemo.libs.support;

import java.util.HashMap;
import java.util.Map;
import java.util.Random;

@SuppressWarnings("unused")
public class ObjectKeeper {
    private static Map<Long, Object> sObjects = new HashMap<>();

    static public long keepObject(Object o) {
        Random rand = new Random();
        while (true) {
            long key = rand.nextLong();
            if (!sObjects.containsKey(key)) {
                sObjects.put(key, o);
                return key;
            }
        }
    }

    public static void releaseObject(long uid) {
        sObjects.remove(uid);
    }

    public static Object getObject(long uid) {
        return sObjects.get(uid);
    }
}
