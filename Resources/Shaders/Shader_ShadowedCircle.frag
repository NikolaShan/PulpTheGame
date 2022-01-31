#ifdef GL_ES
precision highp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_position;

uniform float u_outerRadius;
uniform float u_innerRadius;
uniform float u_shadowSize;
uniform float u_blur;

void main() {
    float dist = distance(v_position, vec2(0.5, 0.5));
    
    float midRadius = u_innerRadius + u_shadowSize;
    
    vec4 color;
    if (dist > 0.5 * (midRadius + u_outerRadius)) {
        float alpha = smoothstep(u_outerRadius, u_outerRadius + u_blur, dist);
        color = mix(vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 0.0), alpha);

    } else if (dist < 0.5 * (midRadius + u_innerRadius)) {
        float alpha = 1.0 - smoothstep(u_innerRadius - u_blur, u_innerRadius, dist);
        color = mix(vec4(0.01, 0.01, 0.01, 1.0), vec4(0.01, 0.01, 0.01, 0.0), alpha);

    } else {
        float alphaMid = smoothstep(midRadius - u_blur, midRadius, dist);
        color.rgb = mix(vec3(0.01, 0.01, 0.01), vec3(1.0, 1.0, 1.0), alphaMid);
        color.a = 1.0;
    }
    
    color.rgb = color.rgb * color.a;
    gl_FragColor = color;
}