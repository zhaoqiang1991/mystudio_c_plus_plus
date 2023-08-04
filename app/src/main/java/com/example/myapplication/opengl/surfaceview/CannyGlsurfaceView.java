package com.example.myapplication.opengl.surfaceview;

import android.content.Context;
import android.util.AttributeSet;

import com.example.myapplication.opengl.render.CannyRender;

public class CannyGlsurfaceView extends TigerPhotoGlsurfaceView {
    public CannyGlsurfaceView(Context context) {
        super(context);
    }

    public CannyGlsurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected Renderer getRenderProvider() {
        return new CannyRender(this);
    }
}
