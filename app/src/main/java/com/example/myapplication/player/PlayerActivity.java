package com.example.myapplication.player;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;

import com.example.myapplication.R;
import com.example.myapplication.listener.OnErrorListener;
import com.example.myapplication.listener.OnPrepareListener;
import com.example.myapplication.listener.OnProgressListener;

public class PlayerActivity extends AppCompatActivity {
    private static final String TAG = PlayerActivity.class.getSimpleName();
    private SurfaceView surfaceView;
    private String playUrl = "";
    private TigerPlayer tigerPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_player);
        surfaceView = this.findViewById(R.id.player_surface);
        tigerPlayer = new TigerPlayer();
        tigerPlayer.setSurfaceView(surfaceView);
        playUrl = "http://vfx.mtime.cn/Video/2021/07/10/mp4/210710171112971120.mp4";
        tigerPlayer.setDataSource(playUrl);

        Log.d(TAG, "==========PlayerActivity onCreate ");
        tigerPlayer.setOnPrepareListener(new OnPrepareListener() {
            @Override
            public void onPrepared() {
                Log.d(TAG, "==========onPrepared ");
                //native层播放器准备好以后就可以通知Java层开始start了
                tigerPlayer.start();
            }
        });

        tigerPlayer.setOnProgressListener(new OnProgressListener() {
            @Override
            public void onProgress(int progress) {
                Log.d(TAG, "==========onProgress progress = " + progress);
            }
        });

        tigerPlayer.setOnErrorListener(new OnErrorListener() {
            @Override
            public void onError(int threadId, String errorDesc) {
                Log.d(TAG, "==========onError threadId = " + threadId + "  errorDesc = " + errorDesc);
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
        tigerPlayer.prepare();
    }

    @Override
    protected void onStop() {
        super.onStop();
        tigerPlayer.stop();
    }
}