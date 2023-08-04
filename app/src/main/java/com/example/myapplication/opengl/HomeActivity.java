package com.example.myapplication.opengl;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;

import com.example.myapplication.R;

public class HomeActivity extends AppCompatActivity {

    private Button mGreyFilter;
    private Button mCannyFilter;
    private Button mTriangleFilter;

    private Button mRectangleFilter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_home);
        mGreyFilter = this.findViewById(R.id.grey_filter);
        mCannyFilter = this.findViewById(R.id.canny_filter);
        mTriangleFilter = this.findViewById(R.id.triangle_filter);
        mRectangleFilter = this.findViewById(R.id.riangle_filter);

        mGreyFilter.setOnClickListener(click->{
            Intent intent = new Intent(HomeActivity.this,PhtotoFilterActivity.class);
            startActivity(intent);
        });

        mCannyFilter.setOnClickListener(click->{
            Intent intent = new Intent(HomeActivity.this,CannyActivity.class);
            startActivity(intent);
        });

        mTriangleFilter.setOnClickListener(click->{
            Intent intent = new Intent(HomeActivity.this,TriangleActivity.class);
            startActivity(intent);
        });

        mRectangleFilter.setOnClickListener(click->{
            Intent intent = new Intent(HomeActivity.this,RectangleActivity.class);
            startActivity(intent);
        });
    }
}