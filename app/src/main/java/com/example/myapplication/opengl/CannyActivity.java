package com.example.myapplication.opengl;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import com.example.myapplication.R;
import com.example.myapplication.opengl.surfaceview.TigerPhotoGlsurfaceView;

public class CannyActivity extends AppCompatActivity {

    private TigerPhotoGlsurfaceView glsurreView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_canny);
        glsurreView = this.findViewById(R.id.tiger_canny_glsurfaceview);
    }

    @Override
    protected void onPause() {
        super.onPause();
        glsurreView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        glsurreView.onResume();
    }
}