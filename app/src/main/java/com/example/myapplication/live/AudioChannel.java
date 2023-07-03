package com.example.myapplication.live;

import android.annotation.SuppressLint;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class AudioChannel {
    private static final String TAG = AudioChannel.class.getName();
    private final AudioRecord mAudioRecord;
    private LivePushClient mLivePusher;
    private final int channels = 2;
    private int inputSamples;
    private ExecutorService executor;
    private boolean isLiving;

    @SuppressLint("MissingPermission")
    public AudioChannel(LivePushClient livePushClient) {
        //准备录音机 采集pcm 数据
        executor = Executors.newSingleThreadExecutor();
        int channelConfig;
        if (channels == 2) {
            channelConfig = AudioFormat.CHANNEL_IN_STEREO;
        } else {
            channelConfig = AudioFormat.CHANNEL_IN_MONO;
        }
        mLivePusher = livePushClient;
        //这句话特别重要，必须写在这里，不然会出现那种嘈杂的电流声
        mLivePusher.native_setAudioEncInfo(44100, channels);
        //16 位 2个字节
        inputSamples = mLivePusher.getInputSamples() * 2;
        //最小需要的缓冲区
        int minBufferSize = AudioRecord.getMinBufferSize(44100, channelConfig, AudioFormat.ENCODING_PCM_16BIT) * 2;
        mAudioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, 44100, channelConfig, AudioFormat.ENCODING_PCM_16BIT,
                minBufferSize > inputSamples ? minBufferSize : inputSamples);
       // mLivePusher.native_setAudioEncInfo(44100,channels);

    }

    public void stopLive() {
        isLiving = false;
    }

    public void startLive() {
        isLiving = true;
        executor.submit(new AudoTask());
    }


    public void release(){
        if(mAudioRecord != null){
            mAudioRecord.release();
        }
    }
    private class AudoTask implements Runnable {

        @Override
        public void run() {
            mAudioRecord.startRecording();
            byte[] bytes = new byte[inputSamples];
            while (isLiving){
                int len = mAudioRecord.read(bytes, 0, bytes.length);
                if(len > 0){
                    Log.d(TAG,"audio 开始推送");
                    mLivePusher.native_pushAudio(bytes);
                }
            }
            mAudioRecord.stop();
        }
    }
}
