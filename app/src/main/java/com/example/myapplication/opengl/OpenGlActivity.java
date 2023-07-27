package com.example.myapplication.opengl;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;

import com.example.myapplication.R;

public class OpenGlActivity extends AppCompatActivity {
    private TigerGlsurfaceView glSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_open_gl);
        glSurfaceView = this.findViewById(R.id.tiger_surfaceview);
    }
}