package com.example.myapplication.video;

import android.app.Activity;
import android.os.Bundle;
import android.text.TextUtils;

import com.example.myapplication.R;
import com.example.myapplication.video.widget.VideoView;


public class VideoActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video);
        String path = getIntent().getStringExtra("path");
        if (TextUtils.isEmpty(path)){
            finish();
        }
        VideoView videoView = findViewById(R.id.videoView);
        videoView.setDataSource(path);
        videoView.startPlay();
    }
}
