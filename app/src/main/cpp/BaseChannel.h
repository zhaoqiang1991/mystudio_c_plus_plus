//
// Created by lh on 2023/6/12.
//

#ifndef MY_APPLICATION_BASECHANNEL_H
#define MY_APPLICATION_BASECHANNEL_H
using namespace std;

#include "CallJavaHelper.h"
#include "safe_queue.h"

extern "C" {
#include <libavcodec/avcodec.h>
}


class BaseChannel {
public:

    BaseChannel(int channleId, AVCodecContext *avCodecContext, CallJavaHelper *javaCallHelper,
                AVRational time_base);

    virtual ~BaseChannel();

    //是否在播放
    virtual void play() = 0;

    //是否已经暂停
    virtual void stop() = 0;

    //释放AVFrame  指针的指针可以修改传递进来的指针的指向，来修改它
    static void releaseAvFrame(AVFrame **frame) {
        if (frame) {
            //如果没有释放，那么就释放frame
            av_frame_free(frame);
            *frame = nullptr;
        }
    }

    //释放AVPacket
    static void releaseAvPacket(AVPacket **packet) {
        if (packet) {
            //如果没有释放，那么就释放frame
            av_packet_free(packet);
            *packet = nullptr;
        }
    }


    //清空两个队列
    void clear() {
        packet_queue.clear();
        frame_queue.clear();
    }

    //暂停队列
    void stopWork() {
        packet_queue.setWork(0);
        frame_queue.setWork(0);
    }

    //开启队列
    void startWork() {
        packet_queue.setWork(1);
        frame_queue.setWork(1);
    }

public:
    volatile int channleId;
    AVCodecContext *avCodecContext;
    CallJavaHelper *javaCallHelper;
    volatile bool isPlaying = 0;
    //时间基
    AVRational time_base;
    float clock = 0;

    //视频流解码出来的packet包
    SafeQueue<AVPacket *> packet_queue;
    //视频流解码出来的一帧数据
    SafeQueue<AVFrame *> frame_queue;
};


#endif //MY_APPLICATION_BASECHANNEL_H
