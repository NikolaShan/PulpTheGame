attribute vec4 a_position;
attribute vec4 a_color;

#ifdef GL_ES
varying mediump vec2 v_position;
#else
varying vec2 v_position;
#endif

void main() {
    gl_Position = CC_PMatrix * a_position;
    v_position = a_color.xy;
}