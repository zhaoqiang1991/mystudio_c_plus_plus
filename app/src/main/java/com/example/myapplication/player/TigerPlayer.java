package com.example.myapplication.player;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;


import androidx.annotation.NonNull;

import com.example.myapplication.listener.OnErrorListener;
import com.example.myapplication.listener.OnPrepareListener;
import com.example.myapplication.listener.OnProgressListener;

public class TigerPlayer implements SurfaceHolder.Callback {
    private String url;
    private OnPrepareListener mOnPrepareListener;
    private OnErrorListener mOnErrorListener;
    private OnProgressListener mOnProgressListener;
    private SurfaceHolder holder;

    public void setDataSource(String url) {
        this.url = url;
    }

    public void setSurfaceView(SurfaceView surfaceView) {
        /*if(holder != null){
            holder.removeCallback(this);
        }*/
        holder = surfaceView.getHolder();
        holder.addCallback(this);
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

    public void release(){
        holder.removeCallback(this);
        native_release();
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
        native_setSurface(holder.getSurface());
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

    //设置可用的画布
    private native void native_setSurface(Surface surface);

    private native void native_release();
}
