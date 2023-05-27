package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.example.myapplication.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getName();

    // Used to load the 'myapplication' library on application startup.
    static {
        System.loadLibrary("native");
    }

    private ActivityMainBinding binding;
    private TextView tv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

    }


    @Override
    protected void onResume() {
        super.onResume();

        tv = binding.sampleText;
        tv.setText(stringFromJNI());


        binding.sampleAddressTv.setText(getAddress());
       /* String info = shareSchoolInfo("布拉德", 29);
        Log.d(TAG,"=====info = "+ info);*/
    }

    public native String stringFromJNI();

    //java调用jni
    public native String getAddress();

   // public native void shareSchoolInfo(String name, int age);
}