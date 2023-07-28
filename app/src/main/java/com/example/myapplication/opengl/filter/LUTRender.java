package com.example.myapplication.opengl.filter;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;


import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static android.opengl.GLES20.GL_COLOR_BUFFER_BIT;
import static android.opengl.GLES20.glClear;
import static android.opengl.GLES20.glClearColor;
import static android.opengl.GLES20.glViewport;

import static com.example.myapplication.bean.Constants.BYTES_PER_FLOAT;

import com.example.myapplication.R;
import com.example.myapplication.bean.VertexArray;
import com.example.myapplication.opengl.LUTProgram;
import com.example.myapplication.opengl.utils.TextureHelper;


public class LUTRender implements GLSurfaceView.Renderer {
    private static final String TAG = "LUTRender";
    private static final int POSITION_COMPONENT_COUNT = 2;
    private static final int TEXTURE_COORDINATES_COMPONENT_COUNT = 2;
    private static final int STRIDE = (POSITION_COMPONENT_COUNT
            + TEXTURE_COORDINATES_COMPONENT_COUNT) * BYTES_PER_FLOAT;

    /**
     * 纹理坐标信息用来定义该矩形上每个顶点对应的纹理坐标，以决定如何在这个矩形上贴图（即将图片纹理映射到矩形上）。
     *
     * 以下是每个顶点的坐标和纹理坐标的详细说明：
     *
     * 第一个顶点 (-1.0, -1.0) 对应的纹理坐标为 (0.0, 1.0)。
     * 第二个顶点 (1.0, -1.0) 对应的纹理坐标为 (1.0, 1.0)。
     * 第三个顶点 (-1.0, 1.0) 对应的纹理坐标为 (0.0, 0.0)。
     * 第四个顶点 (1.0, 1.0) 对应的纹理坐标为 (1.0, 0.0)。
     * 这样得到的矩形顶点信息和纹理坐标信息可以用于绘制一个全屏大小的矩形，并将图片纹理贴图到该矩形上。同时，由于纹理坐标的设计
     * ，图片将会在贴图过程中进行上下翻转，即纹理的上下部分会颠倒显示。这通常用于OpenGL中处理图片纹理时的一种常见操作
     */
    public static final float CUBE[] = {//翻转顶点信息中的纹理坐标,统一用1去减
            -1.0f, -1.0f, 0f, 1f,
            1.0f, -1.0f, 1f, 1f,
            -1.0f, 1.0f, 0f, 0f,
            1.0f, 1.0f, 1f, 0f,
    };
/*	public static final float CUBE[] = {//翻转顶点信息中的纹理坐标,统一用1去减
			-1.0f,  1.0f, 1.0f,  1f,  1f, //0左上
        -1.0f, -1.0f, 1.0f,  1f,  0f, //1左下
         1.0f,  1.0f, 1.0f,  0f,  1f, //2右上
         1.0f, -1.0f, 1.0f,  0f,  0f, //3右下


	};*/

    Context context;
    VertexArray vertexArray;
    LUTProgram lutProgram;
    private int[] textureIDs;
    private int originTexture;
    private int lutTexture;

    public LUTRender(Context context) {
        this.context = context;
        vertexArray = new VertexArray(CUBE);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        lutProgram = new LUTProgram(context);
        originTexture = TextureHelper.loadTexture(context, R.drawable.lena);
        lutTexture = TextureHelper.loadLutTexture(context, R.drawable.fairy_tale);

        textureIDs = new int[]{originTexture, lutTexture};
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        glViewport(0, 0, width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        glClear(GL_COLOR_BUFFER_BIT);

        lutProgram.useProgram();
        lutProgram.setUniforms(textureIDs);

        vertexArray.setVertexAttribPointer(
                0,
                lutProgram.getPositionAttributeLocation(),
                POSITION_COMPONENT_COUNT,
                STRIDE);

        vertexArray.setVertexAttribPointer(
                POSITION_COMPONENT_COUNT,
                lutProgram.getTextureCoordinatesAttributeLocation(),
                TEXTURE_COORDINATES_COMPONENT_COUNT,
                STRIDE);
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);
    }
}