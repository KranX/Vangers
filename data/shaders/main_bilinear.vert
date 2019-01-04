#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;

uniform vec2 u_Scale; // w, h
uniform float u_wFactor;
out vec2 Texcoord;

void main()
{
//    vec2 scale =  u_SurfaceSize.xy / u_ScreenSize.xy;

//    float wFactor = 4.0f/3.0f / (u_ScreenSize.x / u_ScreenSize.y);

    Texcoord = texcoord * u_Scale;
    gl_Position = vec4(position.x * u_wFactor , -position.y, 0.0f, 1.0f);
}
