#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;
uniform vec4 u_Scale; // w, h, 0, 0
uniform vec4 u_ScreenSize; // w, h, 0, 0

out vec2 Texcoord;

void main()
{
//    float offsetX = 1 - 1/u_Scale.x;
//    float offsetY = 1 - 1/u_Scale.y;
//    float wFactor = 4.0f/3.0f / (u_ScreenSize.x / u_ScreenSize.y);
    Texcoord = texcoord;
//    gl_Position = vec4((position.x + offsetX) * u_Scale.y * wFactor, (-position.y - offsetY) * u_Scale.y, 0.0f, 1.0f);
    gl_Position = vec4(position.x , -position.y, 0.0f, 1.0f);
}
