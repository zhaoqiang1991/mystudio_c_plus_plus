package com.example.myapplication.opengl.filter;

import android.content.Context;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;

import com.example.myapplication.R;
import com.example.myapplication.opengl.utils.OpenUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class ScreeFilter {
    private final int mProgram;
    private final int vPosition;
    private final int vCoord;
    private final int vMatrix;
    private final int vTexture;
    private Context mContext;
    private int mWidth;
    private int mHeight;
    private FloatBuffer mTextureBuffer;
    private FloatBuffer mVertexBuffer;


    public ScreeFilter(Context mContext) {
        this.mContext = mContext;
        String vertetSource = OpenUtils.readRawTextFile(mContext.getApplicationContext(), R.raw.camera_vertex);
        String fragSource = OpenUtils.readRawTextFile(mContext.getApplicationContext(), R.raw.camera_sqlit_frag);

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

        vPosition = GLES20.glGetAttribLocation(mProgram, "vPosition");
        vCoord = GLES20.glGetAttribLocation(mProgram, "vCoord");
        vMatrix = GLES20.glGetUniformLocation(mProgram, "vMatrix");


        vTexture = GLES20.glGetUniformLocation(mProgram, "vTexture");


        mVertexBuffer = ByteBuffer.allocateDirect(4 * 2 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mVertexBuffer.clear();

        //定点着色器坐标
        float[] v = {
                -1.0f, -1.0f,
                1.0f, -1.0f,
                -1.0f, 1.0f,
                1.0f, 1.0f,
        };

        mVertexBuffer.put(v);


        //纹理坐标
        mTextureBuffer = ByteBuffer.allocateDirect(4 * 2 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mTextureBuffer.clear();

        //定点着色器坐标
        //原始数据 数据有问题 是颠倒的
      /*  float[] f = {
                0.0f, 1.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f,
        };*/

        //向右旋转90度  数据是镜像的
       /* float[] f = {
                1.0f, 1.0f,
                1.0f, 0.0f,
                0.0f, 1.0f,
                0.0f, 0.0f,
        };*/


        //在第二步的基础之上进行水平翻转，就可以得到正确数据  这些旋转可以借助画一个坐标，然后在相册里面选装
        float[] f = {
                1.0f, 0.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                0.0f, 1.0f,
        };

        mTextureBuffer.put(f);


    }


    public void onDrawFrame(int texture, float[] mtx) {
        GLES20.glViewport(0, 0, mWidth, mHeight);
        //使用这个着色器小程序
        GLES20.glUseProgram(mProgram);

        //通过mVertexBuffer把cpu 中的数据传递到GPU里面的变量中
        mVertexBuffer.position(0);
        //坐标赋值
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 0, mVertexBuffer);
        //激活数据
        GLES20.glEnableVertexAttribArray(vPosition);

        //纹理赋值
        mTextureBuffer.position(0);
        GLES20.glVertexAttribPointer(vCoord,2,GLES20.GL_FLOAT, false, 0, mTextureBuffer);
        GLES20.glEnableVertexAttribArray(vCoord);

        GLES20.glUniformMatrix4fv(vMatrix,1,false,mtx,0);

        //片元 vTexture 绑定图像数据到采样器
        //激活图层
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES,texture);
        //传递参数 0：需要和纹理层GL_TEXTURE0对应
        GLES20.glUniform1i(vTexture,0);


        //参数传递完成，通知opengl开始画画 从第0点开始 共4个点
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP,0,4);


    }


    public void onReady(int width, int height) {
        mWidth = width;
        mHeight = height;
    }
}
