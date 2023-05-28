#include <jni.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <android/log.h>
#include "util.h"


using namespace std;
jclass clazz;

JavaVM *_vm;

typedef struct {
    jobject instance;
} Context;

int JNI_OnLoad(JavaVM *vm, void *r) {
    LOGD("======JNI_OnLoad 调用了");
    _vm = vm;
    JNIEnv *jniEnv = 0;
    jint ref = vm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    if (ref != JNI_OK) {
        LOGD("======GetEnv失败了");
        return -1;
    }
    return JNI_VERSION_1_6;
}

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
    env->ReleaseStringUTFChars(name, className);
    jint score = 95;

    // jmethodID constructor = env->GetMethodID(clazz, "<init>", "()V");
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "(Ljava/lang/String;I)V");

    jobject studentObj = env->NewObject(clazz, constructor, newName, score);

    jmethodID methodId = env->GetMethodID(clazz, "setScore", "(I)V");
    env->CallVoidMethod(studentObj, methodId, 1000);

    jmethodID methodNameId = env->GetMethodID(clazz, "setName", "(Ljava/lang/String;)V");
    jstring result = env->NewStringUTF("华莱士爱索菲亚");
    env->CallVoidMethod(studentObj, methodNameId, result);

    //释放掉局部变量,可以不手动释放，会被自动释放
    env->DeleteLocalRef(result);
    return studentObj;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_setStudentInfo(JNIEnv *env, jobject thiz,
                                                           jobject student) {
    jclass jstudent = env->GetObjectClass(student);
    jmethodID jmethodId = env->GetMethodID(jstudent, "getScore", "()I");
    jint score = env->CallIntMethod(student, jmethodId);
    LOGD("======score = %d\n", score);

    jmethodID jmethodNameId = env->GetMethodID(jstudent, "getName", "()Ljava/lang/String;");
    jstring jstring1 = (jstring) env->CallObjectMethod(student, jmethodNameId);

    const char *result = env->GetStringUTFChars(jstring1, 0);
    LOGD("======result = %s\n", result);
}

/**
 * 局部引用在这个方法走完，那么就会在这个栈就会被回收，这个方法栈里面的
 * 所有资源都会被销毁，所以再次调用的时候就会报错,但是下面这种写法每次都
 * 会创建新的，所以不会报错，但是换种写法就会报错，在看接下来的方法
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_localRef(JNIEnv *env, jobject thiz, jstring s) {
    const char *name = env->GetStringUTFChars(s, 0);
    jclass clazz = env->FindClass(name);
    jstring newName = env->NewStringUTF("John Doe");

    jint score = 95;
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "(Ljava/lang/String;I)V");

    jobject studentObj = env->NewObject(clazz, constructor, newName, score);
    jmethodID methodId = env->GetMethodID(clazz, "setScore", "(I)V");
    env->CallVoidMethod(studentObj, methodId, 1000);
    LOGD("======调用了局部引用方法\n");
}


/**
 * 局部引用在这个方法走完，那么就会在这个栈就会被回收，这个方法栈里面的
 * 所有资源都会被销毁，所以再次调用的时候就会报错,根本原因就是
 * 指针有值，但是指针所指向的地址数据被释放了，相当于野指针，所以
 * 第二次调用就会crash
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_localRef2(JNIEnv *env, jobject thiz,
                                                      jstring method_name) {
    const char *name = env->GetStringUTFChars(method_name, 0);
    if (clazz == NULL) {
        clazz = env->FindClass(name);
    }
    jstring newName = env->NewStringUTF("John Doe");

    jint score = 95;
    jmethodID constructor = env->GetMethodID(clazz, "<init>", "(Ljava/lang/String;I)V");

    jobject studentObj = env->NewObject(clazz, constructor, newName, score);
    jmethodID methodId = env->GetMethodID(clazz, "setScore", "(I)V");
    env->CallVoidMethod(studentObj, methodId, 1000);
    LOGD("======调用了局部引用方法\n");
}


/**
 * 全局引用，即使出了方法栈，那么引用也不会被回收
 * 还有一个弱全局引用env->NewWeakGlobalRef()，这个不会阻止被回收，在使用的时候需要判断
 */
jclass studentClazz = 0;
extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_globalRef3(JNIEnv *env, jobject thiz,
                                                       jstring method_name) {
    const char *name = env->GetStringUTFChars(method_name, 0);

    if (studentClazz == NULL) {
        clazz = env->FindClass(name);
        //把class转化为全局引用
        studentClazz = static_cast<jclass>(env->NewGlobalRef(clazz));
    }
    jstring newName = env->NewStringUTF("John Doe");

    jint score = 95;
    jmethodID constructor = env->GetMethodID(studentClazz, "<init>", "(Ljava/lang/String;I)V");

    jobject studentObj = env->NewObject(studentClazz, constructor, newName, score);
    jmethodID methodId = env->GetMethodID(studentClazz, "setScore", "(I)V");
    env->CallVoidMethod(studentObj, methodId, 1000);
    env->DeleteGlobalRef(studentClazz);

    LOGD("======调用了局部引用方法\n");
}




extern "C"
void *updateUi(void *args) {
    JNIEnv *jniEnv = 0;
    //附加线程，把native线程附加到javaVm虚拟机
    jint status = _vm->AttachCurrentThread(&jniEnv, 0);
    if (status != JNI_OK) {
        //绑定线程失败
        LOGD("====绑定线程失败 !");
        return 0;
    }




    Context *context = static_cast<Context *>(args);

    jclass jclassobj = jniEnv->GetObjectClass(context->instance);
    jmethodID jmethodId = jniEnv->GetMethodID(jclassobj, "notifyUIRefresh", "()V");
    jniEnv->CallVoidMethod(context->instance, jmethodId);



    //分离
     _vm->DetachCurrentThread();
    delete context;
    context = 0;
    return 0;
}




extern "C"
JNIEXPORT void JNICALL
Java_com_example_myapplication_MainActivity_quarteringThread(JNIEnv *env, jobject thiz) {
    pthread_t pthread;
    Context *context = new Context();
    jobject jobject1 = env->NewGlobalRef(thiz);
    context->instance = jobject1;
    pthread_create(&pthread, 0, updateUi, context);

}