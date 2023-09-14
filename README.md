# mystudio_c_plus_plus
一，这是一个采用ffmpeg和c/c++编写的音视频播放器，音频播放采用opensl-es,视频播放采用ffmpeg

二，ffmpeg自己编译Android版本需要的so,具体关于ffmpeg资料可以阅读我的博客
https://blog.csdn.net/qq_18757557/article/details/131826726?spm=1001.2014.3001.5502

三，这个项目可以实现，在线视频的播放，快进，快退，音视频的同步，视频的解码采用的是ffmpeg

四，在feature_my_opengl分支上面还实现了采用opengl glsl语言实现路径特效
 4.1 这个是主页面![image](https://github.com/zhaoqiang1991/mystudio_c_plus_plus/assets/7472721/92857dfc-215d-4b38-8042-0a72c9911b6c)
 
 4.2 透明效果滤镜![image](https://github.com/zhaoqiang1991/mystudio_c_plus_plus/assets/7472721/613f0642-7aae-44aa-825d-0f3951c3de3c)
 
 4.3 使用glsl语言绘制三角形
 ![image](https://github.com/zhaoqiang1991/mystudio_c_plus_plus/assets/7472721/1ef7d9a3-4d4e-450a-8023-52f9db4de3a6)
 4.4 使用glsl语言绘制矩形
 ![image](https://github.com/zhaoqiang1991/mystudio_c_plus_plus/assets/7472721/210a3a15-d81a-481a-8b72-826a5fc3d523)

五,在feature_filter 分支上面实现了美颜，大眼，贴纸特效，录制视频采用的是MediaExtractor
  然后采用MediaCodec进行硬编码，

六,feature_live分支使用了rtmp协议，AudioRecord录音，Android自带的Camera采集视频数据，使用x264进行视频数据编码,然后
通过Rtmp发送到流媒体服务器进行直播，下面是具体的直播视频demo
录制的视频数据上传不上来，所以先放一个截图吧
![image](https://github.com/zhaoqiang1991/mystudio_c_plus_plus/assets/7472721/3a661fe1-fe18-4634-8011-c3bb3d04e2e7)



 


