
const char* ccGraphicsVert = R"(
in vec4 a_position;
in vec2 a_texCoord;

#ifdef GL_ES
out mediump vec2 v_texCoord;
#else
out vec2 v_texCoord;
#endif

void main()
{
    gl_Position = CC_MVPMatrix * a_position;
    v_texCoord = a_texCoord;
}

)";
