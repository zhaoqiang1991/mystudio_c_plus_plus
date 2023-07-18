package com.example.myapplication.opengl;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.EGL14;
import android.opengl.EGLContext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import com.example.myapplication.face.Face;
import com.example.myapplication.face.FaceTrack;
import com.example.myapplication.filter.BeautyFilter;
import com.example.myapplication.filter.BigEyeFilter;
import com.example.myapplication.filter.CameraFilter;
import com.example.myapplication.filter.ScreeFilter;
import com.example.myapplication.filter.StickFilter;
import com.example.myapplication.record.MediaRecorder;
import com.example.myapplication.utils.CameraHelper;
import com.example.myapplication.utils.OpenGLUtils;

import java.io.IOException;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class TigerRender implements GLSurfaceView.Renderer, SurfaceTexture.OnFrameAvailableListener, Camera.PreviewCallback {
    private final TigerView mView;
    private CameraHelper mCameraHelper;
    private SurfaceTexture mSurfaceTexture;
    float[] mtx = new float[16];
    private ScreeFilter mScreeFilter;
    private int[] mTextures;
    private CameraFilter mCameraFilter;
    private MediaRecorder mMediaRecorder;
    private FaceTrack mFaceTrack;
    private BigEyeFilter mBigEyeFilter;
    private StickFilter mStickFilter;
    private BeautyFilter mBeautyFilter;

    private int mHeigh;
    private int mWidth;

    private MediaRecorder.OnRecordFinishListener mListener;

    public TigerRender(TigerView tigerView) {
        mView = tigerView;
        Context context = mView.getContext();
        //拷贝 模型
        OpenGLUtils.copyAssets2SdCard(context, "lbpcascade_frontalface.xml", "/sdcard/lbpcascade_frontalface.xml");
        OpenGLUtils.copyAssets2SdCard(context, "seeta_fa_v1.1.bin", "/sdcard/seeta_fa_v1.1.bin");
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
        mCameraHelper = new CameraHelper(Camera.CameraInfo.CAMERA_FACING_FRONT);
        mCameraHelper.setPreviewCallback(this);
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

        /*mBigEyeFilter = new BigEyeFilter(mView.getContext());
        mStickFilter = new StickFilter(mView.getContext());
        mBeautyFilter = new BeautyFilter(mView.getContext());*/

        EGLContext eglContext = EGL14.eglGetCurrentContext();
        mMediaRecorder = new MediaRecorder(mView.getContext(), "/mnt/sdcard/test.mp4", CameraHelper.HEIGHT, CameraHelper.WIDTH, eglContext);
        mMediaRecorder.setOnRecordFinishListener(mListener);
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
        mWidth = width;
        mHeigh = height;
        mFaceTrack = new FaceTrack("/sdcard/lbpcascade_frontalface.xml",
                "/sdcard/seeta_fa_v1.1.bin", mCameraHelper);
        //启动跟踪器
        mFaceTrack.startTrack();
        //开启预览
        mCameraHelper.startPreview(mSurfaceTexture);
        mCameraFilter.onReady(width, height);
        mScreeFilter.onReady(width, height);


        /*mBigEyeFilter.onReady(width, height);
        mStickFilter.onReady(width, height);
        mBeautyFilter.onReady(width, height);*/
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

        Face face = mFaceTrack.getFace();
        if (null != mBigEyeFilter) {
            mBigEyeFilter.setFace(face);
            id = mBigEyeFilter.onDrawFrame(id);
        }
        // 贴纸
        if (null != mStickFilter) {
            mStickFilter.setFace(face);
            id = mStickFilter.onDrawFrame(id);
        }
        if (null != mBeautyFilter) {
            id = mBeautyFilter.onDrawFrame(id);
        }

        //在这里添加各种效果，相当于责任链
        //开始画画
        mScreeFilter.onDrawFrame(id);

        mMediaRecorder.encodeFrame(id, mSurfaceTexture.getTimestamp());
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

    public void startRecord(float speed) {
        try {
            mMediaRecorder.start(speed);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public void onSurfaceDestroyed() {
        mCameraHelper.stopPreview();
        mFaceTrack.stopTrack();
    }

    public void stopRecord() {
        mMediaRecorder.stop();
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        //data送去进行人脸检测和关键点定位
        mFaceTrack.detector(data);
    }

    public void switchCamera() {
        mCameraHelper.switchCamera();
    }

    public void setOnRecordFinishListener(MediaRecorder.OnRecordFinishListener listener) {
        if (null != mMediaRecorder) {
            mMediaRecorder.setOnRecordFinishListener(listener);
        }
        mListener = listener;
    }

    public void enableStick(boolean isChecked) {
        //向GL线程发布一个任务
        //任务会放入一个任务队列， 并在gl线程中去执行
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                //Opengl线程
                if (isChecked) {
                    mStickFilter = new StickFilter(mView.getContext());
                    mStickFilter.onReady(mWidth, mHeigh);
                } else {
                    mStickFilter.release();
                    mStickFilter = null;
                }
            }
        });
    }

    public void enableBeauty(boolean isChecked) {
        //向GL线程发布一个任务
        //任务会放入一个任务队列， 并在gl线程中去执行
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                //Opengl线程
                if (isChecked) {
                    mBeautyFilter = new BeautyFilter(mView.getContext());
                    mBeautyFilter.onReady(mWidth, mHeigh);
                } else {
                    mBeautyFilter.release();
                    mBeautyFilter = null;
                }
            }
        });
    }

    public void enableBigEye(boolean isChecked) {
        //向GL线程发布一个任务
        //任务会放入一个任务队列， 并在gl线程中去执行
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                //Opengl线程
                if (isChecked) {
                    mBigEyeFilter = new BigEyeFilter(mView.getContext());
                    mBigEyeFilter.onReady(mWidth, mHeigh);
                } else {
                    mBigEyeFilter.release();
                    mBigEyeFilter = null;
                }
            }
        });
    }
}
