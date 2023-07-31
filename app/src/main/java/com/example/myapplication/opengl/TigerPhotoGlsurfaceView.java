package com.example.myapplication.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import com.example.myapplication.opengl.filter.LUTRender;

public class TigerPhotoGlsurfaceView extends GLSurfaceView {
    public TigerPhotoGlsurfaceView(Context context) {
        super(context);
    }

    public TigerPhotoGlsurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        //设置EGL版本
        setEGLContextClientVersion(2);
        //setRenderer(new LUTRender(context));
        setRenderer(new PhotoRender(this));
        //PhotoRender
        //渲染模式，按需渲染
        setRenderMode(RENDERMODE_WHEN_DIRTY);

    }
}
