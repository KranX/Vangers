#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;
uniform mat4 u_ViewProj;
uniform vec4 u_TextureScale;

out vec2 Texcoord;

void main()
{
	vec2 corrMul = u_TextureScale.xy * 3 / 2;
	vec2 corrAdd = u_TextureScale.xy / 2;
	vec2 corrPos = position * corrMul + corrAdd;

// -1 * 3w/2 + 1/2w = -w
// 1 * 3w/2 + 1/2w = 2w

// -1 * x + y = -w
// 1 * x + y = 2w


// 0 * x + y = -1
// 1 * x + y = 2

    Texcoord = (texcoord * vec2(3, 3) + vec2(-1, -1)) * vec2(-1, 1);
    gl_Position = u_ViewProj * vec4(corrPos, 0.0, 1.0);
}