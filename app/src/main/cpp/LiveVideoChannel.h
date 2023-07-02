//
// Created by lh on 2023/7/1.
//

#ifndef MY_APPLICATION_LIVEVIDEOCHANNEL_H
#define MY_APPLICATION_LIVEVIDEOCHANNEL_H

#include <pthread.h>
#include <x264.h>
#include <inttypes.h>
#include "util.h"
#include "librtmp/rtmp.h"

class LiveVideoChannel {
    typedef void (*VideoCallback)(RTMPPacket *packet);

public:
    LiveVideoChannel();

    virtual ~LiveVideoChannel();

    void encodeData(int8_t *data);

public:
    void setVideoEncInfo(int width, int height, int fps, int bitrate);

    void sendSpsPps(uint8_t *sps, uint8_t *pps, int sps_len, int pps_len);

    void sendFrame(int type, uint8_t *payload, int i_payload);

    void setVideoCallback(VideoCallback videoCallback);


private:
    //编码的时候需要一个互斥锁，因为有可能在配置编码器的时候，但是屏幕大小有改变(推流的过程中有可能改变大小，因为推流
    // 是在一个线程，编码的时候是在另一个线程，处于多线程的环境)
    pthread_mutex_t mutex;
    int mWidth;
    int mHeight;
    int mFps;
    int mBitrate;

    int ySize;
    int uvSize;

    x264_t *videoCodec = NULL;
    x264_picture_t *pic_in = NULL;
    VideoCallback videoCallback = NULL;
};


#endif //MY_APPLICATION_LIVEVIDEOCHANNEL_H
