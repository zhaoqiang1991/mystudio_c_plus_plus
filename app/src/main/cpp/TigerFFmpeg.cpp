//
// Created by lh on 2023/5/30.
//

#include "TigerFFmpeg.h"
#include "util.h"
#include "BaseChannel.h"
#include "VideoChannel.h"
#include "AudioChannel.h"


TigerFFmpeg::TigerFFmpeg(CallJavaHelper *callJavaHelper, const char *dataSource) : callJavaHelper(
        callJavaHelper) {
    this->url = new char[strlen(dataSource) + 1];
    stpcpy(this->url, dataSource);
    isPlaying = 0;
    this->duration = 0;
    this->isSeek = 0;//todo 是否需要设置为0
    pthread_mutex_init(&this->seekLock, nullptr);
}

TigerFFmpeg::~TigerFFmpeg() {
    pthread_mutex_destroy(&this->seekLock);
    this->callJavaHelper = nullptr;
    DELETE(this->url);
}

/**
 * 因为准备阶段是一个耗时的过程，所以需要单独开启一个线程
 */
void TigerFFmpeg::prepare() {
    pthread_create(&this->pid_prepare, nullptr, prepare_FFmpeg, this);
}

void TigerFFmpeg::prepareFFmpeg() {
    //1. 初始化ffmepg网络，不然不能读取网络视频流
    avformat_network_init();
    //2.代表一个 视频/音频 包含了视频、音频的各种信息
    formatContext = avformat_alloc_context();
    //3.设置超时时间
    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "timeout", "3000000", 0);
    //4.设置输入文件的封装格式
    int status = avformat_open_input(&formatContext, url, nullptr, &opts);
    LOGE("%s open %d  %s", url, status, av_err2str(status));
    if (status != 0) {
        //打开失败，需要回调到Java层
        if (callJavaHelper != nullptr) {
            callJavaHelper->onError(THREAD_CHILD, (jstring) FFMPEG_CAN_NOT_OPEN_URL);
        }
        return;
    }

    //5.查找流
    int ref = avformat_find_stream_info(formatContext, nullptr);
    //>=0 if OK, AVERROR_xxx on error
    if (ref < 0) {
        if (callJavaHelper != nullptr) {
            //找不到媒体流
            callJavaHelper->onError(THREAD_CHILD, (jstring) FFMPEG_CAN_NOT_FIND_STREAMS);
        }
        return;
    }
    //视频时长（单位：微秒us，转换为秒需要除以1000000）
    duration = formatContext->duration / 1000000;

    //6.查找流
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        //遍历总共有几路流

        //7.拿到解码器参数
        AVCodecParameters *codecpar = formatContext->streams[i]->codecpar;
        //8.查找解码器
        AVCodec *avCodec = avcodec_find_decoder(codecpar->codec_id);
        if (!avCodec) {
            if (callJavaHelper != nullptr) {
                //找不到媒体流
                callJavaHelper->onError(THREAD_CHILD, (jstring) FFMPEG_FIND_DECODER_FAIL);
            }
            return;
        }
        //9.创建解码器上下文
        AVCodecContext *codecContext = avcodec_alloc_context3(avCodec);
        if (!codecContext) {
            if (callJavaHelper != nullptr) {
                //无法根据加码器创建上下文
                callJavaHelper->onError(THREAD_CHILD, (jstring) FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            }
            return;
        }
        //10 赋值参数,这一步特别重要
        if(avcodec_parameters_to_context(codecContext,codecpar) < 0 ){
            if (callJavaHelper)
                callJavaHelper->onError(THREAD_CHILD,(jstring) FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            return;
        }

        //11.打开解码器
        //@return zero on success, a negative value on error
        if (avcodec_open2(codecContext, avCodec, nullptr) != 0) {
            if (callJavaHelper != nullptr) {
                //无法根据加码器创建上下文
                callJavaHelper->onError(THREAD_CHILD,
                                        (jstring) FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            }
            return;
        }
        //拿到对应视频流的时间基
        AVRational base = formatContext->streams[i]->time_base;
        if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            //音频流
            LOGE("=========创建audio");
            audioChannel = new AudioChannel(i, codecContext, callJavaHelper, base);
        } else if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频流
            LOGE("=========创建vidio");
            videoChannel = new VideoChannel(i, codecContext, callJavaHelper, base, 0);
            videoChannel->setRenderFrameCallback(callback);
        }


    }
    if (callJavaHelper != nullptr) {
        //准备完成了，通知Java层可以开始播放了
        callJavaHelper->onPrepare(THREAD_CHILD);
    }

}


void *play(void *args) {
    auto *tigerFFmpeg = static_cast<TigerFFmpeg *>(args);
    tigerFFmpeg->_start();
    //线程必须要return，不然会出现各种奇怪的问题，很难排查
    return nullptr;
}

void TigerFFmpeg::start() {
    isPlaying = 1;
    //因为需要从流中不断的读取网络上数据，而且外部有可能是在主线程中调用，因此需要单独的开启一个线程
    if (videoChannel) {
        videoChannel->packet_queue.setWork(1);
        videoChannel->isPlaying = isPlaying;
        videoChannel->play();
    }

    if (audioChannel) {
        audioChannel->packet_queue.setWork(1);
        audioChannel->isPlaying = isPlaying;
        audioChannel->play();
    }
    pthread_create(&pid_play, nullptr, play, this);
}

void TigerFFmpeg::stop() {

}

void TigerFFmpeg::seek(int progress) {

}

void *TigerFFmpeg::prepare_FFmpeg(void *args) {
    auto *tigerFFmpeg = static_cast<TigerFFmpeg *>(args);
    tigerFFmpeg->prepareFFmpeg();
    return nullptr;
}

void TigerFFmpeg::_start() const {
    int ref = 0;
    while (isPlaying) {
        //如果是在播放，需要不断的从流中读取数据
        AVPacket *packet = av_packet_alloc();

        //从媒体文件中读取一帧数据
        ref = av_read_frame(formatContext, packet);
        if (ref == 0) {
            //读取成功
            if (audioChannel && packet->stream_index == audioChannel->channleId) {
                //表示音频
                audioChannel->packet_queue.push(packet);
            } else if (videoChannel && packet->stream_index == videoChannel->channleId) {
                //表示视频
                //读取到一帧数据，就放到视频队列里面
                videoChannel->packet_queue.push(packet);
            }
        } else if (ref == AVERROR_EOF) {
            //读取完毕
        } else {
            //读取失败
        }
    }
}

void TigerFFmpeg::setRenderFrameCallback(RenderFrame callback) {
    this->callback = callback;
}
