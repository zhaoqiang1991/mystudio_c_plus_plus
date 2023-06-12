//
// Created by lh on 2023/6/12.
//

#include "BaseChannel.h"


BaseChannel::BaseChannel(int channleId, AVCodecContext *avCodecContext,
                         CallJavaHelper *javaCallHelper) : channleId(channleId),
                                                           avCodecContext(avCodecContext),
                                                           javaCallHelper(javaCallHelper) {

}


BaseChannel::~BaseChannel() {

}






