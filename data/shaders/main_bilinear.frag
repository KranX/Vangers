#version 150 core
in vec2 Texcoord;

out vec4 outColor;

uniform vec2 u_TextureSize;
uniform usampler2D t_Texture;
uniform sampler1D t_Palette;

vec4 palColor(uvec4 color_id){
	float c = color_id.x;
	c = c / 256;
	return texture(t_Palette, c);
}

vec4 evaluate_color(vec2 tex_coord) {
	vec4 tl, tr, bl, br;

	tl = palColor(textureOffset(t_Texture, tex_coord, ivec2(0, 0)));
    tr = palColor(textureOffset(t_Texture, tex_coord, ivec2(1, 0)));
    bl = palColor(textureOffset(t_Texture, tex_coord, ivec2(0, 1)));
    br = palColor(textureOffset(t_Texture, tex_coord, ivec2(1, 1)));

	vec2 f = fract(tex_coord * u_TextureSize);

	vec4 top_color = mix(tl, tr, f.x);
	vec4 bottom_color = mix(bl, br, f.x);
	return mix(top_color, bottom_color, f.y);
}

void main()
{
    outColor = evaluate_color(Texcoord);
}