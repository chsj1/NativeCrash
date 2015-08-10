
package com.example.nativecrash;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Test.initial();

        new Handler().postDelayed(new Runnable() {
            
            @Override
            public void run() {
                new Test().crash();
            }
        }, 1*1000);
    }
}
