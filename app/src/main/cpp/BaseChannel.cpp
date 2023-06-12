//
// Created by lh on 2023/6/12.
//

#include "BaseChannel.h"

BaseChannel::BaseChannel(int channleId, AVCodecContext *avCodecContext,
                         CallJavaHelper *javaCallHelper, AVRational time_base) :
        channleId(channleId), avCodecContext(avCodecContext), time_base(time_base) {
    //注册回收packet函数
    pkt_queue.setReleaseHandle(releaseAvPacket);
    frame_queue.setReleaseHandle(releaseAvFrame);

}

/**
 * 析构函数里面回收资源
 */
BaseChannel::~BaseChannel() {
    if (avCodecContext) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = nullptr;
    }
    pkt_queue.clear();
    frame_queue.clear();
    LOGD("============释放channel:%d %d", pkt_queue.size(), frame_queue.size());


}







