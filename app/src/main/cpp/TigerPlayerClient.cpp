

//
// Created by lh on 2023/5/30.
//
#ifdef ENABLE_JNI_ONLOAD

#include <jni.h>
#include "util.h"
#include "CallJavaHelper.h"
#include "TigerFFmpeg.h"

TigerFFmpeg *fFmpeg = 0;
JavaVM *_vm;
static const char *className = "com/example/myapplication/player/TigerPlayer";


/**
 * 本地方法
 */
static void player_native_prepare(JNIEnv *env, jobject thiz, jstring data_source) {
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
}

/**
 * start
 */
void player_native_start(JNIEnv *env, jobject thiz) {
    LOGD("======player_native_start 调用了");
    LOGD("================================================");
}

/**
 * start
 */
void player_native_stop(JNIEnv *env, jobject thiz) {
    LOGD("======native_stop 调用了");
    LOGD("================================================");
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
        {"native_prepare", "(Ljava/lang/String;)V", (void *) player_native_prepare},
        {"native_start",   "()V",                   (void *) player_native_start},
        {"native_stop",    "()V",                   (void *) player_native_stop},
        {"native_seek",    "(I)V",                  (void *) player_native_seek}

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
