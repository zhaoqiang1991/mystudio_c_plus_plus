package com.example.myapplication.live;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;

import com.blankj.utilcode.constant.PermissionConstants;
import com.blankj.utilcode.util.PermissionUtils;
import com.example.myapplication.MainActivity;
import com.example.myapplication.R;

public class LiveActivity extends AppCompatActivity {

    private SurfaceView mSurfaceView;
    private LivePushClient livePusher;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_live);
        mSurfaceView = this.findViewById(R.id.surfaceView);
        livePusher = new LivePushClient(this, 800, 480, 800_000, 10, Camera.CameraInfo.CAMERA_FACING_BACK);
        //  设置摄像头预览的界面
        livePusher.setPreviewDisplay(mSurfaceView.getHolder());



    }

    private void startPreviewWithPermission() {
        //权限申请
        //PermissionUtils工具包权限类型:CAMERA相机
        //callback回调监听授权或拒绝 onGranted 授权事件  onDenied否绝事件
        PermissionUtils.permission(PermissionConstants.CAMERA)
                .callback(new PermissionUtils.SimpleCallback() {
                    @Override
                    public void onGranted() {
                        try {
                            Toast.makeText(LiveActivity.this, "相机授权申请成功", Toast.LENGTH_SHORT).show();
                            //申请成功后，可以调用相机拍摄/视频等操作
                            livePusher.startLive("rtmp://192.168.0.160/myapp/mystream");
                        } catch (Exception ignored) {
                            Toast.makeText(LiveActivity.this, "相机授权申请报错", Toast.LENGTH_SHORT).show();
                        }
                    }

                    @Override
                    public void onDenied() {
                        //可以在此函数中调用 AlertDialog 提示用户相应操作等
                        Toast.makeText(LiveActivity.this, "相机授权申请否决", Toast.LENGTH_SHORT).show();
                    }
                }).request();
    }


    public void switchCamera(View view) {
        livePusher.switchCamera();
    }

    public void startLive(View view) {
        startPreviewWithPermission();

    }

    public void stopLive(View view) {
        livePusher.stopLive();
    }
}