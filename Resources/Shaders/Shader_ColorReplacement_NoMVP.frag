#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform vec4 u_replacementColor;

void main() {
    vec4 tex = texture2D(CC_Texture0, v_texCoord);
    float alpha = tex.r;
    if (tex.a > 0.00001) {
        alpha = alpha / tex.a;
    }
    vec3 colorRGB = mix(u_replacementColor.rgb, v_fragmentColor.rgb, alpha);
    vec4 color = vec4(colorRGB, v_fragmentColor.a * tex.a);
    color.rgb = color.rgb * color.a;
    gl_FragColor = color;
}