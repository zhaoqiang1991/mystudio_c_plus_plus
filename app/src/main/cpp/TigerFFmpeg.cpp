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
    av_dict_free(&opts);
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
        AVStream *stream = formatContext->streams[i];
        //7.拿到解码器参数
        AVCodecParameters *codecpar = stream->codecpar;
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
        if (avcodec_parameters_to_context(codecContext, codecpar) < 0) {
            if (callJavaHelper)
                callJavaHelper->onError(THREAD_CHILD,
                                        (jstring) FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
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
        AVRational base = stream->time_base;
        if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            //音频流
            LOGE("=========创建audio");
            audioChannel = new AudioChannel(i, codecContext, callJavaHelper, base);
        } else if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频流
            LOGE("=========创建vidio");
            //画面间隔==帧率
            AVRational frame_rate = stream->avg_frame_rate;
            int fps = av_q2d(frame_rate);
            videoChannel = new VideoChannel(i, codecContext, callJavaHelper, base, fps);
            videoChannel->setAudioChannel(audioChannel);
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
    if (audioChannel) {
        audioChannel->packet_queue.setWork(1);
        audioChannel->isPlaying = isPlaying;
        audioChannel->play();
    }

    if (videoChannel) {
        if (audioChannel) {
            videoChannel->setAudioChannel(audioChannel);
        }
        videoChannel->packet_queue.setWork(1);
        videoChannel->isPlaying = isPlaying;
        videoChannel->play();
    }
    pthread_create(&pid_play, nullptr, play, this);
}

/**
 * 异步等待当前prepare线程结束
 * @param args
 * @return
 */
void *ayns_stop(void *args) {
    TigerFFmpeg *fFmpeg = static_cast<TigerFFmpeg *>(args);
    pthread_join(fFmpeg->pid_prepare, 0);
    pthread_join(fFmpeg->pid_play, 0);
    DELETE(fFmpeg->videoChannel);
    DELETE(fFmpeg->audioChannel);

    if (fFmpeg->formatContext) {
        avformat_close_input(&fFmpeg->formatContext);
        avformat_free_context(fFmpeg->formatContext);
        fFmpeg->formatContext = nullptr;
    }
    DELETE(fFmpeg);
    return nullptr;
}

void TigerFFmpeg::stop() {
    isPlaying = 0;
    this->callJavaHelper = nullptr;
    pthread_create(&pid_stop, 0, ayns_stop, this);
}

void TigerFFmpeg::seek(int progress) {
    //进去必须 在0- duration 范围之类
    if (progress < 0 || progress >= duration) {
        return;
    }
    if (!audioChannel && !videoChannel) {
        return;
    }
    if (!formatContext) {
        return;
    }
    isSeek = 1;
    pthread_mutex_lock(&seekLock);
    //单位是 微妙
    int64_t seek = progress * 1000000;
    //seek到请求的时间 之前最近的关键帧
    // 只有从关键帧才能开始解码出完整图片
    av_seek_frame(formatContext, -1, seek, AVSEEK_FLAG_BACKWARD);
//    avformat_seek_file(formatContext, -1, INT64_MIN, seek, INT64_MAX, 0);
    // 音频、与视频队列中的数据 是不是就可以丢掉了？
    if (audioChannel) {
        //暂停队列
        audioChannel->stopWork();
        //可以清空缓存
//        avcodec_flush_buffers();
        audioChannel->clear();
        //启动队列
        audioChannel->startWork();
    }
    if (videoChannel) {
        videoChannel->stopWork();
        videoChannel->clear();
        videoChannel->startWork();
    }
    pthread_mutex_unlock(&seekLock);
    isSeek = 0;
}

void *TigerFFmpeg::prepare_FFmpeg(void *args) {
    auto *tigerFFmpeg = static_cast<TigerFFmpeg *>(args);
    tigerFFmpeg->prepareFFmpeg();
    return nullptr;
}

void TigerFFmpeg::_start() {
    int ref = 0;
    while (isPlaying) {
        //读取文件的时候没有网络请求，一下子就读取完毕了，可能会导致OOM，所以需要等待下
        //最重要的是读取本地文件的时候，一下子就会读取完毕，读取网络请求还好，因为也有请求网络时间
        if (audioChannel && audioChannel->packet_queue.size() > 100) {
            av_usleep(1000 * 10);
            continue;
        }

        if (videoChannel && videoChannel->packet_queue.size() > 100) {
            av_usleep(1000 * 10);
            continue;
        }


        pthread_mutex_lock(&seekLock);
        //如果是在播放，需要不断的从流中读取数据
        AVPacket *packet = av_packet_alloc();

        //从媒体文件中读取一帧数据
        ref = av_read_frame(formatContext, packet);
        pthread_mutex_unlock(&seekLock);
        /*//多线程的问题，不丢掉的话，seek的时候多出现一帧画面
        if(isSeek){
            av_packet_free(&packet);
            continue;
        }*/
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
            //读取完毕,但是可能还没有播放完成
            if (audioChannel->packet_queue.empty() && audioChannel->frame_queue.empty() &&
                videoChannel->packet_queue.empty() && videoChannel->frame_queue.empty()) {
                break;
            }
            //这里要继续循环的不能进行sleep,是因为如果是直播(不能拖动)的话，可以sleep,如果是点播(可以拖动，快进快退)，就会有问题，
        } else {
            //读取失败
            break;
        }
    }
    isPlaying = 0;
    audioChannel->stop();
    videoChannel->stop();

}

void TigerFFmpeg::setRenderFrameCallback(RenderFrame callback) {
    this->callback = callback;
}

int TigerFFmpeg::getDuration() {
    return duration;
}
