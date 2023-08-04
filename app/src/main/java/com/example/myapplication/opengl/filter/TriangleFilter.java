package com.example.myapplication.opengl.filter;

import static android.opengl.GLES20.GL_TEXTURE0;
import static android.opengl.GLES20.GL_TEXTURE_2D;
import static android.opengl.GLES20.glActiveTexture;
import static android.opengl.GLES20.glBindTexture;
import static android.opengl.GLES20.glGetUniformLocation;

import android.content.Context;
import android.opengl.GLES20;

import com.example.myapplication.R;
import com.example.myapplication.opengl.utils.OpenUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class TriangleFilter {
    private final int mProgram;
    private final int vPosition;

    private final int vColor;
    private int mWidth;
    private int mHeight;

    private FloatBuffer vertBuffer;

    public TriangleFilter(Context mContext) {
        String vertetSource = OpenUtils.readRawTextFile(mContext.getApplicationContext(), R.raw.triangle_vertex_shader);
        String fragSource = OpenUtils.readRawTextFile(mContext.getApplicationContext(), R.raw.triangle_fragment_shader);

        //创建定点着色器
        int vshaderId = GLES20.glCreateShader(GLES20.GL_VERTEX_SHADER);
        //绑定定点着色器到着色器程序上面
        GLES20.glShaderSource(vshaderId, vertetSource);
        //编译着色器代码
        GLES20.glCompileShader(vshaderId);
        //主动获取下配置是否成功，失败
        int[] status = new int[1];
        GLES20.glGetShaderiv(vshaderId, GLES20.GL_COMPILE_STATUS, status, 0);
        if (status[0] != GLES20.GL_TRUE) {
            throw new IllegalStateException("ScreenFilter 顶点着色器配置失败!");
        }


        int fshaderId = GLES20.glCreateShader(GLES20.GL_FRAGMENT_SHADER);
        GLES20.glShaderSource(fshaderId, fragSource);
        GLES20.glCompileShader(fshaderId);
        GLES20.glGetShaderiv(vshaderId, GLES20.GL_COMPILE_STATUS, status, 0);
        if (status[0] != GLES20.GL_TRUE) {
            throw new IllegalStateException("ScreenFilter 片元着色器配置失败!");
        }
        //创建运行在GPU上面运行的小程序
        mProgram = GLES20.glCreateProgram();
        GLES20.glAttachShader(mProgram, vshaderId);
        GLES20.glAttachShader(mProgram, fshaderId);

        //链接着色器程序
        GLES20.glLinkProgram(mProgram);

        //获取链接是否成功的状态信息
        GLES20.glGetProgramiv(mProgram, GLES20.GL_LINK_STATUS, status, 0);
        if (status[0] != GLES20.GL_TRUE) {
            throw new IllegalStateException("ScreenFilter 着色器程序配置失败!");
        }

        //因为已经塞到了着色器程序里面，所以现在删除也没关系
        GLES20.glDeleteShader(vshaderId);
        GLES20.glDeleteShader(fshaderId);

        /**
         * 1.找到写在glsl 文件中的变量
         *
         * 2.赋值(这步也就是画画的过程)
         */

        vPosition = GLES20.glGetAttribLocation(mProgram, "a_Position");
        vColor = glGetUniformLocation(mProgram, "vColor");



        //定点着色器坐标
        /*float[] v = {
                0.0f, 1.0f, 0.0f,      // top
                1.0f, -1.0f, 0.0f,    // bottom left
                -1f,-1f, 0.0f   ,     // bottom right

        };*/

        float[] v = {
                -1.0f, -1.0f, 0.0f,      // top
                1.0f, -1.0f, 0.0f,    // bottom left
                0f,1f, 0.0f   ,     // bottom right
        };

        vertBuffer = ByteBuffer.allocateDirect(v.length * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        vertBuffer.clear();
        vertBuffer.put(v);


    }


    public void onDrawFrame(int textureId) {
        GLES20.glViewport(0, 0, mWidth, mHeight);
        //使用这个着色器小程序
        GLES20.glUseProgram(mProgram);


        //通过mVertexBuffer把cpu 中的数据传递到GPU里面的变量中
        vertBuffer.position(0);
        //坐标赋值 size表示一个坐标，几个点  stride：一个顶点占有的总的字节数，这里为3个float，所以是sizeof(float)*2
        GLES20.glVertexAttribPointer(vPosition, 3, GLES20.GL_FLOAT, false, 3*4, vertBuffer);

        //激活数据
        GLES20.glEnableVertexAttribArray(vPosition);
        vertBuffer.position(0);

        float[] colors = new float[]{
                1f, 0.4f, 0.3f, 0.1f
        };

        GLES20.glUniform4fv(vColor, 1, colors,0);

        //参数传递完成，通知opengl开始画画 从第0点开始 共4个点
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 3);
    }


    public void onReady(int width, int height) {
        mWidth = width;
        mHeight = height;
    }
}
