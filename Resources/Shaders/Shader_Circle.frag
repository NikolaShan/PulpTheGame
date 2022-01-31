#ifdef GL_ES
precision highp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_position;

uniform float u_outerRadius;
uniform float u_innerRadius;
uniform float u_blur;

void main() {
    float dist = distance(v_position, vec2(0.5, 0.5));
    float alpha = 1.0 - smoothstep(u_innerRadius - u_blur, u_innerRadius, dist) + smoothstep(u_outerRadius, u_outerRadius + u_blur, dist);
    
    vec4 color = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 0.0), alpha);
    color.rgb = color.rgb * color.a;
    gl_FragColor = color;
}