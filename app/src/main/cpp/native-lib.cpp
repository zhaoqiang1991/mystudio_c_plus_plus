#include <jni.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <android/log.h>
#include "util.h"


using namespace std;

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_getAddress(JNIEnv *env, jobject thiz) {
    string address = "北京市朝阳区aaa望京西园";
    return env->NewStringUTF(address.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_shareSchoolInfo(JNIEnv *env, jobject thiz, jstring name,
                                                            jint age) {
    const jchar *jName = env->GetStringChars(name, 0);

    // LOGD("########## i = %d", jName);
    LOGD("########## i = %d", age);
    //__android_log_print(ANDROID_LOG_DEBUG,TAG ,"");
    return env->NewStringUTF("llll");;
}
