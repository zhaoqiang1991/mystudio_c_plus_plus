package com.example.myapplication.player;

import androidx.appcompat.app.AppCompatActivity;

import android.content.res.Configuration;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.Toast;

import com.example.myapplication.R;
import com.example.myapplication.listener.OnErrorListener;
import com.example.myapplication.listener.OnPrepareListener;
import com.example.myapplication.listener.OnProgressListener;

public class PlayerActivity extends AppCompatActivity implements SeekBar.OnSeekBarChangeListener{
    private static final String TAG = PlayerActivity.class.getSimpleName();
    private SurfaceView surfaceView;
    private String playUrl = "";
    private TigerPlayer tigerPlayer;
    private SeekBar seekBar;
    private boolean isTouch;
    private boolean isSeek;
    private int progress;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager
                .LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_player);
        surfaceView = this.findViewById(R.id.player_surface);
        seekBar = this.findViewById(R.id.seekBar);
        seekBar.setOnSeekBarChangeListener(this);
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
                int duration = tigerPlayer.getDuration();

                //直播： 时间就是0
                if (duration != 0){
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(PlayerActivity.this,"native回到到Java层，可以开始播放了",Toast.LENGTH_LONG).show();
                            //显示进度条
                            seekBar.setVisibility(View.VISIBLE);
                        }
                    });
                }
                tigerPlayer.start();
            }
        });

        tigerPlayer.setOnProgressListener(new OnProgressListener() {
            @Override
            public void onProgress(int progress) {
                Log.d(TAG, "==========onProgress progress = " + progress);
                if (!isTouch) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            int duration = tigerPlayer.getDuration();
                            //如果是直播
                            if (duration != 0) {
                                if (isSeek){
                                    isSeek = false;
                                    return;
                                }
                                //更新进度 计算比例
                                seekBar.setProgress(progress * 100 / duration);
                            }
                        }
                    });
                }
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
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager
                    .LayoutParams.FLAG_FULLSCREEN);
        } else {
            getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        }
        setContentView(R.layout.activity_player);
        SurfaceView surfaceView = findViewById(R.id.player_surface);
        tigerPlayer.setSurfaceView(surfaceView);
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

    @Override
    protected void onDestroy() {
        super.onDestroy();
        tigerPlayer.release();
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {
        isTouch = true;
    }

    /**
     * 停止拖动的时候回调
     * @param seekBar
     */
    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        isSeek = true;
        isTouch = false;
        progress = tigerPlayer.getDuration() * seekBar.getProgress() / 100;
        Log.d(TAG,"========= onStopTrackingTouch = progress =  " + progress);
        //进度调整
        tigerPlayer.seek(progress);
    }
}