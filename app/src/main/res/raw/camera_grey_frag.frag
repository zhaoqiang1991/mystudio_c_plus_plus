#extension GL_OES_EGL_image_external : require
//SurfaceTexture比较特殊
//float数据是什么精度的
precision mediump float;

//采样点的坐标
varying vec2 aCoord;

//采样器
uniform samplerExternalOES vTexture;

void main(){
    //变量 接收像素值
    // texture2D：采样器 采集 aCoord的像素
    //赋值给 gl_FragColor 就可以了
    vec4 rgba = texture2D(vTexture, aCoord);
    float color = (rgba.r + rgba.g+ rgba.b) /3.0;
    gl_FragColor = vec4(color, color, color, rgba.a);
}