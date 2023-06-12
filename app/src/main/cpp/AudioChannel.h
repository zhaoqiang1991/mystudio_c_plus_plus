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

public:

};


#endif //MY_APPLICATION_AUDIOCHANNEL_H
