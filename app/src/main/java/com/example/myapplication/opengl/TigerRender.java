package com.example.myapplication.opengl;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import com.example.myapplication.opengl.filter.ScreeFilter;
import com.example.myapplication.opengl.surfaceview.TigerGlsurfaceView;
import com.example.myapplication.opengl.utils.CameraHelper;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class TigerRender implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {
    private float[] mtx = new float[16];
    private final TigerGlsurfaceView mGlsurfaceView;
    private CameraHelper mCameraHelper;
    //纹理
    private int[] mTextures;
    private SurfaceTexture mSurfaceTexture;
    private ScreeFilter mScreeFilter;

    public TigerRender(TigerGlsurfaceView glsurfaceView) {
        mGlsurfaceView = glsurfaceView;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        mCameraHelper = new CameraHelper(Camera.CameraInfo.CAMERA_FACING_FRONT);
        //保存在GPU的内存中，到时候通过UPDATE刷新到GPU里面
        mTextures = new int[1];
        //通过给定的纹理ID，创建一个纹理
        GLES20.glGenTextures(mTextures.length, mTextures, 0);
        mSurfaceTexture = new SurfaceTexture(mTextures[0]);
        mSurfaceTexture.setOnFrameAvailableListener(this);
        mScreeFilter = new ScreeFilter(mGlsurfaceView.getContext());

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mCameraHelper.startPreview(mSurfaceTexture);
        mScreeFilter.onReady(width,height);

    }

    //开始画画
    @Override
    public void onDrawFrame(GL10 gl) {
        //绘制之前清理屏幕
        GLES20.glClearColor(0,0,0,0);
        //开始真正的清理
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        //把纹理中的数据更新到GPU里面
        mSurfaceTexture.updateTexImage();
        mSurfaceTexture.getTransformMatrix(mtx);

        //交给滤镜可以画画
        mScreeFilter.onDrawFrame(mTextures[0],mtx);
    }

    /**
     * 摄像头采集的数据交给了surfacetexture,当摄像头有一帧数据回来的手就会调调到之类，     每一帧数据回调回来调用
     *
     * @param surfaceTexture
     */
    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
           mGlsurfaceView.requestRender();
    }
}
