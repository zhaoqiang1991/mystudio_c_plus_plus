//
// Created by lh on 2023/5/30.
//

#include "TigerFFmpeg.h"
#include "util.h"
#include "BaseChannel.h"


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
    pthread_create(&this->pid_prepare, 0, prepare_FFmpeg, this);
}

void TigerFFmpeg::prepareFFmpeg() {
    //1. 初始化ffmepg网络，不然不能读取网络视频流
    avformat_network_init();
    //2.代表一个 视频/音频 包含了视频、音频的各种信息
    formatContext = avformat_alloc_context();
    //3.设置超时时间
    AVDictionary *pm = nullptr;
    av_dict_set(&pm, "timeout", "3000000", 0);
    //4.设置输入文件的封装格式
    int status = avformat_open_input(&formatContext, url, nullptr, &pm);
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
        //10.打开解码器
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
        } else if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频流
        }


    }
    if (callJavaHelper != nullptr) {
        //准备完成了，通知Java层可以开始播放了
        callJavaHelper->onPrepare(THREAD_CHILD);
    }

}

void TigerFFmpeg::start() {

}

void TigerFFmpeg::stop() {

}

void TigerFFmpeg::seek(int progress) {

}

void *TigerFFmpeg::prepare_FFmpeg(void *args) {
    TigerFFmpeg *tigerFFmpeg = static_cast<TigerFFmpeg *>(args);
    tigerFFmpeg->prepareFFmpeg();
    return nullptr;
}
