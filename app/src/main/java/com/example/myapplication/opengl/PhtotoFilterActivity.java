package com.example.myapplication.opengl;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;

import com.example.myapplication.R;

public class PhtotoFilterActivity extends AppCompatActivity {

    private TigerPhotoGlsurfaceView glsurreView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_phtoto_filter);
        glsurreView = this.findViewById(R.id.tiger_photo_glsurfaceview);
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