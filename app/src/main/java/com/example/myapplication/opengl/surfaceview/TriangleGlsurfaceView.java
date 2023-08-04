package com.example.myapplication.opengl.surfaceview;

import android.content.Context;
import android.util.AttributeSet;

import com.example.myapplication.opengl.render.CannyRender;
import com.example.myapplication.opengl.render.TriangleRender;

public class TriangleGlsurfaceView extends TigerPhotoGlsurfaceView {
    public TriangleGlsurfaceView(Context context) {
        super(context);
    }

    public TriangleGlsurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected Renderer getRenderProvider() {
        return new TriangleRender(this);
    }
}
