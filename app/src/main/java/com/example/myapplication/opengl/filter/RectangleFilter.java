package com.example.myapplication.opengl.filter;

import static android.opengl.GLES20.glGetUniformLocation;

import android.content.Context;
import android.opengl.GLES20;

import com.example.myapplication.R;
import com.example.myapplication.opengl.utils.OpenUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.opengles.GL10;

public class RectangleFilter {
    private final int mProgram;
    private final int vPosition;
    private final int vColorHandle;
    private final int mMVPMatrixHandle;

    private final int vertexCount;
    private int mWidth;
    private int mHeight;

    private FloatBuffer vertBuffer;
    private float[] mColor = {
            0.0f, 1.0f, 0.0f, 1.0f,
    };
    //变换矩阵，提供set方法
    private float[] mvpMatrix = new float[16];


    public RectangleFilter(Context mContext) {
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
        vColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");
        // 获取变换矩阵的句柄
        mMVPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uMVPMatrix");


        //定点着色器坐标
        /*float[] v = {
                0.0f, 1.0f, 0.0f,      // top
                1.0f, -1.0f, 0.0f,    // bottom left
                -1f,-1f, 0.0f   ,     // bottom right

        };*/

        float[] v = {
                -1f, 0.5f, 0.0f, // top left
                -1f, -0.5f, 0.0f, // bottom left
                1f, 0.5f, 0.0f,  // top right
                1f, -0.5f, 0.0f  // bottom right
        };
        vertexCount = v.length / 3;
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
        GLES20.glVertexAttribPointer(vPosition, 3, GLES20.GL_FLOAT, false, 3 * 4, vertBuffer);
        //激活数据
        GLES20.glEnableVertexAttribArray(vPosition);
        vertBuffer.position(0);


        GLES20.glUniform4fv(vColorHandle, 1, mColor, 0);
        // 将投影和视图转换传递给着色器，可以理解为给uMVPMatrix这个变量赋值为mvpMatrix
        GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);

        /** 3.绘制正方形，4个顶点， GL_TRIANGLE_STRIP的方式绘制连续的三角形*/
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, vertexCount);//vertexCount = 4

    }


    public void onReady(int width, int height) {
        mWidth = width;
        mHeight = height;
    }
}
