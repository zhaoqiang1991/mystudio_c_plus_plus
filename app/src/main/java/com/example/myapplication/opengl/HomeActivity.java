package com.example.myapplication.opengl;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.example.myapplication.R;

public class HomeActivity extends AppCompatActivity {

    private Button mGreyFilter;
    private Button mCannyFilter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);
        mGreyFilter = this.findViewById(R.id.grey_filter);
        mCannyFilter = this.findViewById(R.id.canny_filter);

        mGreyFilter.setOnClickListener(click->{
            Intent intent = new Intent(HomeActivity.this,PhtotoFilterActivity.class);
            startActivity(intent);
        });

        mCannyFilter.setOnClickListener(click->{
            Intent intent = new Intent(HomeActivity.this,CannyActivity.class);
            startActivity(intent);
        });
    }
}