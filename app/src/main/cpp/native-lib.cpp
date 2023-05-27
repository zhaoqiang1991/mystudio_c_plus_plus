#include <jni.h>
#include <string>
#include <iostream>
#include <pthread.h>

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

/*
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_shareSchoolInfo(JNIEnv *env, jobject thiz, jstring name,
                                                            jint age) {
    // TODO: implement shareSchoolInfo()
}*/
