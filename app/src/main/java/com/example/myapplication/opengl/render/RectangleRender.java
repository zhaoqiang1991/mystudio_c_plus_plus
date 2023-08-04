package com.example.myapplication.opengl.render;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import com.example.myapplication.opengl.filter.RectangleFilter;
import com.example.myapplication.opengl.filter.TriangleFilter;
import com.example.myapplication.opengl.surfaceview.TigerPhotoGlsurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class RectangleRender implements GLSurfaceView.Renderer {
    private final TigerPhotoGlsurfaceView mGlsurfaceView;

    private RectangleFilter mRectangleFilter;
    private int originTexture;
    private int textureID;

    public RectangleRender(TigerPhotoGlsurfaceView glsurfaceView) {
        mGlsurfaceView = glsurfaceView;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        //保存在GPU的内存中，到时候通过UPDATE刷新到GPU里面
        mRectangleFilter = new RectangleFilter(mGlsurfaceView.getContext());

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mRectangleFilter.onReady(width, height);
    }

    //开始画画
    @Override
    public void onDrawFrame(GL10 gl) {
        //绘制之前清理屏幕
        GLES20.glClearColor(0, 0, 0, 0);
        //开始真正的清理
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        //交给滤镜可以画画
        mRectangleFilter.onDrawFrame(textureID);
    }


}
