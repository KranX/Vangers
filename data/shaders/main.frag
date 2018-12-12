#version 150 core
in vec2 Texcoord;

out vec4 outColor;
uniform sampler2D t_Texture;

void main()
{
    outColor = texture(t_Texture, Texcoord);
}