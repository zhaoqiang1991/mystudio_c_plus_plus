//
// Created by lh on 2023/6/12.
//

#ifndef MY_APPLICATION_BASECHANNEL_H
#define MY_APPLICATION_BASECHANNEL_H
using namespace std;

#include "CallJavaHelper.h"

extern "C" {
#include <libavcodec/avcodec.h>
}


class BaseChannel {
public:

    BaseChannel(int channleId, AVCodecContext *avCodecContext, CallJavaHelper *javaCallHelper);

    virtual ~BaseChannel();

public:
    int channleId;
    AVCodecContext *avCodecContext;
    CallJavaHelper *javaCallHelper;
};


#endif //MY_APPLICATION_BASECHANNEL_H
