$input v_color0, v_texcoord0

#include "../bgfx_shader.sh"

uniform vec4 u_textColor;
SAMPLER2D(s_texColor, 0);

void main()
{
    float dist = texture2D(s_texColor, v_texcoord0).a;
    float width = fwidth(dist);
    float alpha = smoothstep(0.5-width, 0.5+width, dist) * u_textColor.a;
	gl_FragColor = v_color0 * vec4(u_textColor.rgb, alpha);
}