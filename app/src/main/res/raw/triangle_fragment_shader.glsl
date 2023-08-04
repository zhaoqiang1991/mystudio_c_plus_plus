//SurfaceTexture比较特殊
//float数据是什么精度的
precision mediump float;
uniform vec4 vColor;
void main(){
    gl_FragColor = vColor;
}