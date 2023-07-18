package com.example.myapplication.opengl;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

import com.example.myapplication.record.MediaRecorder;

public class TigerView extends GLSurfaceView {

    private TigerRender mTigerRender;
    //默认正常速度
    private Speed mSpeed = Speed.MODE_NORMAL;

    public void setSpeed(Speed speed) {
        mSpeed = speed;
    }

    public void setOnRecordFinishListener(MediaRecorder.OnRecordFinishListener listener) {
        mTigerRender.setOnRecordFinishListener(listener);
    }

    public void switchCamera() {
        mTigerRender.switchCamera();
    }

    public void enableStick(boolean isChecked) {
        mTigerRender.enableStick(isChecked);
    }

    public void enableBeauty(boolean isChecked) {
        mTigerRender.enableBeauty(isChecked);
    }

    public void enableBigEye(boolean isChecked) {
        mTigerRender.enableBigEye(isChecked);
    }

    public enum Speed {
        MODE_EXTRA_SLOW, MODE_SLOW, MODE_NORMAL, MODE_FAST, MODE_EXTRA_FAST
    }

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
        mTigerRender = new TigerRender(this);
        setRenderer(mTigerRender);

        /**
         * 设置按需渲染，当我们调用requestRender()的时候就会调用GlThread回调一次onDrawFrame()
         */
        setRenderMode(RENDERMODE_WHEN_DIRTY);

    }

    public void startRecord() {
        float speed = 1.f;
        switch (mSpeed) {
            case MODE_EXTRA_SLOW:
                speed = 0.3f;
                break;
            case MODE_SLOW:
                speed = 0.5f;
                break;
            case MODE_NORMAL:
                speed = 1.f;
                break;
            case MODE_FAST:
                speed = 1.5f;
                break;
            case MODE_EXTRA_FAST:
                speed = 3.f;
                break;
        }
        mTigerRender.startRecord(speed);
    }

    public void stopRecord() {
        mTigerRender.stopRecord();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
        mTigerRender.onSurfaceDestroyed();
    }
}
