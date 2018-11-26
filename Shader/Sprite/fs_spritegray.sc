$input v_color0, v_texcoord0

#include "../bgfx_shader.sh"

SAMPLER2D(s_texColor, 0);

void main()
{
    vec4 c = v_color0 * texture2D(s_texColor, v_texcoord0);
	gl_FragColor.xyz = vec3_splat(0.2989*c.r + 0.5870*c.g + 0.1140*c.b);
    gl_FragColor.w = c.a;
}
