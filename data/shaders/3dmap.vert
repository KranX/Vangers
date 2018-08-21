#version 150 core
in vec2 position;
in vec2 texcoord;
uniform mat4 u_ViewProj;

//out vec2 Texcoord;

void main()
{
//    Texcoord = texcoord;
    gl_Position = u_ViewProj * vec4(position, 0.0, 1.0);
}