//
// Created by lh on 2023/5/30.
//

#include "TigerFFmpeg.h"
#include "util.h"


TigerFFmpeg::TigerFFmpeg(CallJavaHelper *callJavaHelper, char *dataSource) : callJavaHelper(
        callJavaHelper), data_source(dataSource) {
    this->data_source = new char[strlen(dataSource) + 1];
    stpcpy(this->data_source, dataSource);
    isPlaying = 0;
    this->duration = 0;
    this->isSeek = 0;//todo
    pthread_mutex_init(&this->seekLock, nullptr);
}

TigerFFmpeg::~TigerFFmpeg() {
    pthread_mutex_destroy(&this->seekLock);
    this->callJavaHelper = nullptr;
    DELETE(this->data_source);
}

void TigerFFmpeg::prepare() {

}

void TigerFFmpeg::prepareFFmpeg() {

}

void TigerFFmpeg::start() {

}

void TigerFFmpeg::stop() {

}

void TigerFFmpeg::seek(int progress) {

}
