//
// Created by lh on 2023/7/2.
//

#ifndef MY_APPLICATION_LIVEAUDIOCHANNEL_H
#define MY_APPLICATION_LIVEAUDIOCHANNEL_H

#include "faac.h"
#include "librtmp/rtmp.h"
#include "util.h"
#include <string.h>


class LiveAudioChannel {
    typedef void (*AudioCallback)(RTMPPacket *packet);

public:
    LiveAudioChannel();

    virtual ~LiveAudioChannel();

    void setAudioEncInfo(int sampleRateInHz, int channels);

    void setAudioCallback(AudioCallback audioCallback);

    int getInputSamples();

    void encodeData(int8_t *data);

    RTMPPacket* getAudioTag();

private:
    AudioCallback audioCallback;
    int mChannels;

    //一次输入样本，简要编码的数据个数
    unsigned long inputSamples;
    //输出编码的数据个数
    unsigned long maxOutputBytes;

    faacEncHandle audioCodec = 0;
    u_char *buffer = 0;
};


#endif //MY_APPLICATION_LIVEAUDIOCHANNEL_H
