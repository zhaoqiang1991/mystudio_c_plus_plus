//
// Created by lh on 2023/6/12.
//

#ifndef MY_APPLICATION_AUDIOCHANNEL_H
#define MY_APPLICATION_AUDIOCHANNEL_H

#include "BaseChannel.h"

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

public:
    pthread_t pid_audio_play;
    pthread_t pid_audio_decode;

};


#endif //MY_APPLICATION_AUDIOCHANNEL_H
