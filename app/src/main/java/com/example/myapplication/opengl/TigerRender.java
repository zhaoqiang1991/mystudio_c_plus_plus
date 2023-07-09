package com.example.myapplication.opengl;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import com.example.myapplication.filter.CameraFilter;
import com.example.myapplication.filter.ScreeFilter;
import com.example.myapplication.utils.CameraHelper;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class TigerRender implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener {
    private final TigerView mView;
    private CameraHelper mCameraHelper;
    private SurfaceTexture mSurfaceTexture;
    float[] mtx = new float[16];
    private ScreeFilter mScreeFilter;
    private int[] mTextures;
    private CameraFilter mCameraFilter;

    public TigerRender(TigerView tigerView) {
        mView = tigerView;
    }

    /**
     * 画布创建好了
     *
     * @param gl     the GL interface. Use <code>instanceof</code> to
     *               test if the interface supports GL11 or higher interfaces.
     * @param config the EGLConfig of the created surface. Can be used
     *               to create matching pbuffers.
     */
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        mCameraHelper = new CameraHelper(Camera.CameraInfo.CAMERA_FACING_BACK);
        //准备好摄像头绘制的画布
        //通过open gl创建一个纹理id
        mTextures = new int[1];
        GLES20.glGenTextures(mTextures.length, mTextures, 0);
        mSurfaceTexture = new SurfaceTexture(mTextures[0]);
        //设置有一帧新的数据到来的时候，回调监听
        mSurfaceTexture.setOnFrameAvailableListener(this);
        //必须要在GlThread里面创建着色器程序
        mCameraFilter = new CameraFilter(mView.getContext());
        mScreeFilter = new ScreeFilter(mView.getContext());
    }

    /**
     * 画布发生改变
     *
     * @param gl     the GL interface. Use <code>instanceof</code> to
     *               test if the interface supports GL11 or higher interfaces.
     * @param width
     * @param height
     */
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mCameraHelper.startPreview(mSurfaceTexture);
        mCameraFilter.onReady(width, height);
        mScreeFilter.onReady(width, height);
    }

    /**
     * 画画
     *
     * @param gl the GL interface. Use <code>instanceof</code> to
     *           test if the interface supports GL11 or higher interfaces.
     */
    @Override
    public void onDrawFrame(GL10 gl) {
        //告诉open gl需要把屏幕清理成 什么样子的颜色
        GLES20.glClearColor(0, 0, 0, 0);
        //开始真正的屏幕颜色清理，也就是上一次设置的屏幕颜色
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        //把摄像头采集的数据输出出来
        //更新纹理，然后我们才可以使用opengl从SurfaceTexure当中获取数据，进行渲染
        mSurfaceTexture.updateTexImage();

        //mSurfaceTexture比较特殊，在设置坐标的时候，需要一个变换矩阵，使用的是特殊的采样器samplerExternalOES
        //这种采样器,正常的是sample2D

        mSurfaceTexture.getTransformMatrix(mtx);
        mCameraFilter.setMatrix(mtx);
        int id = mCameraFilter.onDrawFrame(mTextures[0]);
        //在这里添加各种效果，相当于责任链
        //开始画画
        mScreeFilter.onDrawFrame(id);

    }

    /**
     * SurfaceTexture有一个新的有效的图片的时候会被回调，此时可以把这个数据回调给GLSurfaceView的onDrawFrame
     *
     * @param surfaceTexture
     */
    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        if (mView != null) {
            //开始渲染，有一帧新的图像，就开始调用GLSurfaceView的onDrawFrame进行绘制
            mView.requestRender();
        }
    }
}
