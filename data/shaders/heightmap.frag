#version 330 core

uniform mat4 u_ViewProj;

//const vec4 u_ScreenSize = vec4(1280.0, 700.0, 0.0, 0.0);		// XY = size
//const vec4 u_TextureScale = vec4(2048.0, 16384.0, 96.0, 1.0);	// XY = size, Z = height scale, w = number of layers

uniform vec4 u_ScreenSize; 	// XY = size
uniform vec4 u_TextureScale;	// XY = size, Z = UNUSED, w = number of layers

uniform usampler2DArray t_Color;
uniform sampler1D t_Palette;


const float c_HorFactor = 0.5; //H_CORRECTION
const float c_ReflectionVariance = 0.5, c_ReflectionPower = 0.1;
const uint c_DoubleLevelMask = 1U<<6, c_ShadowMask = 1U<<7;
const uint c_TerrainShift = 3U, c_TerrainBits = 3U;
const uint c_DeltaShift = 0U, c_DeltaBits = 2U;

#define TERRAIN_WATER	0U

in vec2 Texcoord;
out vec4 Target0;



vec3 palColor(uvec4 color_id){
	float c = color_id.x;
	c = c / 256;
	return texture(t_Palette, c).rgb;
}

vec4 evaluate_color(vec2 tex_coord) {
	float x = tex_coord.x;
	float numChunks = u_TextureScale.w;
//	float chunkSize = u_TextureScale.y / u_TextureScale.w;
//	int chunkSize_i = int(chunkSize);

//	float y = int(tex_coord.y * u_TextureScale.y) % chunkSize_i / chunkSize;
//	float level = int(tex_coord.y * u_TextureScale.y) / chunkSize;
//	float y = tex_coord.y* 8;
	float y = mod(tex_coord.y, 1/numChunks) * numChunks;
	float level = floor(tex_coord.y * numChunks);
	vec3 tex_coord_layer = vec3(x, y, level);

	vec3 tl = palColor(textureOffset(t_Color, tex_coord_layer, ivec2(-1, 0)));
	vec3 tr = palColor(textureOffset(t_Color, tex_coord_layer, ivec2(0, 0)));
	vec3 bl = palColor(textureOffset(t_Color, tex_coord_layer, ivec2(-1, 1)));
	vec3 br = palColor(textureOffset(t_Color, tex_coord_layer, ivec2(0, 1)));
	vec2 f = fract( vec2(tex_coord.x * u_TextureScale.x, tex_coord.y * u_TextureScale.y));

	vec3 top_color = mix(tl, tr, f.x);
	vec3 bottom_color = mix(bl, br, f.x);
	vec3 color = mix(top_color, bottom_color, f.y);

	return vec4(color, 1);
}


void main() {
	vec4 col = evaluate_color(Texcoord);
	Target0 = vec4(col);

}



