//
// Created by lh on 2023/6/12.
//

#ifndef MY_APPLICATION_AUDIOCHANNEL_H
#define MY_APPLICATION_AUDIOCHANNEL_H

#include "BaseChannel.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

extern "C" {
#include <libswresample/swresample.h>
}

class AudioChannel : public BaseChannel {
public:
    AudioChannel(int channleId, AVCodecContext *avCodecContext, CallJavaHelper *javaCallHelper,
                 AVRational timeBase);

    ~AudioChannel() override;

    void play() override;

    void stop() override;

    //解码
    void decode();

    //播放
    void _play();

    //获取pcm数据
    int getPcm();

public:
    pthread_t pid_audio_play{};
    pthread_t pid_audio_decode{};

    //opensl es SLObjectItf对象
    /**
   * opensl es
   */
    SLObjectItf engineObject = NULL;
    SLEngineItf engineInterface = NULL;

    //混音器
    SLObjectItf outputMixObject = NULL;

    //播放器
    SLObjectItf bqPlayerObject = NULL;
    SLPlayItf bqPlayerInterface = NULL;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = NULL;


    SwrContext *swrContext = nullptr;
    uint8_t *data = nullptr;

    int out_channels;
    int out_samplesize;
    int out_sample_rate;
};


#endif //MY_APPLICATION_AUDIOCHANNEL_H
