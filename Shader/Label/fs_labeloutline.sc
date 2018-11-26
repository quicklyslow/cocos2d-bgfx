$input v_color0, v_texcoord0

#include "../bgfx_shader.sh"

uniform vec4 u_effectColor;
uniform vec4 u_textColor;
SAMPLER2D(s_texColor, 0);

void main()
{
    vec4 texel = texture2D(s_texColor, v_texcoord0);
    float fontAlpha = texel.g;
    float outlineAlpha = texel.r;
    if(u_effectColor.a > 0.0)
    {
        if(fontAlpha < 1.0)
        {
            gl_FragColor = vec4(u_effectColor.rgb, u_effectColor.a * outlineAlpha);
        }
        else
        {
            discard;
        }
    }
    else if(fontAlpha > 0.0)
    {
        gl_FragColor = v_color0 * vec4(u_textColor.rgb, u_textColor.a * fontAlpha);
    }
    else
    {
        discard;
    }
}