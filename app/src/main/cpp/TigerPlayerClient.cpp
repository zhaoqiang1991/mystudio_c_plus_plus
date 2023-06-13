

//
// Created by lh on 2023/5/30.
//
#ifdef ENABLE_JNI_ONLOAD

#include <jni.h>
#include "util.h"
#include "CallJavaHelper.h"
#include "TigerFFmpeg.h"
#include <android/native_window_jni.h>

TigerFFmpeg *fFmpeg = nullptr;
JavaVM *_vm;
ANativeWindow *window = nullptr;
//静态初始化锁,成员变量，和application的生命周期一样长
pthread_mutex_t mutex_thread = PTHREAD_MUTEX_INITIALIZER;

static const char *className = "com/example/myapplication/player/TigerPlayer";


//开始进行画画
void renderAVframe(uint8_t *data, int linesize, int w, int h) {
    pthread_mutex_lock(&mutex_thread);
    if (!window) {
        pthread_mutex_unlock(&mutex_thread);
        return;
    }
    //设置窗口属性
    ANativeWindow_setBuffersGeometry(window, w, h, WINDOW_FORMAT_RGBA_8888);
    //ANativeWindow_Buffer *windowBuffer;
    ANativeWindow_Buffer windowBuffer;
    if (ANativeWindow_lock(window, &windowBuffer, 0)) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    //填充rgb数据给dst_data
    uint8_t *dst_data = static_cast<uint8_t *>(windowBuffer.bits);
    //stride 表示一行有多少个数据(RGBA)， dis_linesize转化为字节
    int dis_linesize = windowBuffer.stride * 4;

    //一行一行的拷贝数据
    for (int i = 0; i < windowBuffer.height; ++i) {
        memcpy(dst_data + i * dis_linesize, data + i * dis_linesize, dis_linesize);
    }

    ANativeWindow_unlockAndPost(window);
    pthread_mutex_unlock(&mutex_thread);
}

/**
 * 本地方法
 */
static void player_native_prepare(JNIEnv *env, jobject thiz, jstring data_source) {
    LOGD("======native_prepare 调用了");
    LOGD("================================================");
    const char *dataSource = env->GetStringUTFChars(data_source, 0);
    CallJavaHelper *callJavaHelper = new CallJavaHelper(_vm, env, &thiz);
    fFmpeg = new TigerFFmpeg(callJavaHelper, dataSource);
    //设置渲染器回调
    fFmpeg->setRenderFrameCallback(renderAVframe);
    //开始准备
    fFmpeg->prepare();

    //回收资源
    env->ReleaseStringUTFChars(data_source, dataSource);
    LOGD("url = %s\n", dataSource);
}

/**
 * start
 */
void player_native_start(JNIEnv *env, jobject thiz) {
    LOGD("======player_native_start 调用了");
    LOGD("================================================");
    fFmpeg->start();
}

/**
 * start
 */
void player_native_stop(JNIEnv *env, jobject thiz) {
    LOGD("======native_stop 调用了");
    LOGD("================================================");
}

/**
 * player_setSurface  设置给native的Surface画布，来进行画画，渲染视频数据
 */
void player_setSurface(JNIEnv *env, jobject thiz, jobject surface) {
    pthread_mutex_lock(&mutex_thread);
    LOGD("======player_setSurface 调用了");
    LOGD("================================================");
    if (window) {
        //如果已经有了，那么就需要释放掉
        ANativeWindow_release(window);
        window = nullptr;
    }
    window = ANativeWindow_fromSurface(env, surface);
    //需要设置窗口属性  ，图像在内存中的排列情况
    pthread_mutex_unlock(&mutex_thread);

}


/**
 * start
 */
void player_native_seek(JNIEnv *env, jobject thiz, jint progress) {
    LOGD("======native_seek 调用了");
    LOGD("================================================");
    LOGD("progress = %d\n", progress);
}

/**
 * 动态注册方法表
 * 第一个参数：java里面写的jni方法
 * 第二个参数：Java里面写的native方法对应的方法签名
 * 第三个参数:本地方法
 */
static const JNINativeMethod jniNativeMethod[] = {
        {"native_prepare",    "(Ljava/lang/String;)V",     (void *) player_native_prepare},
        {"native_start",      "()V",                       (void *) player_native_start},
        {"native_stop",       "()V",                       (void *) player_native_stop},
        {"native_seek",       "(I)V",                      (void *) player_native_seek},
        {"native_setSurface", "(Landroid/view/Surface;)V", (void *) player_setSurface}

};


int JNI_OnLoad(JavaVM *vm, void *r) {
    LOGD("======JNI_OnLoad 调用了");
    _vm = vm;
    JNIEnv *jniEnv = 0;
    jint ref = vm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    if (ref != JNI_OK) {
        LOGD("======GetEnv失败了");
        return -1;
    }

    jclass clazz = jniEnv->FindClass(className);

    jniEnv->RegisterNatives(clazz, jniNativeMethod,
                            sizeof(jniNativeMethod) / sizeof(JNINativeMethod));
    return JNI_VERSION_1_6;
}


#endif


/*
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_player_TigerPlayer_native_1prepare(JNIEnv *env, jobject thiz,
                                                                  jstring data_source) {
    LOGD("======native_prepare 调用了");
    LOGD("================================================");
    const char *dataSource = env->GetStringUTFChars(data_source, 0);
    CallJavaHelper *callJavaHelper = new CallJavaHelper(_vm, env, &thiz);
    fFmpeg = new TigerFFmpeg(callJavaHelper, dataSource);
    //todo 设置渲染器
    // fFmpeg.setReaderCallBack()
    //开始准备
    fFmpeg->prepare();

    //回收资源
    env->ReleaseStringUTFChars(data_source, dataSource);
    LOGD("url = %s\n", dataSource);
}*/
