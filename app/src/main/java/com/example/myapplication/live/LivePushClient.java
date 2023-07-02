package com.example.myapplication.live;

import android.hardware.Camera;
import android.view.SurfaceHolder;

public class LivePushClient {
    private VideoChannel videoChannel;

    public LivePushClient(LiveActivity activity, int width, int height, int bitrate, int fps, int cameraId) {
        native_init();
        videoChannel = new VideoChannel(this,activity, width, height, bitrate, fps, cameraId);
    }

    public void setPreviewDisplay(SurfaceHolder surfaceHolder) {
        videoChannel.setPreviewDisplay(surfaceHolder);
    }

    public void switchCamera() {
        videoChannel.switchCamera();
    }

    public void startLive(String path) {
        native_start(path);
        videoChannel.startLive();
        //audioChannel.startLive();
    }

    public void stopLive(){
        videoChannel.stopLive();
        //audioChannel.stopLive();
        native_stop();
    }

    public native void native_init();

    public native void native_pushVideo(byte[] data);

    public native void native_setVideoEncInfo(int w, int h, int mFps, int mBitrate);

    public native void native_start(String path);

    public native void native_stop();

    public native void native_release();
}
