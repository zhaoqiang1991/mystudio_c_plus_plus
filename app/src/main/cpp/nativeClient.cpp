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
    const char *result = env->GetStringUTFChars(name, 0);
    LOGD("########## i = %s", result);
    LOGD("########## i = %d", age);
    //回收
    env->ReleaseStringUTFChars(name, result);
    return env->NewStringUTF(result);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_myapplication_MainActivity_getStudentInfo(JNIEnv *env, jobject thiz,
                                                           jstring name) {

    const char *className = env->GetStringUTFChars(name, 0);
   // jclass clazz = env->FindClass("com/example/myapplication/bean/Student");
    jclass clazz = env->FindClass(className);

    /* jmethodID getName = env->GetMethodID(clazz, "getName", " ()Ljava/lang/String");

     jmethodID getName = env->GetMethodID(clazz, "getScore", " getScore()");*/

    // 创建字符串对象并设置值
    jstring newName = env->NewStringUTF("John Doe");
    jint score = 95;

   // jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "(Ljava/lang/String;I)V");

    jobject studentObj = env->NewObject(clazz, constructor, newName, score);

    return studentObj;
}