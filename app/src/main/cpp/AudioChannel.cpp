//
// Created by lh on 2023/6/12.
//

#include "AudioChannel.h"


AudioChannel::AudioChannel(int channleId, AVCodecContext *avCodecContext,
                           CallJavaHelper *javaCallHelper, AVRational timeBase)
        : BaseChannel(channleId, avCodecContext, javaCallHelper, timeBase) {

    //根据布局获取声道数
    out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    out_samplesize = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    out_sample_rate = 44100;

    //out_sample_rate * 2 * 2 = out_sample_rate(采样率) * 双声道+ 16位(2个字节)
    data = static_cast<uint8_t *>(malloc(out_sample_rate * out_samplesize * out_channels));
    //给这段内存初始化
     memset(data, 0, out_sample_rate * out_channels * out_samplesize);
}

AudioChannel::~AudioChannel() {
    if (data) {
        free(data);
        data = nullptr;
    }
}

void *audio_decode(void *args) {
    auto *audioChannel = static_cast<AudioChannel *>(args);
    audioChannel->decode();
    return nullptr;
}

void *audio_play(void *args) {
    auto *audioChannel = static_cast<AudioChannel *>(args);
    audioChannel->_play();
    return nullptr;
}

void AudioChannel::play() {
    //构造函数里面已经设置了工作队列是播放状态
    //特别重要 特别重要 特别重要
    startWork();

    //重采样器
    swrContext = swr_alloc_set_opts(0, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, out_sample_rate,
                                    avCodecContext->channel_layout,
                                    avCodecContext->sample_fmt, avCodecContext->sample_rate, 0, 0);
    //必须需要初始化
    swr_init(swrContext);
    isPlaying = 1;
    //1. 解码
    pthread_create(&pid_audio_decode, 0, audio_decode, this);
    //2.播放
    pthread_create(&pid_audio_play, 0, audio_play, this);

}

void AudioChannel::stop() {

}

void AudioChannel::decode() {
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

// 回调函数this callback handler is called every time a buffer finishes playing
void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    auto *audioChannel = static_cast<AudioChannel *>(context);
    int dataSize = audioChannel->getPcm();

    if (dataSize > 0) {
        //表示有数据，此时进行播放 dataSize / 2表示传递的是16位的数据,因为采样位是16位的
        (*bq)->Enqueue(bq, audioChannel->data, dataSize/* / 2*/);
    }
}

/**
 * 播放音频
 */
void AudioChannel::_play() {
    SLresult result;

    //1. create engine 创建引擎
    result = slCreateEngine(&engineObject, 0, nullptr, 0, NULL, NULL);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //2. realize the engine  初始化引擎
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //3.获取调用接口
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }

    //4.创建混音器 create output mix, with environmental reverb specified as a non-required
    // interface
    /* const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
     const SLboolean req[1] = {SL_BOOLEAN_FALSE};*/
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, 0, 0);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }

    //5.初始化混音器 realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }

    // get the environmental reverb interface
    // this could fail if the environmental reverb effect is not available,
    // either because the feature is not present, excessive CPU load, or
    // the required MODIFY_AUDIO_SETTINGS permission was not requested and granted
    /* result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                               &outputMixEnvironmentalReverb);*/
    if (SL_RESULT_SUCCESS == result) {//todo 可以不需要，不是必须的步骤
        //6. 设置混音器效果
        /*result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);*/
    }

    //7.创建播放器
    // 7.1 配置输入声音信息 configure audio source
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    //pcm数据格式
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 2,
                                   SL_SAMPLINGRATE_44_1, SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                                   SL_BYTEORDER_LITTLEENDIAN};

    //7.2讲上述配置信息加入到数据源中
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    //7.3配置音轨(输出)
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, nullptr};

    /*
     * create audio player:
     *     fast audio does not support when SL_IID_EFFECTSEND is required, skip it
     *     for fast audio case
     */
    //7.4需要的接口
    const SLInterfaceID idss[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean reqq[1] = {SL_BOOLEAN_TRUE};

    //7.5 创建播放器
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk,1, idss, reqq);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }

    // 7.6 初始化播放器realize the player
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }

    // 7.7 获取播放器接口
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }

    //7.7 获取播放器回调接口 get the buffer queue interface
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE,
                                             &bqPlayerBufferQueue);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }

    //7.7 注册播放器回调接口 get the buffer queue interface register callback on the buffer queue
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, this);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }

    //7.8 设置为播放状态
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //7.9 手动激活回调函数,必须要手动激活
    bqPlayerCallback(bqPlayerBufferQueue, this);
}

int AudioChannel::getPcm() {
    int data_size = 0;
    AVFrame *frame;
    int ret = frame_queue.pop(frame);
    if (!isPlaying) {
        if (ret) {
            releaseAvFrame(&frame);
        }
        return data_size;
    }
    //需要重采样，因为播放器只能播放特定的数据格式数据，但是开发人员传递的参数不一定是规定的那些，为了保证播放的声音质量，所以需要重采样

    int64_t delays = swr_get_delay(swrContext, frame->sample_rate);
    //将nb_samples个数据由sample_rate采样率转成44100后返回多少数据，10个48000= nb个44100
    //AV_ROUND_UP 向上(相当于四舍入伍)
    //delays + frame->nb_samples 处理积压数据
    int64_t max_samples = av_rescale_rnd(delays + frame->nb_samples, out_sample_rate,
                                         frame->sample_rate,
                                         AV_ROUND_UP);
    //frame->data 输入数据，frame->nb_samples输入数据量,把转码后的数据存放在data里面,samples真正最后转化后的数据
    //samples单位是 out_sample_rate*2,这里的2表示的是声道数
    int samples = swr_convert(swrContext, &data, max_samples,
                              (const uint8_t **) (frame->data),
                              frame->nb_samples);
    //获得多少个有效的字节数据
    //data_size = samples /** out_sample_rate*/ * 2 * 2;
    data_size = samples * out_channels * out_samplesize;

    //获取一个相对播放的时间戳,获得相对播放这一段数据的秒数,相对开始播放
    clock = frame->pts * av_q2d(time_base);

    return data_size;
}
