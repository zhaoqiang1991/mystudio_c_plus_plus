//
// Created by lh on 2023/5/30.
//

#ifndef MY_APPLICATION_CALLJAVAHELPER_H
#define MY_APPLICATION_CALLJAVAHELPER_H



#include <jni.h>
#include <iostream>
#include "util.h"
/**
 * c/c++语言的规则都是先声明后实现方法
 */
class CallJavaHelper {
public:
    CallJavaHelper(JavaVM *vm, JNIEnv *env, const jobject &jobj);

    virtual ~CallJavaHelper();


    void onError(int threadId, jstring errorDesc);

    void onProgress(int threadId,int progress);

    void onPrepare(int threadId);


public:
    JavaVM *_vm;
    JNIEnv *env;

    jobject jobj;
    jmethodID jmid_onerror;
    jmethodID jmid_onprogress;
    jmethodID jmid_onprepare;

};


#endif //MY_APPLICATION_CALLJAVAHELPER_H
