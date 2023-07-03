//
// Created by lh on 2023/7/2.
//

#include <cstdio>
#include "LiveAudioChannel.h"


LiveAudioChannel::LiveAudioChannel() {}

LiveAudioChannel::~LiveAudioChannel() {
    DELETE(buffer);
    //释放编码器
    if (audioCodec) {
        faacEncClose(audioCodec);
        audioCodec = 0;
    }
}

void LiveAudioChannel::setAudioEncInfo(int sampleRateInHz, int channels) {
    mChannels = channels;
    //编码器
    audioCodec = faacEncOpen(sampleRateInHz, channels, &inputSamples, &maxOutputBytes);
    faacEncConfigurationPtr config = faacEncGetCurrentConfiguration(audioCodec);
    //指定为 mpeg4 标准
    config->mpegVersion = MPEG4;
    //lc 标准
    config->aacObjectType = LOW;
    //16位
    config->inputFormat = FAAC_INPUT_16BIT;
    // 编码出原始数据 既不是adts也不是adif
    config->outputFormat = 0;
    //设置配置
    faacEncSetConfiguration(audioCodec, config);
    //输出缓冲区 编码后的数据 用这个缓冲区来保存
    buffer = new u_char[maxOutputBytes];
}

void LiveAudioChannel::setAudioCallback(LiveAudioChannel::AudioCallback audioCallback) {
    this->audioCallback = audioCallback;
}

/**
 * 返回出inputSamples给外界
 * @return
 */
int LiveAudioChannel::getInputSamples() {
    return inputSamples;
}

void LiveAudioChannel::encodeData(int8_t *data) {
    //开始编码,返回编码后字节的长度
    int bytelen = faacEncEncode(audioCodec, reinterpret_cast<int32_t *>(data), inputSamples, buffer,
                                maxOutputBytes);
    if (bytelen > 0) {
        //看表
        int bodySize = 2 + bytelen;
        RTMPPacket *packet = new RTMPPacket;
        RTMPPacket_Alloc(packet, bodySize);
        //双声道
        packet->m_body[0] = 0xAF;
        if (mChannels == 1) {
            packet->m_body[0] = 0xAE;
        }
        //编码出的声音 都是 0x01
        packet->m_body[1] = 0x01;
        //图片数据
        memcpy(&packet->m_body[2], buffer, bytelen);

        packet->m_hasAbsTimestamp = 0;
        packet->m_nBodySize = bodySize;
        packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
        packet->m_nChannel = 0x11;
        packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
        audioCallback(packet);
    }
}

RTMPPacket *LiveAudioChannel::getAudioTag() {
    u_char *buf;
    u_long len;
    faacEncGetDecoderSpecificInfo(audioCodec, &buf, &len);
    int bodySize = 2 + len;
    RTMPPacket *packet = new RTMPPacket;
    RTMPPacket_Alloc(packet, bodySize);
    //双声道
    packet->m_body[0] = 0xAF;
    if (mChannels == 1) {
        packet->m_body[0] = 0xAE;
    }
    packet->m_body[1] = 0x00;
    //图片数据
    memcpy(&packet->m_body[2], buf, len);

    packet->m_hasAbsTimestamp = 0;
    packet->m_nBodySize = bodySize;
    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nChannel = 0x11;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    return packet;
}


