//
// Created by lh on 2023/6/12.
//

#ifndef MY_APPLICATION_VIDEOCHANNEL_H
#define MY_APPLICATION_VIDEOCHANNEL_H

#include "BaseChannel.h"
#include <pthread.h>
#include "AudioChannel.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
}

typedef void (*RenderFrame)(uint8_t *, int, int, int);

class VideoChannel : public BaseChannel {
public:
    VideoChannel(int channleId, AVCodecContext *avCodecContext, CallJavaHelper *javaCallHelper,
                 AVRational timeBase, int fps);

    ~VideoChannel() override;

    void play() override;

    void stop() override;

    void * decodePacket();

    void render();

    void setRenderFrameCallback(RenderFrame callback);

    void setAudioChannel(AudioChannel* audioChannel);


public:
    int fps;
    pthread_t pid_decode;
    pthread_t pid_render;
    //渲染器
    RenderFrame renderFrame = nullptr;

    SwsContext *swsContext = nullptr;

    AudioChannel *audioChannel= nullptr;
};


#endif //MY_APPLICATION_VIDEOCHANNEL_H
