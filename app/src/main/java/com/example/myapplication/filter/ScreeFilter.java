package com.example.myapplication.filter;

import android.content.Context;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;

import com.example.myapplication.R;
import com.example.myapplication.utils.OpenGLUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.opengles.GL;

/**
 * 负责把opengl采集到的数据向屏幕上面显示出来
 */
public class ScreeFilter {

    private final int mProgram;
    private final int vPosition;
    private final int vCoord;
    private final int vMatrix;
    private final int vTexture;
    private final FloatBuffer mVertexBuffer;
    private final FloatBuffer mTextureBuffer;
    private int mWidth;
    private int mHeight;

    public ScreeFilter(Context context) {
        String vertexSource = OpenGLUtils.readRawTextFile(context, R.raw.camera_vertex);
        String fragSource = OpenGLUtils.readRawTextFile(context, R.raw.camera_frag);

        //通过上面的着色器来创建运行在GPU上面的opengl小程序

        /**
         * 1.创建定点着色器
         */
        int vShaderId = GLES20.glCreateShader(GLES20.GL_VERTEX_SHADER);
        //绑定代码到着色器程序上面
        GLES20.glShaderSource(vShaderId, vertexSource);
        //编译着色器程序(编译着色器程序)
        GLES20.glCompileShader(vShaderId);
        //主动获取成功，失败,这个不是必须的，是为了方便定位错误
        int[] status = new int[1];
        GLES20.glGetShaderiv(vShaderId, GLES20.GL_COMPILE_STATUS, status, 0);
        if (status[0] != GLES20.GL_TRUE) {
            throw new IllegalStateException("screen filter 顶点着色器配置失败");
        }

        //* 2.创建片元着色器
        int fShaderId = GLES20.glCreateShader(GLES20.GL_FRAGMENT_SHADER);
        GLES20.glShaderSource(fShaderId, fragSource);
        //编译着色器程序(编译着色器程序)
        GLES20.glCompileShader(vShaderId);
        //主动获取成功，失败,这个不是必须的，是为了方便定位错误
        GLES20.glGetShaderiv(vShaderId, GLES20.GL_COMPILE_STATUS, status, 0);
        if (status[0] != GLES20.GL_TRUE) {
            throw new IllegalStateException("screen filter 片元着色器配置失败");
        }

        //* 3，创建着色器程序

        mProgram = GLES20.glCreateProgram();
        //把着色器和着色器程序绑定

        //把顶点着色器和着色器程序绑定
        GLES20.glAttachShader(mProgram, vShaderId);
        //把片元着色器和着色器程序绑定
        GLES20.glAttachShader(mProgram, fShaderId);

        //链接着色器程序
        GLES20.glLinkProgram(mProgram);

        GLES20.glGetProgramiv(mProgram, GLES20.GL_LINK_STATUS, status, 0);
        if (status[0] != GLES20.GL_TRUE) {
            throw new IllegalStateException("screen filter 着色器程序配置失败");
        }

        //因为着着色器已经已经塞到着色器程序里面了，所以就可以删除掉了
        GLES20.glDeleteShader(vShaderId);
        GLES20.glDeleteShader(fShaderId);

        //获取定点着色器程序里面的索引
        vPosition = GLES20.glGetAttribLocation(mProgram, "vPosition");
        vCoord = GLES20.glGetAttribLocation(mProgram, "vCoord");
        vMatrix = GLES20.glGetUniformLocation(mProgram, "vMatrix");

        //获取片云设色器程序里面的索引
        vTexture = GLES20.glGetUniformLocation(mProgram, "vTexture");
        //4个点，每个点两个数据，每个数据在java里面占用4个字节
        mVertexBuffer = ByteBuffer.allocateDirect(4 * 2 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mVertexBuffer.clear();
        float[] v = {-1.0f, -1.0f,
                1.0f, -1.0f,
                -1.0f, 1.0f,
                1.0f, 1.0f};
        mVertexBuffer.put(v);


        //纹理左边缓冲区
        mTextureBuffer = ByteBuffer.allocateDirect(4 * 2 * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mTextureBuffer.clear();
        float[] t = {-0.0f, 1.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f};
        mTextureBuffer.put(t);
    }


    /**
     * 使用着色器程序来进行画画
     *
     * @param texture
     * @param mtx
     */
    public void onDrawFrame(int texture, float[] mtx) {
        //设置窗口，设置画画的时候，画布大小,设置窗口位置，和窗口大小
        GLES20.glViewport(0, 0, mWidth, mHeight);

        //使用着色器程序来画画
        GLES20.glUseProgram(mProgram);
        //获得着色器程序里面的ID，通过这个所以给着色器程序里面的变量赋值

        //怎么画画？，其实就是给着色器程序里面的变量赋值
        //将顶点数据传入，确定形状
        mVertexBuffer.position(0);
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 0, mVertexBuffer);
        //传入数据以后，需要激活
        GLES20.glEnableVertexAttribArray(vPosition);

        //纹理坐标传入，采样坐标
        mTextureBuffer.position(0);
        GLES20.glVertexAttribPointer(vCoord, 2, GLES20.GL_FLOAT, false, 0, mTextureBuffer);
        GLES20.glEnableVertexAttribArray(vCoord);

        //变换矩阵
        GLES20.glUniformMatrix4fv(vMatrix, 1, false, mtx, 0);


        //片元着色器程序传值
        //激活图层片元着色器传值，绑定图像数据采样器,激活第0层
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);

        //图像数据
        /**正常GL_TEXTURE_2D
         * surfaceTexure特殊，
         */
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texture);
        //纹理和第0层绑定
        GLES20.glUniform1i(vTexture, 0);

        //数据传递完毕了，通知opengl开始画画了,从第0个点开始，画4个点
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }

    public void onReady(int width, int height) {
        mWidth = width;
        mHeight = height;
    }
}
