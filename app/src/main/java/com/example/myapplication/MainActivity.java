package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.example.myapplication.bean.Student;
import com.example.myapplication.databinding.ActivityMainBinding;
import com.example.myapplication.live.LiveActivity;
import com.example.myapplication.player.PlayerActivity;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getName();

    static {
        System.loadLibrary("tigerPlayer");
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
       /* methond1();
        method2();
        method3();
        method4();*/
//        method5();
//        method5();

       /* method6();
        method6();*/
       /* method7();
        method7();*/
        // method8();
        // method9();
        //method10();
        // method11();
        method12();
    }

    private void method12() {
        binding.startPlayerTv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, PlayerActivity.class);
                startActivity(intent);
            }
        });

        binding.startLiveTv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //开始直播
                Intent intent = new Intent(MainActivity.this, LiveActivity.class);
                startActivity(intent);
            }
        });


    }

    private void method11() {
        quarteringThread();
    }


    private void method10() {
        JavaHelper javaHelper = new JavaHelper();
        // javaHelper.testThread();
    }

    private void method9() {
        JavaHelper javaHelper = new JavaHelper();
        // javaHelper.showName(90, "爱迪生");
    }

    /**
     * 测试动态注册
     */
    private void method8() {
        JavaHelper javaHelper = new JavaHelper();
        //javaHelper.getNativeAddress();
    }

    private void method7() {
        globalRef3("com/example/myapplication/bean/Student");
    }

    private void method5() {
        localRef("com/example/myapplication/bean/Student");
    }

    private void method6() {
        localRef2("com/example/myapplication/bean/Student");
    }


    private void method4() {
        setStudentInfo(new Student("法拉第", 9999));
    }

    private void method3() {
        Student studentInfo = getStudentInfo("com/example/myapplication/bean/Student");
        Log.d(TAG, "=====getStudentInfo = " + studentInfo.toString());
    }

    private void method2() {
        binding.sampleAddressTv.setText(getAddress());
        String info = shareSchoolInfo("布拉德皮特", 29);
        Log.d(TAG, "=====info = " + info);
        Log.d(TAG, "============================================\n\n ");
    }

    private void methond1() {
        tv = binding.sampleText;
        tv.setText(stringFromJNI());
    }

    public void notifyUIRefresh() {
        if (Looper.myLooper() == Looper.getMainLooper()) {
            //在主线程
            Toast.makeText(MainActivity.this, "主线程中更新UI", Toast.LENGTH_LONG).show();
        } else {
            //子线程
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Log.d(TAG, "====Java层当前线程的名字 = " + Thread.currentThread().getName());
                    Toast.makeText(MainActivity.this, "子线程中更新UI", Toast.LENGTH_LONG).show();
                }
            });
        }
        ;
    }

    public native String stringFromJNI();

    //java调用jni
    public native String getAddress();

    public native String shareSchoolInfo(String name, int age);

    public native Student getStudentInfo(String name);


    public native void setStudentInfo(Student student);

    /**
     * 局部引用测试
     *
     * @param s
     */
    public native void localRef(String s);

    /**
     * 局部引用测试
     *
     * @param methodName
     */
    public native void localRef2(String methodName);

    public native void globalRef3(String method_name);

    /**
     * 测试夸线程调用
     */
    public native void quarteringThread();
}