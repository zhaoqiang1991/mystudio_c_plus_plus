

//
// Created by lh on 2023/5/30.
//
#ifdef ENABLE_JNI_ONLOAD

#include <jni.h>
#include "util.h"
#include "CallJavaHelper.h"
#include "TigerFFmpeg.h"
#include <android/native_window_jni.h>
#include <pthread.h>

JavaVM *javaVM = NULL;
TigerFFmpeg *ffmpeg = 0;
CallJavaHelper *javaCallHelper = 0;
ANativeWindow *window = 0;
pthread_mutex_t mutext = PTHREAD_MUTEX_INITIALIZER;


extern "C" {
#include <libavutil/imgutils.h>}

void renderFrame(uint8_t *data, int linesize, int w, int h) {
    pthread_mutex_lock(&mutext);
    if (!window) {
        pthread_mutex_unlock(&mutext);
        return;
    }
    //设置窗口属性
    ANativeWindow_setBuffersGeometry(window, w,
                                     h,
                                     WINDOW_FORMAT_RGBA_8888);

    ANativeWindow_Buffer window_buffer;
    if (ANativeWindow_lock(window, &window_buffer, 0)) {
        ANativeWindow_release(window);
        window = 0;
        pthread_mutex_unlock(&mutext);
        return;
    }
    uint8_t *dst_data = static_cast<uint8_t *>(window_buffer.bits);
    //一行需要多少像素 * 4(RGBA)
    int dst_linesize = window_buffer.stride * 4;
    uint8_t *src_data = data;
    int src_linesize = linesize;
    //一次拷贝一行
    for (int i = 0; i < window_buffer.height; ++i) {
        memcpy(dst_data + i * dst_linesize, src_data + i * src_linesize, dst_linesize);
    }
    ANativeWindow_unlockAndPost(window);
    pthread_mutex_unlock(&mutext);
}



JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    javaVM = vm;
//    av_log_set_level(AV_LOG_INFO);
//    av_log_set_callback(callback);
    return JNI_VERSION_1_4;
}


#endif
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_player_TigerPlayer_native_1start(JNIEnv *env, jobject thiz) {
    if (ffmpeg) {
        ffmpeg->start();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_player_TigerPlayer_native_1prepare(JNIEnv *env, jobject instance,
                                                                  jstring dataSource_) {

    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);
    javaCallHelper = new CallJavaHelper(javaVM, env, instance);
    ffmpeg = new TigerFFmpeg(javaCallHelper, dataSource);
    ffmpeg->setRenderCallback(renderFrame);
    ffmpeg->prepare();
    env->ReleaseStringUTFChars(dataSource_, dataSource);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_player_TigerPlayer_native_1set_1surface(JNIEnv *env, jobject thiz,
                                                                       jobject surface) {
    pthread_mutex_lock(&mutext);
    //先释放之前的显示窗口
    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
    //创建新的窗口用于视频显示
    window = ANativeWindow_fromSurface(env, surface);
    pthread_mutex_unlock(&mutext);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_player_TigerPlayer_native_1stop(JNIEnv *env, jobject thiz) {
    if (ffmpeg) {
        ffmpeg->stop();
        ffmpeg = 0;
    }
    if (javaCallHelper) {
        delete javaCallHelper;
        javaCallHelper = 0;
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_player_TigerPlayer_native_1release(JNIEnv *env, jobject thiz) {
    pthread_mutex_lock(&mutext);
    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
    pthread_mutex_unlock(&mutext);
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_myapplication_player_TigerPlayer_native_1getDuration(JNIEnv *env, jobject thiz) {
    if (ffmpeg) {
        return ffmpeg->getDuration();
    }
    return 0;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_player_TigerPlayer_native_1seek(JNIEnv *env, jobject thiz,
                                                               jint progress) {
    if (ffmpeg) {
        ffmpeg->seek(progress);
    }
}
}