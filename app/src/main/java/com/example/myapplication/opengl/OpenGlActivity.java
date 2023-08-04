package com.example.myapplication.opengl;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import com.example.myapplication.R;
import com.example.myapplication.opengl.surfaceview.TigerGlsurfaceView;

public class OpenGlActivity extends AppCompatActivity {
    private TigerGlsurfaceView glSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_open_gl);
        glSurfaceView = this.findViewById(R.id.tiger_surfaceview);
    }
}