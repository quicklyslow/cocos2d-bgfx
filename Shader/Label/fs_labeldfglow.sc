$input v_color0, v_texcoord0

#include "../bgfx_shader.sh"

uniform vec4 u_effectColor;
uniform vec4 u_textColor;
SAMPLER2D(s_texColor, 0);

void main()
{
    float dist = texture2D(s_texColor, v_texcoord0).a;
    float width = fwidth(dist);
    float alpha = smoothstep(0.5-width, 0.5+width, dist);
    float mu = smoothstep(0.5, 1.0, sqrt(dist));
    vec4 color = u_effectColor*(1.0 - alpha) + u_textColor*alpha;
	gl_FragColor = v_color0 * vec4(color.rgb, max(alpha, mu) * color.a);
}