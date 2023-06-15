//
// Created by lh on 2023/6/12.
//

#include "VideoChannel.h"

/**
 * 针对视频挤压的多余的包，进行丢包,丢包，指导下一个关键帧(I帧)
 * @param q
 */
void dropAvPacket(queue<AVPacket*> &q) {
    while (!q.empty()){
       AVPacket  * packet = q.front();
       //如果不属于I帧
       if(packet->flags != AV_PKT_FLAG_KEY){
           BaseChannel::releaseAvPacket(&packet);
           //出队列
           q.pop();
       }else{
           break;
       }
    }
}


/**
 * 丢frames比较简单，就不会有太多的限制，相对丢包比较简单
 * @param q
 */
void dropAvFrame(queue<AVFrame *> &q) {
    if (!q.empty()){
        AVFrame * frame = q.front();
        BaseChannel::releaseAvFrame(&frame);
        q.pop();
    }
}

VideoChannel::VideoChannel(int channleId, AVCodecContext *avCodecContext,
                           CallJavaHelper *javaCallHelper, AVRational timeBase, int fps)
        : BaseChannel(channleId, avCodecContext, javaCallHelper, timeBase), fps(fps) {
    //用于设置一个同步操作  //丢frame包比较简单
    frame_queue.setSyncHandle(dropAvFrame);
    //丢包比较复杂
    //packet_queue.setSyncHandle(dropAvPacket);
}

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
    isPlaying = 0;
    packet_queue.setWork(0);
    frame_queue.setWork(0);
    //pthread_join 等待，卡主pid_decode线程 等待pid_decode执行完毕，此时代码会一致卡在79行，不继续向下执行
    pthread_join(pid_decode,0);
    pthread_join(pid_render,0);

}

void * VideoChannel::decodePacket() {
    //开始真正的解码，使用一个循环不断的从队列里面读取数据，直到没有数据
    AVPacket *packet = 0;
    int ref = 0;
    while (isPlaying) {
        //读取一个数据包
        ref = packet_queue.pop(packet);
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
                                AV_PIX_FMT_RGBA, SWS_BILINEAR, 0, 0, 0);
    //每一个画面刷新的间隔
    double frame_delays = 1.0 / fps;

    //图像颜色空间转化 ，不停地去拿数据，不停地去转化数据
    AVFrame *avFrame;
    uint8_t *dst_data[4];
    int dst_linesize[4];
    av_image_alloc(dst_data, dst_linesize, avCodecContext->width, avCodecContext->height,
                   AV_PIX_FMT_RGBA, 1);
    while (isPlaying) {
        int ref = frame_queue.pop(avFrame);

        if (!isPlaying) {
            break;
        }
        if (!ref) {
            continue;
        }
        if(javaCallHelper && !audioChannel){
            javaCallHelper->onProgress(THREAD_CHILD,clock);
        }

        //dst_linesize 表示一行存放的字节长度数据
        sws_scale(swsContext, avFrame->data, avFrame->linesize, 0, avCodecContext->height, dst_data,
                  dst_linesize);
        //获得当前画面播放的相对时间,best_effort_timestamp大概率情况下会和pts一样
        double clock = avFrame->best_effort_timestamp * av_q2d(time_base);
        //额外的延迟时间
        double extra_delay = avFrame->repeat_pict / (2 * fps);
        double delays = frame_delays + extra_delay;
        if (clock == 0) {
            //正常播放
            av_usleep(delays * 1000000);
        } else {
            double audioClock = audioChannel ? audioChannel->clock : 0;
            double diff = fabs(clock - audioClock);
            LOGD("当前和音频比较:%f - %f = %f", clock, audioClock, diff);
            //允许误差 diff > 0.04 &&
            if (audioChannel) {
                //如果视频比音频快，延迟差值播放，否则直接播放
                if (clock > audioClock) {
                    if (diff > 1) {
                        //差的太久了， 那只能慢慢赶 不然就是卡好久
                        av_usleep((delays * 2) * 1000000);
                    } else {
                        //差的不多，尝试一次赶上去
                        av_usleep((delays + diff) * 1000000);
                    }
                } else {
                    //音频比视频快
                    //视频慢了 0.05s 已经比较明显了 (丢帧)
                    if (diff > 1) {
                        //一种可能： 快进了(因为解码器中有缓存数据，这样获得的avframe就和seek的匹配了)
                    } else if (diff >= 0.05) {
                        releaseAvFrame(&avFrame);
                        //执行同步操作 删除到最近的key frame
                        frame_queue.sync();
                        continue;
                    } else {
                        //不休眠 加快速度赶上去
                    }
                }
            } else {
                //正常播放
                av_usleep(delays * 1000000);
            }
        }

        //休眠

        //回调出去进行播放，只需要那数组的第0个元素就可以，其他的都是null
        renderFrame(dst_data[0], dst_linesize[0], avCodecContext->width, avCodecContext->height);

        //avFrame用完就没有用了，就可以释放了
        releaseAvFrame(&avFrame);
    }

    av_freep(&dst_data[0]);
    releaseAvFrame(&avFrame);

    isPlaying = 0 ;
    sws_freeContext(swsContext);
    swsContext = nullptr;
}

void VideoChannel::setRenderFrameCallback(RenderFrame callback) {
    this->renderFrame = callback;
}

void VideoChannel::setAudioChannel(AudioChannel *audioChannel) {
    this->audioChannel = audioChannel;
}
