package com.example.toms.camera_app;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.content.Intent;
import android.util.Log;
import android.view.View;

public class Main extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    private static final String TAG = "Main";

    public void nextPage(View view) {
        Log.d(TAG, "Switching to next page");
        Intent intent = new Intent(this, Buttons_camera.class);
        startActivity(intent);
    }

}