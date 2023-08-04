package com.example.myapplication.opengl.surfaceview;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import com.example.myapplication.opengl.TigerRender;

public class TigerGlsurfaceView extends GLSurfaceView {
    public TigerGlsurfaceView(Context context) {
        super(context);
    }

    public TigerGlsurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        //设置EGL版本
        setEGLContextClientVersion(2);
        setRenderer(new TigerRender(this));
        //渲染模式，按需渲染
        setRenderMode(RENDERMODE_WHEN_DIRTY);

    }
}
