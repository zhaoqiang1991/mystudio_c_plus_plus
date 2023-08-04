package com.example.myapplication.opengl.surfaceview;

import android.content.Context;
import android.util.AttributeSet;

import com.example.myapplication.opengl.render.RectangleRender;
import com.example.myapplication.opengl.render.TriangleRender;

public class RectangleGlsurfaceView extends TigerPhotoGlsurfaceView {
    public RectangleGlsurfaceView(Context context) {
        super(context);
    }

    public RectangleGlsurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected Renderer getRenderProvider() {
        return new RectangleRender(this);
    }
}
