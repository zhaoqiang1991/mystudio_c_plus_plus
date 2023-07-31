package com.example.myapplication.opengl.filter;

import static android.opengl.GLES20.GL_TEXTURE0;
import static android.opengl.GLES20.GL_TEXTURE_2D;
import static android.opengl.GLES20.glActiveTexture;
import static android.opengl.GLES20.glBindTexture;
import static android.opengl.GLES20.glGetUniformLocation;
import static android.opengl.GLES20.glUniform1i;

import android.content.Context;
import android.opengl.GLES20;

import com.example.myapplication.R;
import com.example.myapplication.opengl.utils.OpenUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class PhotoFilter {
    private final int mProgram;
    private final int vPosition;
    private final int vCoord;

    private final int[] uTextureUnitLocation;
    private Context mContext;
    private int mWidth;
    private int mHeight;

    private FloatBuffer vertBuffer;
    private FloatBuffer fragBuffer;


    public PhotoFilter(Context mContext) {
        this.mContext = mContext;
        String vertetSource = OpenUtils.readRawTextFile(mContext.getApplicationContext(), R.raw.multi_texture_vertex_shader);
        String fragSource = OpenUtils.readRawTextFile(mContext.getApplicationContext(), R.raw.lut_fragment_shader);

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
        vCoord = GLES20.glGetAttribLocation(mProgram, "a_TextureCoordinates");

        uTextureUnitLocation = new int[2];
        for(int i = 0; i < uTextureUnitLocation.length; i++){
            uTextureUnitLocation[i] = glGetUniformLocation(mProgram, "u_TextureUnit" + i);
        }

        //定点着色器坐标
        float[] v = {
                -1.0f, -1.0f,
                1.0f, -1.0f,
                -1.0f,  1.0f,
                1.0f,  1.0f

        };

        //纹理坐标
        float[] f = {
                0.0f, 1.0f,
                1.0f, 1.0f,
                0.0f, 0.0f,
                1.0f, 0.0f

        };
        vertBuffer = ByteBuffer.allocateDirect(v.length * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        vertBuffer.clear();

        vertBuffer.put(v);

        fragBuffer = ByteBuffer.allocateDirect(f.length * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        fragBuffer.clear();
        fragBuffer.put(f);

    }


    public void onDrawFrame(int [] textureIDs) {
        GLES20.glViewport(0, 0, mWidth, mHeight);
        //使用这个着色器小程序
        GLES20.glUseProgram(mProgram);


        //通过mVertexBuffer把cpu 中的数据传递到GPU里面的变量中
        vertBuffer.position(0);
        //坐标赋值
        GLES20.glVertexAttribPointer(vPosition, 2, GLES20.GL_FLOAT, false, 2 * 4, vertBuffer);
        //第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔。由于下个组位置数据在3个float之后，我们把步长设置为3 * sizeof(float)。
        // 要注意的是由于我们知道这个数组是紧密排列的（在两个顶点属性之间没有空隙）我们也可以设置为0来让OpenGL决定具体步长是多少（只有当数值是紧密排列时才可用）
        // 。一旦我们有更多的顶点属性，我们就必须更小心地定义每个顶点属性之间的间隔，我们在后面会看到更多的例子（译注: 这个参数的意思简单说就是从这个属性第
        // 二次出现的地方到整个数组0位置之间有多少字节）
        //激活数据
        GLES20.glEnableVertexAttribArray(vPosition);
        vertBuffer.position(0);

        //纹理赋值
        fragBuffer.position(0);
        GLES20.glVertexAttribPointer(vCoord,2,GLES20.GL_FLOAT, false, 2 * 4, fragBuffer);
        GLES20.glEnableVertexAttribArray(vCoord);


        for(int i = 0; i < textureIDs.length; i++){
            //激活图层
            glActiveTexture(GL_TEXTURE0  + i);
            //绑定图层
            glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
            //传递参数 0：需要和纹理层GL_TEXTURE0对应
            glUniform1i(uTextureUnitLocation[i], i);
        }
        //参数传递完成，通知opengl开始画画 从第0点开始 共4个点
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP,0,4);
    }


    public void onReady(int width, int height) {
        mWidth = width;
        mHeight = height;
    }
}
