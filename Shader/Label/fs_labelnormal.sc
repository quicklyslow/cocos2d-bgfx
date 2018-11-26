$input v_color0, v_texcoord0

#include "../bgfx_shader.sh"

uniform vec4 u_textColor;
SAMPLER2D(s_texColor, 0);

void main()
{
	gl_FragColor = v_color0 * vec4(u_textColor.rgb, u_textColor.a * texture2D(s_texColor, v_texcoord0).r);
}