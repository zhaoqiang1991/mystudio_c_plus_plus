package com.example.myapplication;

import android.content.Context;
import android.util.Log;

/**
 * 这里面报错的 native方法需要在CMakelist中把ENABLE_DYNAMIC_JNI_ONLOAD
 * 这个宏开关打开就可以了
 */
public class JavaHelper {
    public native void getNativeAddress();

    public native void showName(int age,String address);

    public native void testThread();

    public void notifyUIRefresh(){

    }
}
