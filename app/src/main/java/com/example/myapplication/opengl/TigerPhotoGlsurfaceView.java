package com.example.myapplication.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

import com.example.myapplication.opengl.render.PhotoRender;

public class TigerPhotoGlsurfaceView extends GLSurfaceView {
    public TigerPhotoGlsurfaceView(Context context) {
        super(context);
    }

    public TigerPhotoGlsurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
        //设置EGL版本
        setEGLContextClientVersion(2);
        setRenderer(getRenderProvider());
       // setRenderer(new BaseSampleRender(context));
        //PhotoRender
        //渲染模式，按需渲染
        setRenderMode(RENDERMODE_WHEN_DIRTY);

    }

    protected Renderer getRenderProvider() {
        return new PhotoRender(this);
    }
}
