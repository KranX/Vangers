#version 330 core

uniform vec4 u_CamPos;
uniform mat4 u_ViewProj;
uniform mat4 u_InvViewProj;

//const vec4 u_ScreenSize = vec4(1280.0, 720.0, 0.0, 0.0);		// XY = size
uniform vec4 u_ScreenSize; 	// XY = size
uniform vec4 u_TextureScale;	// XY = size, Z = height scale, w = number of layers
//const vec4 u_TextureScale = vec4(2048.0, 16384.0, 96.0, 1.0);	// XY = size, Z = height scale, w = number of layers


uniform usampler2DArray t_Height;
uniform usampler2DArray t_Color;
uniform sampler1D t_Palette;
uniform usampler2DArray t_Meta;


const float c_HorFactor = 0.5; //H_CORRECTION
const float c_ReflectionVariance = 0.5, c_ReflectionPower = 0.1;
const uint c_DoubleLevelMask = 1U<<6, c_ShadowMask = 1U<<7;
const uint c_TerrainShift = 3U, c_TerrainBits = 3U;
const uint c_DeltaShift = 0U, c_DeltaBits = 2U;

#define TERRAIN_WATER	0U

//in vec2 Texcoord;
out vec4 Target0;


struct Surface {
	float low_alt, high_alt, delta;
	uint low_type, high_type;
	vec3 tex_coord;
//	bool is_shadowed;
};

struct CastPoint {
	vec3 pos;
	uint type;
	vec3 tex_coord;
//	bool is_underground;
//	bool is_shadowed;
};


vec3 palColor(uvec4 color_id){
	float c = color_id.x;
	c = c / 256;
	return texture(t_Palette, c).rgb;
}

uint get_terrain_type(uint meta) {
	return (meta >> c_TerrainShift) & ((1U << c_TerrainBits) - 1U);
}
uint get_delta(uint meta) {
	return (meta >> c_DeltaShift) & ((1U << c_DeltaBits) - 1U);
}

Surface get_surface(vec2 pos) {
	float y = pos.y;
	float chunkSize = u_TextureScale.y / u_TextureScale.w;

	vec3 tc = vec3(pos / u_TextureScale.xy, 0.0f);
	tc.x = 1 - tc.x;
    tc.z = trunc(y / chunkSize);
	tc.y = mod(y, chunkSize) / chunkSize;


	Surface suf;
	suf.tex_coord = tc;
	uint meta = texture(t_Meta, tc).x;
//	suf.is_shadowed = (meta & c_ShadowMask) != 0U;
	suf.low_type = get_terrain_type(meta);
	if ((meta & c_DoubleLevelMask) != 0U) {
		//TODO: we need either low or high for the most part
		// so this can be more efficient with a boolean param
		uint delta;
		if (!(mod(pos.x, 2.0) >= 1.0)) { // high
			uint meta_low = textureOffset(t_Meta, tc, ivec2(-1, 0)).x;
			suf.high_type = suf.low_type;
			suf.low_type = get_terrain_type(meta_low);
			delta = get_delta(meta_low) << c_DeltaBits + get_delta(meta);
		}else { // low
			uint meta_high = textureOffset(t_Meta, tc, ivec2(1, 0)).x;
			suf.tex_coord.x += 1.0 / u_TextureScale.x;
			suf.high_type = get_terrain_type(meta_high);
			delta = get_delta(meta) << c_DeltaBits + get_delta(meta_high);
		}
		float lh = textureOffset(t_Height, suf.tex_coord, ivec2(-1, 0)).x;
		float hh = texture(t_Height, suf.tex_coord).x;
		lh = lh / 256.0;
		hh = hh / 256.0;

		suf.low_alt = lh * u_TextureScale.z;
		suf.high_alt = hh * u_TextureScale.z;
		suf.delta = float(delta) * u_TextureScale.z / 256.0;
	}else {
		float h = texture(t_Height, tc).x;
		h = h / 256.0;
		suf.low_alt = h * u_TextureScale.z;
		suf.high_alt = suf.low_alt;
		suf.high_type = suf.low_type;
		suf.delta = 0.0;
	}
	suf.low_alt -= u_TextureScale.z/2;
	suf.high_alt -= u_TextureScale.z/2;
	return suf;
}


vec3 cast_ray_to_plane(float level, vec3 base, vec3 dir) {
	float t = (level - base.z) / dir.z;
	return t * dir + base;
}

Surface cast_ray_impl(
	inout vec3 a, inout vec3 b,
	bool high, int num_forward, int num_binary
) {
	vec3 step = (1.0 / float(num_forward + 1)) * (b - a);
	Surface result;

	for (int i=0; i<num_forward; ++i) {
		vec3 c = a + step;
		Surface suf = get_surface(c.xy);

		float height = mix(suf.low_alt, suf.high_alt, high);
		if (c.z < height) {
			result = suf;
			b = c;
			break;
		} else {
			a = c;
		}
	}

	for (int i=0; i<num_binary; ++i) {
		vec3 c = mix(a, b, 0.5);
		Surface suf = get_surface(c.xy);
		float height = mix(suf.low_alt, suf.high_alt, high);
		if (c.z < height) {
			result = suf;
			b = c;
		} else {
			a = c;
		}
	}

	return result;
}




vec4 evaluate_color(vec3 tex_coord) {
	vec3 tl = palColor(textureOffset(t_Color, tex_coord, ivec2(-1, 0)));
	vec3 tr = palColor(textureOffset(t_Color, tex_coord, ivec2(0, 0)));
	vec3 bl = palColor(textureOffset(t_Color, tex_coord, ivec2(-1, 1)));
	vec3 br = palColor(textureOffset(t_Color, tex_coord, ivec2(0, 1)));
	vec2 f = fract( vec2(tex_coord.x * u_TextureScale.x, tex_coord.y * u_TextureScale.y));

	vec3 top_color = mix(tl, tr, f.x);
	vec3 bottom_color = mix(bl, br, f.x);
	vec3 color = mix(top_color, bottom_color, f.y);

	return vec4(color, 1);
}

CastPoint cast_ray_to_map(vec3 base, vec3 dir) {
	vec3 a = cast_ray_to_plane(u_TextureScale.z/2, base, dir);
	vec3 c = cast_ray_to_plane(-u_TextureScale.z/2, base, dir);
	vec3 b = c;
	Surface suf = cast_ray_impl(a, b, true, 7, 7);
	CastPoint result;
	result.type = suf.high_type;
	if (suf.low_alt <= b.z && b.z < suf.high_alt - 2) {
		// continue the cast underground
		a = b; b = c;
		suf = cast_ray_impl(a, b, false, 3, 3);
	}

	//float t = a.z > a.w + 0.1 ? (b.w - a.w - b.z + a.z) / (a.z - a.w) : 0.5;
	result.pos = b;
	result.tex_coord = suf.tex_coord;
	return result;
}

void main() {
	vec4 sp_ndc = vec4((gl_FragCoord.xy / u_ScreenSize.xy) * 2.0 - 1.0, 0.0, 1.0);
	vec4 sp_world = u_InvViewProj * sp_ndc;
	vec3 view = normalize(sp_world.xyz / sp_world.w - u_CamPos.xyz);
	CastPoint pt = cast_ray_to_map(u_CamPos.xyz, view);

	uint meta = texture(t_Meta, pt.tex_coord).x;
	float metab = (meta & c_DoubleLevelMask) != 0U ? 0.0f : 0.0f;

	vec4 metac = vec4(metab, metab, 0, 1.0f);
	vec4 col = evaluate_color(pt.tex_coord);
//	vec4 col = evaluate_color(UV);

	col = mix(col, metac, metab);


	Target0 = vec4(col);

	if (pt.type == TERRAIN_WATER) {
		vec3 a = pt.pos;
		vec2 variance = mod(a.xy, c_ReflectionVariance);
		vec3 reflected = normalize(view * vec3(1.0 + variance, -1.0));
		vec3 outside = cast_ray_to_plane(u_TextureScale.z, a, reflected);
		vec3 b = outside;
		Surface suf = cast_ray_impl(a, b, true, 4, 4);

		if (b != outside) {
			CastPoint other;
			other.pos = b;
			other.type = suf.high_type;
			other.tex_coord = suf.tex_coord;
			Target0 += c_ReflectionPower * evaluate_color(other.tex_coord);
		}else {
		}
	}
}


