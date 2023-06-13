//
// Created by lh on 2023/6/12.
//

#include "VideoChannel.h"


VideoChannel::VideoChannel(int channleId, AVCodecContext *avCodecContext,
                           CallJavaHelper *javaCallHelper, AVRational timeBase, int fps)
        : BaseChannel(channleId, avCodecContext, javaCallHelper, timeBase), fps(fps) {}

VideoChannel::~VideoChannel() {

}

void *decode_task(void *args) {
    auto *channel = static_cast<VideoChannel *>(args);
    channel->decodePacket();
    return nullptr;
}


void *render_task(void *args) {
    auto *channel = static_cast<VideoChannel *>(args);
    channel->render();
    return nullptr;
}

void VideoChannel::play() {
    isPlaying = 1;
    startWork();
    //1. 解码
    pthread_create(&pid_decode, nullptr, decode_task, this);

    //2，播放
    pthread_create(&pid_render, nullptr, render_task, this);
}

void VideoChannel::stop() {

}

void VideoChannel::decodePacket() {
    //开始真正的解码，使用一个循环不断的从队列里面读取数据，直到没有数据
    AVPacket *packet = 0;
    int ref = 0;
    while (isPlaying) {
        //读取一个数据包
        ref = pkt_queue.pop(packet);
        if (!isPlaying) {
            //没有开始播放就需要退出读取包
            break;
        }
        if (!ref) {
            //没有暂停，那么就继续从媒体文件中读取数据
            continue;
        }

        //把数据丢给解码器， 丢给ffmpeg的是一个包，拿出来的是一帧数据  send -->receive
        ref = avcodec_send_packet(this->avCodecContext, packet);
        releaseAvPacket(&packet);
        if (ref == AVERROR(EAGAIN)) {
            //重试 解码器里面的数据太多太多了，需要读取一些数据才能够存放，缓存里面放不下这些包了
            continue;
        } else if (ref < 0) {
            break;
        }
        AVFrame *avFrame = av_frame_alloc();

        //从解码器里面读取一帧数据
        ref = avcodec_receive_frame(this->avCodecContext, avFrame);
        if (ref == AVERROR(EAGAIN)) {
            //需要更多的数据，才能够解码
            continue;
        } else if (ref < 0) {
            break;
        }

        //在开一个线程来播放(保证播放的流畅度),不然下一帧数据来的时候，可能会有延迟
        //把解码到的一帧一帧的数据放在一个帧队列里面去处理
        frame_queue.push(avFrame);
    }

    releaseAvPacket(&packet);
}

void VideoChannel::render() {
    swsContext = sws_getContext(avCodecContext->width, avCodecContext->height,
                                avCodecContext->pix_fmt,
                                avCodecContext->width, avCodecContext->height,
                                AV_PIX_FMT_ARGB, SWS_BILINEAR, 0, 0, 0);
    //图像颜色空间转化 ，不停地去拿数据，不停地去转化数据
    AVFrame *avFrame;
    uint8_t *dst_data[4];
    int dst_linesize[4];
    av_image_alloc(dst_data, dst_linesize, avCodecContext->width, avCodecContext->height,
                   AV_PIX_FMT_ABGR, 1);
    while (isPlaying) {
        int ref = frame_queue.pop(avFrame);

        if (!isPlaying) {
            break;
        }
        if (!ref) {
            continue;
        }
        //dst_linesize 表示一行存放的字节长度数据
        sws_scale(swsContext, avFrame->data, avFrame->linesize, 0, avCodecContext->height, dst_data,
                  dst_linesize);

        //回调出去进行播放，只需要那数组的第0个元素就可以，其他的都是null
        renderFrame(dst_data[0],dst_linesize[0],avCodecContext->width,avCodecContext->height);

        //avFrame用完就没有用了，就可以释放了
        releaseAvFrame(&avFrame);
    }

    av_freep(&dst_data[0]);
    releaseAvFrame(&avFrame);
}

void VideoChannel::setRenderFrameCallback(RenderFrame callback) {
    this->renderFrame = callback;
}
