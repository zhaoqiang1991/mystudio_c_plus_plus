//
// Created by lh on 2023/6/30.
//直播相关的代码，JIN部分
#ifdef ENABLE_LIVE_JNI_ONLOAD
#include <jni.h>
#include "util.h"
#include "librtmp/rtmp.h"
#include <x264.h>

JavaVM *javaVM = NULL;

int JNI_OnLoad(JavaVM *vm, void *r) {
    LOGD("======JNI_OnLoad 调用了");
    javaVM = vm;
    JNIEnv *jniEnv = 0;
    jint ref = vm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    if (ref != JNI_OK) {
        LOGD("======GetEnv失败了");
        return -1;
    }

    RTMP_Alloc();
    x264_picture_t *p = new x264_picture_t;
    p->b_keyframe;
    /*jclass clazz = jniEnv->FindClass(className);

    jniEnv->RegisterNatives(clazz, jniNativeMethod,
                            sizeof(jniNativeMethod) / sizeof(JNINativeMethod));*/
    return JNI_VERSION_1_4;
}

#endif
