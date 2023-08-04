package com.example.myapplication.opengl;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

import com.example.myapplication.R;
import com.example.myapplication.opengl.surfaceview.TigerPhotoGlsurfaceView;

public class TriangleActivity extends AppCompatActivity {
    private TigerPhotoGlsurfaceView glsurreView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_triangle);
        glsurreView = this.findViewById(R.id.tiger_triangle_glsurfaceview);
    }
}