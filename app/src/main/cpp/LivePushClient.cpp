//
// Created by lh on 2023/6/30.
//直播相关的代码，JIN部分
#ifdef ENABLE_LIVE_JNI_ONLOAD

#include <jni.h>
#include "util.h"
#include "librtmp/rtmp.h"
#include <x264.h>
#include "safe_queue.h"
#include "LiveVideoChannel.h"

static const char *className = "com/example/myapplication/live/LivePushClient";

pthread_t pid;

JavaVM *javaVM = NULL;
//发送给服务端的数据
SafeQueue<RTMPPacket *> packets;
LiveVideoChannel *videoChannel = NULL;
bool isStart = false;
int readyPushing = 0;
uint32_t start_time;


void releasePackets(RTMPPacket *&packet) {
    if (packet) {
        RTMPPacket_Free(packet);
        delete packet;
        packet = 0;
    }
}

void callback(RTMPPacket *packet) {
    if (packet) {
        //设置时间戳
        packet->m_nTimeStamp = RTMP_GetTime() - start_time;
        packets.push(packet);
    }
}

/**
 * start
 */
void live_native_init(JNIEnv *env, jobject thiz) {
    LOGD("======live_native_init 调用了");
    //准备编码，以及把编码后的数据放在一个队列里面，生产者，消费者模型，只有队列中有数据，就会
    //不断的取数据发给服务端,这种生产者消费者编程模型特别的重要
    videoChannel = new LiveVideoChannel();
    videoChannel->setVideoCallback(callback);
    packets.setReleaseHandle(releasePackets);
}

void live_native_push_video(JNIEnv *env, jobject thiz, jbyteArray data_) {

   /* if (!videoChannel || !readyPushing) {
        return;
    }*/
    LOGD("======live_native_push_video 调用了");
    jbyte *data = env->GetByteArrayElements(data_, NULL);
    videoChannel->encodeData(data);
    env->ReleaseByteArrayElements(data_, data, 0);
};

//设置编码器
void live_native_video_encInfo(JNIEnv *env, jobject thiz, jint w, jint h,
                               jint m_fps, jint m_bitrate) {
    LOGD("======live_native_video_encInfo 调用了");
    //委托给liveChannel来处理编码
    videoChannel->setVideoEncInfo(w, h, m_fps, m_bitrate);
};


void *start(void *args) {
    char *url = static_cast<char *>(args);
    RTMP *rtmp = NULL;
    LOGD("======rtmp start 调用了 url = %s",url);
    //写do while是为了方便退出循环，break
    do {
        rtmp = RTMP_Alloc();
        if (!rtmp) {
            LOGD("=======rtmp alloc RTMP创建失败");
            break;
        }
        //初始化
        RTMP_Init(rtmp);
        //链接网络超时时间5s
        rtmp->Link.timeout = 5;
        int ret = RTMP_SetupURL(rtmp, url);
        if (!ret) {
            LOGD("=======rtmp 地址失败 url = %s,", url);
            break;
        }
        //开启输出模式，因为是直播
        RTMP_EnableWrite(rtmp);
        ret = RTMP_Connect(rtmp, 0);
        if (!ret) {
            LOGD("=======rtmp链接服务器失败,请检查网络连接情况 url = %s", url);
            break;
        }
        ret = RTMP_ConnectStream(rtmp, 0);
        if (!ret) {
            LOGE("rtmp 连接流:%s", url);
            break;
        }
        //记录一个开始时间
        start_time = RTMP_GetTime();
        //表示可以开始推流了
        readyPushing = 1;
        packets.setWork(1);
        RTMPPacket *packet = 0;
        while (readyPushing) {
            packets.pop(packet);
            if (!isStart) {
                break;
            }
            if (!packet) {
                continue;
            }
            packet->m_nInfoField2 = rtmp->m_stream_id;
            //发送rtmp包 1：队列
            // 意外断网？发送失败，rtmpdump 内部会调用RTMP_Close
            // RTMP_Close 又会调用 RTMP_SendPacket
            // RTMP_SendPacket  又会调用 RTMP_Close
            // 将rtmp.c 里面WriteN方法的 Rtmp_Close注释掉
            ret = RTMP_SendPacket(rtmp, packet, 1);
            releasePackets(packet);
            if (!ret) {
                LOGE("rtmp 发送失败");
                break;
            }
        }
        releasePackets(packet);
    } while (0);

    isStart = false;
    readyPushing = 0;
    packets.setWork(0);
    packets.clear();
    if (rtmp) {
        RTMP_Close(rtmp);
        RTMP_Free(rtmp);
    }
    delete url;
    return 0;
}

//开始推流
void live_native_start(JNIEnv *env, jobject instance, jstring path_) {
    LOGD("======live_native_video_encInfo 调用了");
    const char *path = env->GetStringUTFChars(path_, 0);
    char *url = new char[strlen(path) + 1];
    if (isStart) {
        return;
    }
    strcpy(url, path);
    isStart = true;
    //启动线程，联网，开始使推流
    pthread_create(&pid, 0, start, url);

    env->ReleaseStringUTFChars(path_, path);
};

/**
 * stop
 */
void live_native_stop(JNIEnv *env, jobject thiz) {
    LOGD("======live_native_stop 调用了");

}

/**
 * release
 */
void live_native_release(JNIEnv *env, jobject thiz) {
    LOGD("======live_native_stop 调用了");

}

/**
 * 动态注册方法表
 * 第一个参数：java里面写的jni方法
 * 第二个参数：Java里面写的native方法对应的方法签名
 * 第三个参数:本地方法
 */
static const JNINativeMethod jniNativeMethod[] = {
        {"native_init",            "()V",                   (void *) live_native_init},
        {"native_pushVideo",       "([B)V",                 (void *) live_native_push_video},
        {"native_setVideoEncInfo", "(IIII)V",               (void *) live_native_video_encInfo},
        {"native_start",           "(Ljava/lang/String;)V", (void *) live_native_start},
        {"native_stop",            "()V",                   (void *) live_native_stop},
        {"native_release",         "()V",                   (void *) live_native_release}
};

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
    jclass clazz = jniEnv->FindClass(className);

    jniEnv->RegisterNatives(clazz, jniNativeMethod,
                            sizeof(jniNativeMethod) / sizeof(JNINativeMethod));
    return JNI_VERSION_1_4;
}

#endif


