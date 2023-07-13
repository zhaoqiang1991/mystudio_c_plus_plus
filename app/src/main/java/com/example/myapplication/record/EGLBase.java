package com.example.myapplication.record;

import android.content.Context;
import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLExt;
import android.opengl.EGLSurface;
import android.view.Surface;

import com.example.myapplication.filter.ScreeFilter;

/**
 * EGL配置 和录制opengl的操作
 */
public class EGLBase {
    private final EGLSurface mEglSurface;
    private final ScreeFilter mScreeFilter;
    private EGLDisplay mEglDisplay;
    private EGLConfig mEglConfig;
    private EGLContext mEGLContext;

    /**
     * @param context
     * @param width
     * @param height
     * @param surface MediaCodec创建的surface, 我们需要将这个surface贴到虚拟屏幕里面
     */
    public EGLBase(Context context, int width, int height, Surface surface, EGLContext eglContext) {
        createEGL(eglContext);
        //把surface贴到EGLDisplay 虚拟屏幕里面
        int[] attrib_list = {
                //不需要配置什么属性
                EGL14.EGL_NONE};

        //就是向mEglDisplay这个虚拟屏幕上面画画
        mEglSurface = EGL14.eglCreateWindowSurface(mEglDisplay, mEglConfig, surface, attrib_list, 0);
        //必须要绑定当前线程的显示上下文，不然就绘制不上去，这样子之后操作的opelgl就是在这个虚拟屏幕上操作,读和写都是在同一个surface里面
        if (!EGL14.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEGLContext)) {
            throw new RuntimeException("eglMakeCurrent failed");
        }

        //向虚拟屏幕画画
        mScreeFilter = new ScreeFilter(context);
        mScreeFilter.onReady(width, height);
    }

    private void createEGL(EGLContext eglContext) {
        //创建虚拟显示器
        mEglDisplay = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);
        if (mEglDisplay == EGL14.EGL_NO_DISPLAY) {
            throw new RuntimeException("eglGetDisplay failed");
        }
        int[] version = new int[2];
        //初始化虚拟设备
        if (!EGL14.eglInitialize(mEglDisplay, version, 0, version, 1)) {
            throw new RuntimeException("eglInitialize failed");
        }

        int[] attrib_list = new int[]{//rgba 红绿蓝透明度
                EGL14.EGL_RED_SIZE, 8, EGL14.EGL_GREEN_SIZE, 8, EGL14.EGL_BLUE_SIZE, 8, EGL14.EGL_ALPHA_SIZE, 8, EGL14.EGL_RENDERABLE_TYPE, EGL14.EGL_OPENGL_ES2_BIT,//和egl的版本有关系
                EGL14.EGL_NONE//这个很重要，一定要配置为NONE，表示配置结束了
        };
        EGLConfig[] configs = new EGLConfig[1];
        int[] num_config = new int[1];
        boolean eglChooseConfig = EGL14.eglChooseConfig(mEglDisplay, attrib_list, 0, configs, 0, configs.length, num_config, 0);
        if (!eglChooseConfig) {
            //如果配置失败
            throw new IllegalArgumentException("eglChooseConfig failed");
        }
        mEglConfig = configs[0];
        int[] attriblist = {EGL14.EGL_CONTEXT_CLIENT_VERSION, 2, EGL14.EGL_NONE};
        //创建EGL上下文
        //share_context 共享上下，传递绘制线程的(GLThread)EGL上下文，达到共享资源的目的
        mEGLContext = EGL14.eglCreateContext(mEglDisplay, mEglConfig, eglContext, attriblist, 0);
        if (mEGLContext == null || mEGLContext == EGL14.EGL_NO_CONTEXT) {
            mEGLContext = null;
            throw new RuntimeException("createContex error !");
        }
    }

    /**
     * @param textureId 纹理id,代表一张图片
     * @param timesnap  时间戳
     */
    public void draw(int textureId, long timesnap) {
        //必须要绑定当前线程的显示上下文，不然就绘制不上去，这样子之后操作的opelgl就是在这个虚拟屏幕上操作,读和写都是在同一个surface里面
        //画画之前也必须要绑定
        if (!EGL14.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEGLContext)) {
            throw new RuntimeException("eglMakeCurrent failed");
        }
        //向虚拟屏幕画画
        mScreeFilter.onDrawFrame(textureId);

        //刷新eglSurface时间戳
        EGLExt.eglPresentationTimeANDROID(mEglDisplay, mEglSurface, timesnap);

        //交换数据,EGL工作模式，双缓存模式，内部有两个frameBuff,当EGL将一个frame显示到屏幕上以后，
        // 另一个frame就在后台等待opengl进行交换
        //也就是画完一次，交换一次
        EGL14.eglSwapBuffers(mEglDisplay, mEglSurface);

    }


    public void release() {
        EGL14.eglDestroySurface(mEglDisplay, mEglSurface);
        EGL14.eglMakeCurrent(mEglDisplay, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_CONTEXT);
        EGL14.eglDestroyContext(mEglDisplay, mEGLContext);
        EGL14.eglReleaseThread();
        EGL14.eglTerminate(mEglDisplay);
    }
}
