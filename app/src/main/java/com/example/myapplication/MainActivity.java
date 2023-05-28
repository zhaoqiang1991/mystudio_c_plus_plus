package com.example.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.example.myapplication.bean.Student;
import com.example.myapplication.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getName();

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
        method9();
    }

    private void method9() {
        JavaHelper javaHelper = new JavaHelper();
        javaHelper.showName(90,"爱迪生");
    }

    /**
     * 测试动态注册
     */
    private void method8() {
        JavaHelper javaHelper = new JavaHelper();
        javaHelper.getNativeAddress();
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
}