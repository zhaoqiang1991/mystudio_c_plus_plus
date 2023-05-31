//
// Created by lh on 2023/5/30.
//

#ifndef MY_APPLICATION_TIGERFFMPEG_H
#define MY_APPLICATION_TIGERFFMPEG_H


#include "CallJavaHelper.h"

using namespace std;

/**
 * 对FFmpeg的封装
 */

class TigerFFmpeg {
public:
    TigerFFmpeg(CallJavaHelper *callJavaHelper, char *dataSource);

    virtual ~TigerFFmpeg();

    void prepare();

    void prepareFFmpeg();

    void start();

    void stop();

    void seek(int progress);

public:
    CallJavaHelper *callJavaHelper;
    char *data_source;

    //准备，播放，停止都放在子线程中
    pthread_t pid_prepare;
    pthread_t pid_play;
    pthread_t pid_stop;


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
