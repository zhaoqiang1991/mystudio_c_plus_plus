package com.example.myapplication;

import android.util.Log;

public class JavaHelper {
  /*  public void getAddress() {
        Log.d("MainActivity", "=======getAddress=====");
    }


    public void showName() {
        Log.d("MainActivity", "=======showName=====");
    }*/


    public native void getNativeAddress();

    public native void showName(int age,String address);
}
