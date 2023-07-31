package com.example.myapplication.opengl;

import android.opengl.GLES20;
import android.opengl.GLSurfaceView;

import com.example.myapplication.R;
import com.example.myapplication.opengl.filter.PhotoFilter;
import com.example.myapplication.opengl.utils.TextureHelper;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class PhotoRender implements GLSurfaceView.Renderer {
    private final TigerPhotoGlsurfaceView mGlsurfaceView;

    private PhotoFilter mPhotoFilter;
    private int originTexture;
    private int lutTexture;
    private int[] textureIDs;

    public PhotoRender(TigerPhotoGlsurfaceView glsurfaceView) {
        mGlsurfaceView = glsurfaceView;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {

        //保存在GPU的内存中，到时候通过UPDATE刷新到GPU里面

        originTexture = TextureHelper.loadTexture(mGlsurfaceView.getContext(), R.drawable.lena);
        lutTexture = TextureHelper.loadLutTexture(mGlsurfaceView.getContext(), R.drawable.fairy_tale);

        textureIDs = new int[]{originTexture, lutTexture};
        mPhotoFilter = new PhotoFilter(mGlsurfaceView.getContext());

    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mPhotoFilter.onReady(width, height);

    }

    //开始画画
    @Override
    public void onDrawFrame(GL10 gl) {
        //绘制之前清理屏幕
        GLES20.glClearColor(0, 0, 0, 0);
        //开始真正的清理
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

        //交给滤镜可以画画
        mPhotoFilter.onDrawFrame(textureIDs);
    }


}
