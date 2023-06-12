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
        native_prepare(url);
    }

    public void start() {
        native_start();
    }

    public void stop() {
        native_stop();
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

    public void onError(int threadId, String errorDesc) {
        if (mOnErrorListener != null) {
            //回调到java层
            mOnErrorListener.onError(threadId, errorDesc);
        }
    }

    public void onPrepared() {
        //回调到java层
        if (mOnPrepareListener != null) {
            mOnPrepareListener.onPrepared();
        }
    }


    public void onProgress(int progress) {
        //回调到java层
        if (mOnProgressListener != null) {
            mOnProgressListener.onProgress(progress);
        }
    }

    private native void native_prepare(String url);

    private native void native_start();

    private native void native_stop();

    private native void native_seek(int progress);
}
