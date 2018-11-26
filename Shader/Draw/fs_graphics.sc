$input v_texcoord0

#include "../bgfx_shader.sh"

uniform vec4 strokeMult;
uniform vec4 color;

float strokeMask(vec2 texcoord)
{
    return min(1.0, (1.0-abs(texcoord.x*2.0-1.0))*strokeMult.x) * min(1.0, texcoord.y);
}

void main()
{
    float strokeAlpha = strokeMask(v_texcoord0);
    gl_FragColor = vec4(color.rgb, color.a * strokeAlpha);
}