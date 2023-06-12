//
// Created by lh on 2023/6/12.
//

#ifndef MY_APPLICATION_VIDEOCHANNEL_H
#define MY_APPLICATION_VIDEOCHANNEL_H

#include "BaseChannel.h"
#include <pthread.h>

typedef void (*RenderFrame)(uint8_t *, int, int, int);

class VideoChannel /*: public BaseChannel*/ {
public:
    VideoChannel(int channleId);

public:
    int fps;
    pthread_t pid_video_play;
    pthread_t pid_synchronize;
    //渲染器
    RenderFrame renderFrame;
};


#endif //MY_APPLICATION_VIDEOCHANNEL_H
