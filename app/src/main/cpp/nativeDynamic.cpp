#include <jni.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <android/log.h>
#include "util.h"

using namespace std;
JavaVM *_vm;

/**
 * 本地方法
 */
void getAddress(JNIEnv *env, jobject thiz) {
    LOGD("======getAddress 调用了");
}

/**
 * 动态注册方法表
 */
static const JNINativeMethod jniNativeMethod[] = {
        {"getNativeAddress", "()V", (void *) getAddress}
};

static const char *className = "com/example/myapplication/JavaHelper";

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
