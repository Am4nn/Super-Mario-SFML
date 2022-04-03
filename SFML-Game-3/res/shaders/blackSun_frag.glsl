#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D currentTexture;
uniform float time;


void main()
{
    vec2 u_resolution = vec2(1280.0, 768.0);
    float xo = gl_TexCoord[0].x * u_resolution.x;
    float yo = gl_TexCoord[0].y * u_resolution.y;
    vec2 FragCoord = vec2(xo, yo);
    vec4 image = texture2D(currentTexture, gl_TexCoord[0].xy);

    vec2 coord = (FragCoord.xy * 2.0 - u_resolution) / min(u_resolution.x, u_resolution.y);
    coord.x += sin(time) + cos(time * 2.1);
    coord.y += cos(time) + sin(time * 1.6);
    float color = 0.0;
    
    color += 0.1 * (abs(sin(time)) + 0.1) / length(coord);

    gl_FragColor = vec4(vec3(color), 1.0);
}