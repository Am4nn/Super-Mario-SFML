#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D currentTexture;
uniform float time;

float noise1d(float value)
{
    return cos(value + cos(value * 90.0) * 100.0) * 0.5 + 0.5;
}

float random2d(vec2 coord)
{
    return fract(sin(dot(coord.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    vec4 image = texture2D(currentTexture, gl_TexCoord[0].xy);

    float grain = random2d(vec2(sin(gl_TexCoord[0].xy) / 999999.9) * 7 * time);

    image.r += grain;
    image.g += grain;
    image.b += grain;

    gl_FragColor = image;
}