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
 * 本地方法
 */
void showNativename(JNIEnv *env, jobject thiz, jint age, jstring address) {
    LOGD("======getAddress 调用了");
    LOGD("================================================");
    LOGD("age= %d ", age);

    const char *jAddress = env->GetStringUTFChars(address, 0);
    LOGD("address = %s\n", jAddress);
}

/**
 * 动态注册方法表
 * 第一个参数：java里面写的jni方法
 * 第二个参数：Java里面写的native方法对应的方法签名
 * 第三个参数:本地方法
 */
static const JNINativeMethod jniNativeMethod[] = {
        {"getNativeAddress", "()V", (void *) getAddress},
        {"showName", "(ILjava/lang/String;)V", (void *) showNativename}
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
