package com.example.myapplication.record;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.opengl.EGLContext;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.view.Surface;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * 视频录制
 */
public class MediaRecorder {
    private final Context mContext;
    private final String mPath;
    private final int mWidth;
    private final int mHeight;
    private final EGLContext mEglContext;
    private MediaCodec mMediaCodec;
    private Surface mInputSurface;
    private MediaMuxer mMediaMuxer;
    private Handler mHandler;
    private EGLBase mEglBase;
    private boolean isStart;
    private int index;
    private float mSpeed;

    /**
     * @param context
     * @param path    视频保存地址
     * @param width   视频宽
     * @param height  视频高
     */
    public MediaRecorder(Context context, String path, int width, int height, EGLContext eglContext) {
        mContext = context.getApplicationContext();
        mPath = path;
        mWidth = width;
        mHeight = height;
        mEglContext = eglContext;
    }

    /**
     * 开始录制视频
     */
    public void start(float speed) throws IOException {
        /**
         * 配置MediaCodec编码器，视频编码的宽，高，帧率，码率
         * 录制成mp4格式，视频编码格式是h264 MIMETYPE_VIDEO_AVC 高级编码
         */
        mSpeed = speed;
        MediaFormat videoFormat = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, mWidth, mHeight);
        //配置码率 1500kbs
        videoFormat.setInteger(MediaFormat.KEY_BIT_RATE, 1500_000);
        //帧率
        videoFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 20);
        //关键字间隔
        videoFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 20);

        //创建视频高级编码器
        mMediaCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC);
        //因为是从Surface中读取的，所以不需要设置这个颜色格式
        videoFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        //配置编码器，CONFIGURE_FLAG_ENCODE，
        mMediaCodec.configure(videoFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
        //交给虚拟屏幕，通过opengl 将预览的纹理绘制到这一个虚拟屏幕中，这样子Mediacodc 就会自动编码这一帧图像
        mInputSurface = mMediaCodec.createInputSurface();

        //mp4 播放流程  解复用--》解码 》绘制

        //mp4 编码流程  封装器--》编码

        //MUXER_OUTPUT_MPEG_4 MP4格式封装器，将h.264通过他写出到文件就可以了
        mMediaMuxer = new MediaMuxer(mPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);

        /**
         * 配置EGL环境，也就是配置我们的虚拟屏幕环境
         */

        HandlerThread handlerThread = new HandlerThread("ViewoCodec");
        handlerThread.start();
        Looper looper = handlerThread.getLooper();

        //子线程和子线程之间的通信
        mHandler = new Handler(looper);

        //EGl的绑定线程，对我们自己创建的EGl环境，都是在这个线程里面进行
        mHandler.post(() -> {
            //创建我们的EGL环境(虚拟设备，EGL上下文 )
            mEglBase = new EGLBase(mContext, mWidth, mHeight, mInputSurface, mEglContext);
            //启动编码器
            mMediaCodec.start();
            isStart = true;
        });

    }


    /**
     * textureId 纹理id
     * 调用一次，就有一个新的图片需要编码
     */
    public void encodeFrame(int textureId, long timesnap) {
        if (!isStart) {
            return;
        }

        //切换到子线程中编码
        mHandler.post(() -> {
            //把图像纹理画到虚拟屏幕里面
            mEglBase.draw(textureId, timesnap);

            //此时我们需要从编码器里面的输出缓冲区获取编码以后的数据就可以了，
            getCodec(false);
        });
    }

    private void getCodec(boolean endOfStream) {
        if (endOfStream) {
            //表示停止录制，此时我们不录制了，需要给mediacoic 通知
            mMediaCodec.signalEndOfInputStream();
        }
        MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
        //希望将已经编码完成的数据都获取到，然后写出到mp4文件中
        while (true) {
            //并不是传给mediacodec一帧数据就表示可以编码出一帧数据，有可能需要好多帧数据才可以同时编码出数据出来
            //输出缓冲区
            //传递-1表示一直等到输出缓冲区有一个编码好的有效的数据以后才会继续向下走，不然就会一直卡在127行，
            //10_000超时时间
            int status = mMediaCodec.dequeueOutputBuffer(bufferInfo, 10_000);
            if (status == MediaCodec.INFO_TRY_AGAIN_LATER) {
                //如果是停止，就继续循环，
                // 继续循环就表示不会接收到新的等待编码的图像了
                //相当于保证mediacodic中所有待编码的数据都编码完成
                // 标记不是停止，我们退出，下一轮接收到更多的数据才来输出编码以后的数据，我们就让继续走
                // 表示需要更多数据才可以编码出图像
                if (!endOfStream) {
                    //结束录制了
                    break;
                }
                //否则继续
            } else if (status == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                //开始编码，就会调用一次
                MediaFormat outputFormat = mMediaCodec.getOutputFormat();
                //配置封装器，增加一路指定格式的媒体流
                index = mMediaMuxer.addTrack(outputFormat);
                //启动封装器
                mMediaMuxer.start();
            } else if (status == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                //忽略
            } else {
                //成功取出一个有效的输出
                ByteBuffer outputBuffer = mMediaCodec.getOutputBuffer(status);
                if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
                    //如果获取的ByteBuffer是配置信息，那么就不需要写出到mp4文件中
                    bufferInfo.size = 0;
                }
                if (bufferInfo.size != 0) {
                    bufferInfo.presentationTimeUs = (long) (bufferInfo.presentationTimeUs / mSpeed);
                    //写出到 mp4文件中
                    //根据偏移定位去获取数据，而不是从0开始
                    outputBuffer.position(bufferInfo.offset);
                    //设置可读可写的总长度
                    outputBuffer.limit(bufferInfo.offset + bufferInfo.size);
                    mMediaMuxer.writeSampleData(index, outputBuffer, bufferInfo);
                }
                //输出缓冲区使用完毕了， 此时就可以回收了，让mediacodec继续使用
                mMediaCodec.releaseOutputBuffer(status, false);

                if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                    //结束了，
                    break;
                }
            }
        }
    }

    public void stop() {
        isStart = false;
        mHandler.post(() -> {
            getCodec(true);
            mMediaCodec.stop();
            mMediaCodec.release();
            mMediaCodec = null;
            mMediaMuxer.stop();
            mMediaMuxer.release();
            mMediaMuxer = null;
            mEglBase.release();
            mEglBase = null;
            mInputSurface = null;
            mHandler.getLooper().quitSafely();
            mHandler = null;
        });
    }
}
