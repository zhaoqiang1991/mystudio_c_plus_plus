package com.example.myapplication.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class TigerView extends GLSurfaceView {
    public TigerView(Context context) {
        super(context);
    }

    public TigerView(Context context, AttributeSet attrs) {
        super(context, attrs);
        /**
         * 设置egl版本
         */
        setEGLContextClientVersion(2);
        /**
         * 设置渲染器
         */
        setRenderer(new TigerRender(this));

        /**
         * 设置按需渲染，当我们调用requestRender()的时候就会调用GlThread回调一次onDrawFrame()
         */
        setRenderMode(RENDERMODE_WHEN_DIRTY);

    }
}
