#include <jni.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <android/log.h>
#include "util.h"

#ifdef ENABLE_DYNAMIC_JNI_ONLOAD

using namespace std;
JavaVM *_vm;

typedef struct {
    jobject instance;
} Context;

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
 * JNIEnv是和线程有关系的，不能夸线程调用，想要夸线程调动
 * 需要使用JavaVm才可以
 * 如果使用JNIEnv垮线程调用，就会报下面的信息，例如
 * native: #00 pc 000000000001f3ec  /system/lib64/libc.so (syscall+28)
2023-05-28 16:04:26.870 17907-17933 e.myapplicatio          com.example.myapplication            A  runtime.cc:637]   native: #01 pc 00000000000d8788  /system/lib64/libart.so (art::ConditionVariable::WaitHoldingLocks(art::Thread*)+148)
2023-05-28 16:04:26.870 17907-17933 e.myapplicatio          com.example.myapplication            A  runtime.cc:637]   native: #02 pc 00000000003bf3f4  /system/lib64/libart.so (art::Monitor::Wait(art::Thread*, long, int, bool, art::ThreadState)+876)
2023-05-28 16:04:26.870 17907-17933 e.myapplicatio          com.example.myapplication            A  runtime.cc:637]   native: #03 pc 00000000003c10e8  /system/lib64/libart.so (art::Monitor::Wait(art::Thread*, art::mirror::Object*, long, int, bool, art::ThreadState)+424)
 * @param args
 * @return
 */
void *updateUi(void *args) {
    JNIEnv *jniEnv = static_cast<JNIEnv *>(args);
    jstring clazzName = jniEnv->NewStringUTF("com/example/myapplication/JavaHelper");
    const char *name = jniEnv->GetStringUTFChars(clazzName, 0);

    jclass jclass1 = jniEnv->FindClass(name);
    jmethodID jmethodId = jniEnv->GetMethodID(jclass1, "notifyUIRefresh", "()V");
    jniEnv->CallVoidMethod(jclass1, jmethodId);
}

void testNativeThread(JNIEnv *env, jobject thiz) {

    pthread_t pthread;
    pthread_create(&pthread, 0, updateUi, env);
}

void testQuarteringThread(JNIEnv *env, jobject thiz) {
    //跨线程调用，让JNIEnv跨线程
    jint status = _vm->AttachCurrentThread(&env, 0);
    if(status !=JNI_OK){
        //绑定线程失败
        LOGD("====绑定线程失败 !");
        return;
    }

}

/**
 * 动态注册方法表
 * 第一个参数：java里面写的jni方法
 * 第二个参数：Java里面写的native方法对应的方法签名
 * 第三个参数:本地方法
 */
static const JNINativeMethod jniNativeMethod[] = {
        {"getNativeAddress", "()V",                    (void *) getAddress},
        {"showName",         "(ILjava/lang/String;)V", (void *) showNativename},
        {"testThread",       "()V",                    (void *) testNativeThread}

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
#endif
