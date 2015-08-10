
package com.example.nativecrash;

import android.util.Log;

public class Test {
    static {
        try {
            System.loadLibrary("breakpad_client");
        } catch (Error e) {
            e.printStackTrace();
        }
    }

    public native static void initial();

    public native void crash();

    public static void onNativeCrash(String crashInfo) {
        Log.d("tuhuolong", crashInfo + " " + Log.getStackTraceString(new Throwable()));
    }

}
