#version 150 core
in vec2 Texcoord;

out vec4 outColor;

uniform usampler2D t_Texture;
uniform sampler1D t_Palette;

vec4 palColor(uvec4 color_id){
	float c = color_id.x;
	c = c / 256;
	return texture(t_Palette, c);
}

vec4 evaluate_color(vec2 tex_coord) {
	return palColor(texture(t_Texture, tex_coord));
}

void main()
{
    outColor = evaluate_color(Texcoord);
}