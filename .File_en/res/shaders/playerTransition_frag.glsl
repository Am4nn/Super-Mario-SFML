#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D currentTexture;
uniform float time;
uniform bool condition;

float noise1d(float value)
{
    return cos(value + cos(value * 90.0) * 100.0) * 0.5 + 0.5;
}

float random2d(vec2 coord)
{
    return fract(sin(dot(coord.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float size = 1.0;
float speed = 6.0;

void main()
{
    vec4 image = texture2D(currentTexture, gl_TexCoord[0].xy);

    float grain = random2d(vec2(sin(gl_TexCoord[0].xy) / 999999.9) * speed * time);

    if (condition)
        image.r += noise1d(time);
    else
        image.g += noise1d(time);

    image.b += noise1d(time);

    gl_FragColor = image;
}