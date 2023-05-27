//
// Created by lh on 2023/5/27.
//

#ifndef MY_APPLICATION_UTIL_H
#define MY_APPLICATION_UTIL_H

/**
 * 在Android studio里面创建ndk工程，单独创建的.h文件必须要在别的使用引用，比如
 * #include "util.h"，这样子，不然当前创建的.h文件里面导入的头文件就会报错
 */
#include <android/log.h>
#define TAG "FFMPEG" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型
#endif //MY_APPLICATION_UTIL_H
