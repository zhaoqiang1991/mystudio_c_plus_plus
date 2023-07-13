package com.example.myapplication.opengl;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.RadioGroup;
import android.widget.Toast;

import com.blankj.utilcode.constant.PermissionConstants;
import com.blankj.utilcode.util.PermissionUtils;
import com.example.myapplication.R;
import com.example.myapplication.live.LiveActivity;
import com.example.myapplication.view.RecordButton;

public class GLActivity extends AppCompatActivity {

    private TigerView mTigerView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_glactivity);
        mTigerView = findViewById(R.id.tiger_view);
        RecordButton recordButton = findViewById(R.id.btn_record);
        recordButton.setOnRecordListener(new RecordButton.OnRecordListener() {
            /**
             * 开始录制
             */
            @Override
            public void onRecordStart() {

                if(!PermissionUtils.isGranted(PermissionConstants.STORAGE)){
                    PermissionUtils.permission(PermissionConstants.STORAGE)
                            .callback(new PermissionUtils.SimpleCallback() {
                                @Override
                                public void onGranted() {
                                    try {
                                        Toast.makeText(GLActivity.this, "写文件授权申请成功", Toast.LENGTH_SHORT).show();
                                        //申请成功后，可以调用相机拍摄/视频等操作
                                        mTigerView.startRecord();
                                    } catch (Exception ignored) {
                                        Toast.makeText(GLActivity.this, "相机授权申请报错", Toast.LENGTH_SHORT).show();
                                    }
                                }

                                @Override
                                public void onDenied() {
                                    //可以在此函数中调用 AlertDialog 提示用户相应操作等
                                    Toast.makeText(GLActivity.this, "相机授权申请否决", Toast.LENGTH_SHORT).show();
                                }
                            }).request();
                }else{
                    mTigerView.startRecord();
                }

            }

            /**
             * 停止录制
             */
            @Override
            public void onRecordStop() {
                Toast.makeText(GLActivity.this, "停止录制了", Toast.LENGTH_SHORT).show();
                mTigerView.stopRecord();
            }
        });
        RadioGroup radioGroup = findViewById(R.id.rg_speed);
        radioGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            /**
             * 选择录制模式
             * @param group
             * @param checkedId
             */
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                switch (checkedId) {
                    case R.id.rb_extra_slow: //极慢
                        mTigerView.setSpeed(TigerView.Speed.MODE_EXTRA_SLOW);
                        break;
                    case R.id.rb_slow:
                        mTigerView.setSpeed(TigerView.Speed.MODE_SLOW);
                        break;
                    case R.id.rb_normal:
                        mTigerView.setSpeed(TigerView.Speed.MODE_NORMAL);
                        break;
                    case R.id.rb_fast:
                        mTigerView.setSpeed(TigerView.Speed.MODE_FAST);
                        break;
                    case R.id.rb_extra_fast: //极快
                        mTigerView.setSpeed(TigerView.Speed.MODE_EXTRA_FAST);
                        break;
                }
            }
        });
    }
}