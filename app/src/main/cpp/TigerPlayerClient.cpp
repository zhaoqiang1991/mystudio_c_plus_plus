

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

static const char *className = "com/example/myapplication/player/TigerPlayer";

extern "C" {
#include <libavutil/imgutils.h>
}

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


/**
 * 本地方法
 */
static void player_native_prepare(JNIEnv *env, jobject instance, jstring dataSource_) {
    const char *dataSource = env->GetStringUTFChars(dataSource_, 0);
    javaCallHelper = new CallJavaHelper(javaVM, env, instance);
    ffmpeg = new TigerFFmpeg(javaCallHelper, dataSource);
    ffmpeg->setRenderCallback(renderFrame);
    ffmpeg->prepare();
    env->ReleaseStringUTFChars(dataSource_, dataSource);
}

/**
 * start
 */
void player_native_start(JNIEnv *env, jobject thiz) {
    LOGD("======player_native_start 调用了");
    LOGD("================================================");
    if (ffmpeg) {
        ffmpeg->start();
    }
}

/**
 * start
 */
void player_native_stop(JNIEnv *env, jobject thiz) {
    if (ffmpeg) {
        ffmpeg->stop();
        ffmpeg = 0;
    }
    if (javaCallHelper) {
        delete javaCallHelper;
        javaCallHelper = 0;
    }
}

/**
 * player_setSurface  设置给native的Surface画布，来进行画画，渲染视频数据
 */
void player_setSurface(JNIEnv *env, jobject thiz, jobject surface) {
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


/**
 * start
 */
void player_native_seek(JNIEnv *env, jobject thiz, jint progress) {
    LOGD("======native_seek 调用了");
    LOGD("================================================");
    if (ffmpeg){
        ffmpeg->seek(progress);
    }
}

void player_release(JNIEnv *env, jobject thize) {
    pthread_mutex_lock(&mutext);
    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
    pthread_mutex_unlock(&mutext);
}

int player_getDuration(JNIEnv *env, jobject thize) {
    if (ffmpeg) {
        return ffmpeg->getDuration();
    }
    return 0;
}

/**
 * 动态注册方法表
 * 第一个参数：java里面写的jni方法
 * 第二个参数：Java里面写的native方法对应的方法签名
 * 第三个参数:本地方法
 */
static const JNINativeMethod jniNativeMethod[] = {
        {"native_prepare",     "(Ljava/lang/String;)V",     (void *) player_native_prepare},
        {"native_start",       "()V",                       (void *) player_native_start},
        {"native_stop",        "()V",                       (void *) player_native_stop},
        {"native_seek",        "(I)V",                      (void *) player_native_seek},
        {"native_setSurface",  "(Landroid/view/Surface;)V", (void *) player_setSurface},
        {"native_release",     "()V",                       (void *) player_release},
        {"native_getDuration", "()I",                       (void *) player_getDuration}


};


int JNI_OnLoad(JavaVM *vm, void *r) {
    LOGD("======JNI_OnLoad 调用了");
    javaVM = vm;
    JNIEnv *jniEnv = 0;
    jint ref = vm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    if (ref != JNI_OK) {
        LOGD("======GetEnv失败了");
        return -1;
    }

    jclass clazz = jniEnv->FindClass(className);

    jniEnv->RegisterNatives(clazz, jniNativeMethod,
                            sizeof(jniNativeMethod) / sizeof(JNINativeMethod));
    return JNI_VERSION_1_4;
}

#endif