package com.example.myapplication.player;

import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

import com.example.myapplication.listener.OnErrorListener;
import com.example.myapplication.listener.OnPrepareListener;
import com.example.myapplication.listener.OnProgressListener;

public class TigerPlayer implements SurfaceHolder.Callback {
    private String url;
    private SurfaceView mSurfaceView;

    private OnPrepareListener mOnPrepareListener;
    private OnErrorListener mOnErrorListener;
    private OnProgressListener mOnProgressListener;

    public void setDataSource(String url) {
        this.url = url;
    }

    public void setSurfaceView(SurfaceView surfaceView) {
        this.mSurfaceView = surfaceView;
    }

    /**
     * 准备阶段
     */
    public void prepare() {
        _prepare(url);
    }

    public void start() {
        _start();
    }

    public void stop() {
        _stop();
    }


    public void setOnPrepareListener(OnPrepareListener onPrepareListener) {
        this.mOnPrepareListener = onPrepareListener;
    }

    public void setOnErrorListener(OnErrorListener onErrorListener) {
        this.mOnErrorListener = onErrorListener;
    }

    public void setOnProgressListener(OnProgressListener onProgressListener) {
        this.mOnProgressListener = onProgressListener;
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }

    public native void _prepare(String url);

    public native void _start();

    public native void _stop();

    public native void _seek(int progress);
}
