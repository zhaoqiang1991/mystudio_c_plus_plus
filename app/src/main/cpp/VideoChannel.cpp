//
// Created by lh on 2023/6/12.
//

#include "VideoChannel.h"


VideoChannel::VideoChannel(int channleId, AVCodecContext *avCodecContext,
                           CallJavaHelper *javaCallHelper, AVRational timeBase, int fps)
        : BaseChannel(channleId, avCodecContext, javaCallHelper, timeBase), fps(fps) {}
