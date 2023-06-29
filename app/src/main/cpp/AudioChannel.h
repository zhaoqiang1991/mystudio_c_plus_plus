//
// Created by lh on 2023/6/12.
//

#ifndef MY_APPLICATION_AUDIOCHANNEL_H
#define MY_APPLICATION_AUDIOCHANNEL_H


#include "BaseChannel.h"
#include <pthread.h>
#include <SLES/OpenSLES_Android.h>

extern "C" {
#include <libswresample/swresample.h>

}


class AudioChannel : public BaseChannel {
public:
    AudioChannel(int id, CallJavaHelper *javaCallHelper, AVCodecContext *avCodecContext,
                 AVRational base);

    virtual ~AudioChannel();

    virtual void play();

    virtual void stop();

    void initOpenSL();

    void decode();

    void releaseOpenSL();

    int getPcm();


private:
    pthread_t pid_audio_play;
    pthread_t pid_audio_decode;
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


    SwrContext *swr_ctx = NULL;
    int out_channels;
    int out_samplesize;
    int out_sample_rate;
public:
    uint8_t *buffer;

};



#endif //MY_APPLICATION_AUDIOCHANNEL_H
