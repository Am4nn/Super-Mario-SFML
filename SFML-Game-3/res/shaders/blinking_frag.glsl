#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D currentTexture;
uniform float time;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    vec4 pixelColor = texture2D(currentTexture, coord);
    float alpha = 0.5 + 0.5 * cos(20 * time);

    gl_FragColor = vec4 (pixelColor.xyz, min(alpha, pixelColor.w));
}