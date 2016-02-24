package com.example.toms.camera_app;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class Buttons_camera extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_buttons_camera);
    }

    private static final String TAG = "CameraButtons";

    public void up(View view) {
        Log.d(TAG, "Move up");
    }

    public void down(View view) {
        Log.d(TAG, "Move down");
    }

    public void right(View view) {
        Log.d(TAG, "Move right");
    }

    public void left(View view) {
        Log.d(TAG, "Move left");
    }
}
