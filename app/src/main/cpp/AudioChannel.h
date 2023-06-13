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
    SLObjectItf engineObject = nullptr;
    SLEngineItf engineEngine = nullptr;
    SLObjectItf outputMixObject = nullptr;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = nullptr;
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    SLmilliHertz bqPlayerSampleRate = 0;
    SLObjectItf bqPlayerObject = nullptr;
    SLPlayItf bqPlayerPlay = nullptr;
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = nullptr;
    SwrContext *swrContext = nullptr;
    uint8_t *data = nullptr;

    int out_channels;
    int out_samplesize;
    int out_sample_rate;
};


#endif //MY_APPLICATION_AUDIOCHANNEL_H
