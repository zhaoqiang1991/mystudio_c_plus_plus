//把四个定点坐标传递给这个变量，来确定要画画的形状
attribute vec4 vPosition;
//接受纹理坐标，接受采样器采样的纹理坐标，也就是我们拍照的时候那张照片，但是对于SurfaceTexture比较特殊，需要使用另一个变换矩阵
attribute vec4 vCoord;
//需要与原有的vCoord(01,11,00,10)相乘才可以得到正确的surfaceTexture坐标
uniform mat4 vMatrix;

//传递给片元着色器,传递的实际上就是很多像素点了而不是坐标了，
varying vec2 aCoord;

void main() {
    //gl_Position是一个内置变量，我们把定点数据传递给gl_Position这个变量，那么opengl就知道会什么形状了
    gl_Position = vPosition;
    aCoord = (vMatrix * vCoord).xy;
}
