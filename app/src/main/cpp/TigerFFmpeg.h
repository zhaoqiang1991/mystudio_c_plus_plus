//
// Created by lh on 2023/5/30.
//

#ifndef MY_APPLICATION_TIGERFFMPEG_H
#define MY_APPLICATION_TIGERFFMPEG_H


#include "CallJavaHelper.h"
#include "AudioChannel.h"
#include "VideoChannel.h"

extern "C" {
//因为这两个库是c语言写的，但是现在我们是在c++里面应用，所以要指导gcc/g++把这个
//c语言的库编译成c语言的符号
#include <libavformat/avformat.h>
#include <libavutil/time.h>
};


using namespace std;

/**
 * 对FFmpeg的封装
 */

class TigerFFmpeg {
    friend void* async_stop(void* args);
public:
    TigerFFmpeg(CallJavaHelper *javaCallHelper, const char *dataSource);

    ~TigerFFmpeg();

    void prepare();

    void prepareFFmpeg();

    void start();

    void play();

    void setRenderCallback(RenderFrame renderFrame);

    void stop();

    int getDuration() {
        return duration;
    }

    void seek(int i);

private:
    char *url;
    CallJavaHelper *javaCallHelper;

    pthread_t pid_prepare;
    pthread_t pid_play;
    pthread_t pid_stop;

    pthread_mutex_t seekMutex;
    AVFormatContext *formatContext = 0;

    int duration;

    RenderFrame renderFrame;

    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;

    bool isPlaying;
    bool isSeek = 0;
};


#endif //MY_APPLICATION_TIGERFFMPEG_H
