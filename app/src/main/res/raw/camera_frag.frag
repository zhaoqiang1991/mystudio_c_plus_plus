#extension GL_OES_EGL_image_external : require
//SurfaceTexture比较特殊
//指定float数据是什么精度的
precision mediump float;
//顶点着色器传递给片元着色器的像素点，然后在片元着色器里面去采集这些像素点的颜色
varying vec2 aCoord;

//采样器
uniform samplerExternalOES vTexture;
void main() {
    //变量 接收像素值
    // texture2D：内置函数，vTexture采样器去采集 aCoord这个位置的的像素点
    //赋值给 gl_FragColor 就可以了,gl_FragColor是一个opengl的内置变量
    gl_FragColor = texture2D(vTexture, aCoord);
}
