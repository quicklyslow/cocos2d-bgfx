$input v_color0, v_texcoord0, v_position

#include "../bgfx_shader.sh"

uniform vec4 u_effectColor;
uniform vec4 u_textColor;
uniform vec4 u_startColor;
uniform vec4 u_endColor;
uniform vec4 u_labelWidth;
uniform vec4 u_labelHeight;
uniform vec4 u_angle;

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
        float angle = u_angle.x;
        float dx = (v_position.x - u_labelWidth.x / 2.0) * cos(angle) + u_labelWidth.x / 2.0;
        float dy = (v_position.y - u_labelHeight.x / 2.0) * sin(angle) + u_labelHeight.x / 2.0;

        float d = length(vec2(dx * cos(angle), dy * sin(angle))) / length(vec2(u_labelHeight.x * sin(angle), u_labelWidth.x * cos(angle)));
        vec4 mixColor = mix(u_startColor, u_endColor, d);
        gl_FragColor = v_color0 * vec4(mixColor.rgb, u_textColor.a * fontAlpha);
    }
    else
    {
        discard;
    }
}