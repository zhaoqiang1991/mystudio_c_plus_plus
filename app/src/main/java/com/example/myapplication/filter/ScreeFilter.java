package com.example.myapplication.filter;

import android.content.Context;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;

import com.example.myapplication.R;
import com.example.myapplication.utils.OpenGLUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

/**
 * 负责把opengl采集到的数据向屏幕上面显示出来
 */
public class ScreeFilter extends AbstractFilter{

    public ScreeFilter(Context context) {
        super(context,R.raw.base_vertex,R.raw.base_frag);
    }
}
