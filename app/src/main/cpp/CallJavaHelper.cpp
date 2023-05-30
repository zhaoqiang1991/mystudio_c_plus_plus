//
// Created by lh on 2023/5/30.
//

#include "CallJavaHelper.h"

/**
 * 做一些初始化工作
 * c++的语法，给public类型的属性赋值
 * @param vm
 * @param env
 * @param _jobj
 */
CallJavaHelper::CallJavaHelper(JavaVM *vm, JNIEnv *env, const jobject *_jobj) : _vm(vm), env(env) {
    //之所以使用全局的是因为整个播放的过程都需要，如果是局部的话，会在方法栈调用完毕被回收
    jobj = env->NewGlobalRef(*_jobj);
    jclass clzz = env->GetObjectClass(jobj);

    this->jmid_onprepare = env->GetMethodID(clzz, "onPrepared", "()V");
    this->jmid_onerror = env->GetMethodID(clzz, "onError", "(ILjava/lang/String;)V");
    this->jmid_onprogress = env->GetMethodID(clzz, "onProgress", "(I)V");
}

CallJavaHelper::~CallJavaHelper() {
    //回收资源
    this->env->DeleteGlobalRef(this->jobj);
    this->jobj = nullptr;

}

void CallJavaHelper::onError(int threadId, string errorDesc) {
    if (threadId == THREAD_CHILD) {
        //子线程,必须切把navite线程挂在到javavm线程，因为JNIEnv不能跨线程调用
        jint status = _vm->AttachCurrentThread(&this->env, 0);
        if (status > 0) {
            LOGD("onError errorDesc = %s\n", errorDesc.c_str());
            return;
        }
        this->env->CallVoidMethod(this->jobj, this->jmid_onerror);
        _vm->DetachCurrentThread();
    } else {
        this->env->CallVoidMethod(this->jobj, this->jmid_onerror);
    }
}

void CallJavaHelper::onProgress(int threadId, int progress) {
    if (threadId == THREAD_CHILD) {
        //子线程
        jint status = _vm->AttachCurrentThread(&this->env, 0);
        if (status > 0) {
            LOGD("onProgress progress = %d\n", progress);
            return;
        }
        this->env->CallVoidMethod(this->jobj, this->jmid_onprogress);
        _vm->DetachCurrentThread();
    } else {
        //主线程
        this->env->CallVoidMethod(this->jobj, this->jmid_onprogress);
    }
}

void CallJavaHelper::onPrepare(int threadId) {
    if (threadId == THREAD_CHILD) {
        //子线程
        jint status = _vm->AttachCurrentThread(&this->env, 0);
        if (status > 0) {
            LOGD("onProgress progress \n");
            return;
        }
        this->env->CallVoidMethod(this->jobj, this->jmid_onprepare);
        _vm->DetachCurrentThread();
    } else {
        //主线程
        this->env->CallVoidMethod(this->jobj, this->jmid_onprepare);
    }
}
