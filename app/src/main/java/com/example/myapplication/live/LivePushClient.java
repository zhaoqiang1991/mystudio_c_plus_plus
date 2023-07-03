package com.example.myapplication.live;

import android.hardware.Camera;
import android.view.SurfaceHolder;

public class LivePushClient {
    private final AudioChannel mAdioChannel;
    private VideoChannel videoChannel;


    public LivePushClient(LiveActivity activity, int width, int height, int bitrate, int fps, int cameraId) {
        native_init();
        videoChannel = new VideoChannel(this,activity, width, height, bitrate, fps, cameraId);
        mAdioChannel = new AudioChannel(this);
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
        mAdioChannel.startLive();
    }

    public void stopLive(){
        videoChannel.stopLive();
        mAdioChannel.stopLive();
        native_stop();
    }

    public void release() {
        videoChannel.release();
        mAdioChannel.release();
        native_release();
    }
    public native void native_init();

    public native void native_pushVideo(byte[] data);

    public native void native_setVideoEncInfo(int w, int h, int mFps, int mBitrate);

    public native void native_start(String path);

    public native void native_stop();

    public native void native_release();


    public native int getInputSamples();

    public native void native_setAudioEncInfo(int sampleRateInHz, int channels);

    public native void native_pushAudio(byte[] data);
}
