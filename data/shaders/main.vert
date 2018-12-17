#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;
uniform vec4 u_SurfaceSize; // w, h, 0, 0
uniform vec4 u_ScreenSize; // w, h, 0, 0

out vec2 Texcoord;

void main()
{
    vec2 scale =  u_SurfaceSize.xy / u_ScreenSize.xy;

    float wFactor = 4.0f/3.0f / (u_ScreenSize.x / u_ScreenSize.y);

    if(u_SurfaceSize.xy == u_ScreenSize.xy){ // TODO: float equality
        wFactor = 1.0;
    }
    Texcoord = texcoord * scale;
    gl_Position = vec4(position.x * wFactor , -position.y, 0.0f, 1.0f);
}
