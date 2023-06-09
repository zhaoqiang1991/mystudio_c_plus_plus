//
// Created by lh on 2023/5/30.
//

#ifndef MY_APPLICATION_TIGERFFMPEG_H
#define MY_APPLICATION_TIGERFFMPEG_H


#include "CallJavaHelper.h"

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
public:
    TigerFFmpeg(CallJavaHelper *callJavaHelper, const char *dataSource);

   //TigerFFmpeg(CallJavaHelper *callJavaHelper, char *dataSource);

    virtual ~TigerFFmpeg();

    void prepare();

    void prepareFFmpeg();

    void start();

    void stop();

    void seek(int progress);

    static void *prepare_FFmpeg(void *args);

public:
    CallJavaHelper *callJavaHelper;
    char *url;

    //准备，播放，停止都放在子线程中
    pthread_t pid_prepare;
    pthread_t pid_play;
    pthread_t pid_stop;
    AVFormatContext *formatContext = nullptr;


    pthread_mutex_t seekLock;

    //todo 缺少ffmpeg相关的接口
    //时长
    int duration;
    //是否在播放
    int isPlaying;

    //是否在拖动
    int isSeek;
};


#endif //MY_APPLICATION_TIGERFFMPEG_H
