//
// Created by lh on 2023/5/27.
//

#ifndef MY_APPLICATION_UTIL_H
#define MY_APPLICATION_UTIL_H

/**
 * 在Android studio里面创建ndk工程，单独创建的.h文件必须要在别的使用引用，比如
 * #include "util.h"，这样子，不然当前创建的.h文件里面导入的头文件就会报错
 * 可以使用这种.h 文件来定义一些工具类什么的
 */
#include <android/log.h>

#define TAG "FFMPEG" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型


#define DELETE(obj) if(obj){delete obi;obj = 0;}


//标记线程 因为子线程需要attach
#define THREAD_MAIN 1
#define THREAD_CHILD 2

//错误代码
//打不开视频
#define FFMPEG_CAN_NOT_OPEN_URL 1
//找不到流媒体
#define FFMPEG_CAN_NOT_FIND_STREAMS 2
//找不到解码器
#define FFMPEG_FIND_DECODER_FAIL 3
//无法根据解码器创建上下文
#define FFMPEG_ALLOC_CODEC_CONTEXT_FAIL 4
//根据流信息 配置上下文参数失败
#define FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL 6
//打开解码器失败
#define FFMPEG_OPEN_DECODER_FAIL 7
//没有音视频
#define FFMPEG_NOMEDIA 8

#endif //MY_APPLICATION_UTIL_H
