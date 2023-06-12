//
// Created by lh on 2023/6/12.
//

#include "AudioChannel.h"


AudioChannel::AudioChannel(int channleId, AVCodecContext *avCodecContext,
                           CallJavaHelper *javaCallHelper, AVRational timeBase)
        : BaseChannel(channleId, avCodecContext, javaCallHelper, timeBase) {

}
