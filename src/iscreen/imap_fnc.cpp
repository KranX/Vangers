/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"

#include "lang.h"
#include "ivmap.h"
#include "iworld.h"

#include "hfont.h"
#include "iscreen.h"
#include "i_mem.h"
#include "ikeys.h"
#include "../text/legacy_codec.h"
#include "../text/language_policy.h"
#include "../text/legacy_ttf_draw.h"
#include "../text/unicode.h"

#include <unordered_map>

/* ----------------------------- EXTERN SECTION ----------------------------- */

extern iScreenDispatcher* iScrDisp;
extern char* iMouseBMP;
extern int IsMainMenu;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void scale_buf(int sx,int sy,int sc,unsigned char* p,unsigned char* p1);
void dscale_buf(int sx,int sy,int sc,unsigned char* p,unsigned char* p1,int dx,int dy,int lev);
void put_buf(int x,int y,int sx,int sy,unsigned char* buf,unsigned char* mask,int lev,int hide_mode);
void put_tbuf(int x,int y,int sx,int sy,unsigned char* buf,unsigned char* mask,int lev,int hide_mode,int terr);
void add_buf(int x,int y,int sx,int sy,unsigned char* buf,int lev);

void put_fon(int x,int y,int sx,int sy,unsigned char* buf);
void put_attr_fon(int x,int y,int sx,int sy,unsigned char* buf);
void put_terr_fon(int x,int y,int sx,int sy,unsigned char* buf);
void get_fon(int x,int y,int sx,int sy,unsigned char* buf);
void get_terr_fon(int x,int y,int sx,int sy,unsigned char* buf);

void put_level(int x,int y,int sx,int sy,int lev);
void get_buf(int x,int y,int sx,int sy,unsigned char* buf);
void get_col_buf(int x,int y,int sx,int sy,unsigned char* buf);
void iPutStr(int x,int y,int fnt,unsigned char* str,int mode,int scale,int space,int level,int hide_mode);
void iPutStrUtf8(int x,int y,int fnt,std::string_view text_value,int mode,int scale,int space,int level,int hide_mode,bool strong_relief);
int iStrLen(unsigned char* p,int f,int space);
int iUtf8StrLen(std::string_view text_value,int f,int space);

void put_map(int x,int y,int sx,int sy);
void change_terrain(int x,int y,int sx,int sy,int terr1,int terr2);

void map_init(void);

#ifdef _DEBUG
void map_rectangle(int x,int y,int sx,int sy,int col);
#endif

/* --------------------------- DEFINITION SECTION --------------------------- */

namespace
{

bool iscreen_debug_title_metrics_enabled(void)
{
	static int enabled = -1;
	if(enabled == -1){
		const char* value = std::getenv("VANGERS_DEBUG_TEXT");
		enabled = (value && (!strcmp(value, "title") || !strcmp(value, "full"))) ? 1 : 0;
	}
	return enabled != 0;
}

bool iscreen_debug_title_candidate(std::string_view text_value)
{
	return iscreen_debug_title_metrics_enabled() &&
	       text_value.find('\n') == std::string_view::npos &&
	       text::utf8_length(text_value) <= 8;
}

text::LegacyEncoding iscreen_hfont_encoding(void)
{
	return text::runtime_legacy_encoding();
}

std::shared_ptr<text::TtfFontFace> iscreen_get_hfont_ttf_face(int fnt)
{
	if(fnt < 0 || fnt >= iNumFonts)
		return nullptr;
	if(!HFntTable || !HFntTable[fnt])
		return nullptr;

	int target_height = HFntTable[fnt]->SizeY;
	int style = TTF_STYLE_NORMAL;
	if(IsMainMenu){
		target_height += 1;
		style = TTF_STYLE_BOLD;
	}

	return text::default_ui_hfont_ttf_face(target_height, TTF_HINTING_NORMAL, false, 0, style);
}

int iscreen_hfont_peak_level(HFont* font)
{
	static std::unordered_map<const HFont*, int> cache;

	if(!font)
		return HFONT_NULL_LEVEL;

	auto it = cache.find(font);
	if(it != cache.end())
		return it->second;

	int peak_level = HFONT_NULL_LEVEL;
	for(int i = 0; i < font->NumChars; i++){
		HChar* ch = font->data[i];
		if(!ch)
			continue;
		if(ch->MaxHeight > peak_level)
			peak_level = ch->MaxHeight;
	}

	cache.emplace(font, peak_level);
	return peak_level;
}

uint32_t iscreen_decode_legacy_char(unsigned char ch,text::LegacyEncoding encoding)
{
	switch(encoding){
		case text::LegacyEncoding::ASCII:
			return ch < 0x80 ? ch : '?';
		case text::LegacyEncoding::CP866:
			return text::cp866_to_unicode(ch);
		case text::LegacyEncoding::CP1251:
			return text::cp1251_to_unicode(ch);
	}

	return '?';
}

const text::GlyphBitmap* iscreen_get_renderable_codepoint(text::TtfFontFace& face,uint32_t codepoint)
{
	const text::GlyphBitmap* glyph = face.get_glyph(codepoint);
	if(glyph && glyph->provided)
		return glyph;

	glyph = face.get_glyph('?');
	if(glyph && glyph->provided)
		return glyph;

	return nullptr;
}

const text::GlyphBitmap* iscreen_get_renderable_glyph(text::TtfFontFace& face,unsigned char ch,text::LegacyEncoding encoding)
{
	return iscreen_get_renderable_codepoint(face, iscreen_decode_legacy_char(ch, encoding));
}

void build_hfont_ttf_cell_codepoint(text::TtfFontFace& face,uint32_t codepoint,int legacy_height,int legacy_peak_level,int scale,bool strong_relief,
                          std::vector<unsigned char>& cell,int& cell_width,int& cell_height,int& left_offs,int& right_offs);

void iscreen_debug_log_hfont_title_render_once(const char* entry,std::string_view text_value,text::TtfFontFace& face,
                                               int legacy_height,int legacy_peak_level,int scale,int space,bool strong_relief)
{
	static bool logged_direct = false;
	static bool logged_terr = false;
	static bool logged_buf = false;

	bool* logged = nullptr;
	if(!strcmp(entry, "direct"))
		logged = &logged_direct;
	else if(!strcmp(entry, "terrain"))
		logged = &logged_terr;
	else if(!strcmp(entry, "buffer"))
		logged = &logged_buf;

	if(!logged || *logged || !iscreen_debug_title_candidate(text_value))
		return;

	*logged = true;

	fprintf(stderr,
	        "[VANGERS_DEBUG_TEXT] title-render entry=%s text=\"%s\" face=\"%s\" pt=%d height=%d ascent=%d descent=%d legacy_h=%d scale=%d space=%d relief=%d\n",
	        entry, std::string(text_value).c_str(), face.get_file_name().c_str(), face.get_point_size(),
	        face.get_height(), face.get_ascent(), face.get_descent(), legacy_height, scale, space, strong_relief ? 1 : 0);

	size_t offset = 0;
	uint32_t codepoint = 0;
	int pen_x = 0;
	std::vector<unsigned char> ttf_cell;
	int left_offs = 0,right_offs = 0,sx = 0,sy = 0;

	while(text::utf8_next(text_value, offset, codepoint)){
		if(codepoint == '\r' || codepoint == '\n')
			continue;

		const text::GlyphBitmap* glyph = iscreen_get_renderable_codepoint(face, codepoint);
		build_hfont_ttf_cell_codepoint(face, codepoint, legacy_height, legacy_peak_level, scale, strong_relief,
		                               ttf_cell, sx, sy, left_offs, right_offs);
		fprintf(stderr,
		        "[VANGERS_DEBUG_TEXT] title-glyph cp=U+%04X glyph=(w=%d h=%d minx=%d maxx=%d adv=%d) cell=(sx=%d sy=%d left=%d right=%d) pen=%d draw=[%d..%d)\n",
		        (unsigned)codepoint,
		        glyph ? glyph->width : 0, glyph ? glyph->height : 0,
		        glyph ? glyph->minx : 0, glyph ? glyph->maxx : 0, glyph ? glyph->advance : 0,
		        sx, sy, left_offs, right_offs, pen_x, pen_x - left_offs, pen_x - left_offs + sx);
		pen_x += sx - right_offs - left_offs + space;
	}
	fprintf(stderr, "[VANGERS_DEBUG_TEXT] title-pen-width entry=%s result=%d\n", entry, pen_x);
}

int iscreen_utf8_leading_trim_hfont(text::TtfFontFace& face,std::string_view text_value)
{
	size_t offset = 0;
	uint32_t codepoint = 0;

	while(text::utf8_next(text_value, offset, codepoint)){
		if(codepoint == '\r')
			continue;
		if(codepoint == '\n')
			break;

		const text::GlyphBitmap* glyph = iscreen_get_renderable_codepoint(face, codepoint);
		if(!glyph)
			continue;

		return std::max(glyph->minx, 0);
	}

	return 0;
}

int iscreen_hfont_floor_level(int legacy_peak_level,bool strong_relief)
{
	if(!strong_relief)
		return HFONT_NULL_LEVEL;

	int extra_relief = (legacy_peak_level - HFONT_NULL_LEVEL) / 3;
	if(extra_relief < 10)
		extra_relief = 10;
	if(extra_relief > 20)
		extra_relief = 20;

	int floor_level = HFONT_NULL_LEVEL + extra_relief;
	if(floor_level > legacy_peak_level)
		floor_level = legacy_peak_level;

	return floor_level;
}

int iscreen_measure_hfont_utf8_width(text::TtfFontFace& face,std::string_view text_value,int legacy_height,int legacy_peak_level,int space)
{
	std::vector<unsigned char> ttf_cell;
	int left_offs = 0,right_offs = 0,sx = 0,sy = 0;
	int max_width = 0;
	int line_min_x = 0;
	int line_max_x = 0;
	bool line_started = false;
	int pen_x = 0;
	size_t offset = 0;
	uint32_t codepoint = 0;

	while(text::utf8_next(text_value, offset, codepoint)){
		if(codepoint == '\r')
			continue;
		if(codepoint == '\n'){
			if(line_started)
				max_width = std::max(max_width, line_max_x - line_min_x);
			else
				max_width = std::max(max_width, 0);
			line_min_x = line_max_x = 0;
			line_started = false;
			pen_x = 0;
			continue;
		}

		build_hfont_ttf_cell_codepoint(face, codepoint, legacy_height, legacy_peak_level, 256, false,
		                               ttf_cell, sx, sy, left_offs, right_offs);
		if(iscreen_debug_title_metrics_enabled() && text_value == u8"前史"){
			const text::GlyphBitmap* glyph = iscreen_get_renderable_codepoint(face, codepoint);
			fprintf(stderr,
			        "[VANGERS_DEBUG_TEXT] title-metric cp=U+%04X glyph=(w=%d h=%d minx=%d maxx=%d adv=%d) cell=(sx=%d sy=%d left=%d right=%d) pen=%d draw=[%d..%d)\n",
			        (unsigned)codepoint,
			        glyph ? glyph->width : 0, glyph ? glyph->height : 0,
			        glyph ? glyph->minx : 0, glyph ? glyph->maxx : 0, glyph ? glyph->advance : 0,
			        sx, sy, left_offs, right_offs, pen_x, pen_x - left_offs, pen_x - left_offs + sx);
		}
		const int draw_x = pen_x - left_offs;
		if(!line_started){
			line_min_x = draw_x;
			line_max_x = draw_x + sx;
			line_started = true;
		}
		else {
			if(draw_x < line_min_x) line_min_x = draw_x;
			if(draw_x + sx > line_max_x) line_max_x = draw_x + sx;
		}

		pen_x += sx - right_offs - left_offs + space;
	}

	if(line_started)
		max_width = std::max(max_width, line_max_x - line_min_x);

	if(iscreen_debug_title_metrics_enabled() && text_value == u8"前史"){
		fprintf(stderr,
		        "[VANGERS_DEBUG_TEXT] title-width text=\"%s\" result=%d span=[%d..%d]\n",
		        std::string(text_value).c_str(), max_width, line_min_x, line_max_x);
	}

	return max_width;
}

void build_hfont_ttf_cell_codepoint(text::TtfFontFace& face,uint32_t codepoint,int legacy_height,int legacy_peak_level,int scale,bool strong_relief,
                          std::vector<unsigned char>& cell,int& cell_width,int& cell_height,int& left_offs,int& right_offs)
{
	const text::GlyphBitmap* glyph = iscreen_get_renderable_codepoint(face, codepoint);
	const int advance = glyph ? std::max(glyph->advance, 0) : 0;
	const int minx = glyph ? glyph->minx : 0;
	const int maxx = glyph ? glyph->maxx : 0;
	const int floor_level = iscreen_hfont_floor_level(legacy_peak_level, strong_relief);
	const int relief_span = std::max(0, legacy_peak_level - floor_level);
	const int base_relief = std::max(0, floor_level - HFONT_NULL_LEVEL);

	left_offs = std::max(0, -minx);
	cell_height = std::max(1, legacy_height);
	const int advance_right = left_offs + advance;
	const int ink_right = glyph ? (left_offs + std::max(maxx, minx + glyph->width)) : advance_right;
	cell_width = std::max(1, std::max(advance_right, ink_right));
	right_offs = std::max(0, cell_width - advance_right);
	cell.assign((size_t)cell_width * (size_t)cell_height, HFONT_NULL_LEVEL);

	if(!glyph || glyph->alpha.empty())
		return;

	const int face_box_height = std::max(1, face.get_ascent() - face.get_descent());
	const int top_pad = std::max(0, (legacy_height - face_box_height) / 2);
	const int draw_x = left_offs + glyph->minx;
	int draw_y = top_pad + face.get_ascent() - glyph->maxy;
	if(draw_y + glyph->height > cell_height)
		draw_y = cell_height - glyph->height;
	if(draw_y < 0)
		draw_y = 0;

	for(int gy = 0; gy < glyph->height; gy++){
		for(int gx = 0; gx < glyph->width; gx++){
			unsigned int alpha = glyph->alpha[(size_t)gy * (size_t)glyph->pitch + (size_t)gx];
			if(strong_relief){
				if(alpha < 32)
					continue;
				if(alpha < 96)
					alpha = 96;
			}
			if(!alpha)
				continue;

			const int px = draw_x + gx;
			const int py = draw_y + gy;
			if(px < 0 || px >= cell_width || py < 0 || py >= cell_height)
				continue;

			int relief = base_relief;
			if(relief_span > 0)
				relief += (int)((alpha * relief_span + 127) / 255);

			int level = (relief * scale) >> 8;
			level += HFONT_NULL_LEVEL;
			if(level < 0)
				level = 0;
			else if(level > 255)
				level = 255;

			cell[(size_t)py * (size_t)cell_width + (size_t)px] = (unsigned char)level;
		}
	}
}

void build_hfont_ttf_cell(text::TtfFontFace& face,unsigned char ch,text::LegacyEncoding encoding,int legacy_height,int legacy_peak_level,int scale,bool strong_relief,
                          std::vector<unsigned char>& cell,int& cell_width,int& cell_height,int& left_offs,int& right_offs)
{
	build_hfont_ttf_cell_codepoint(face, iscreen_decode_legacy_char(ch, encoding), legacy_height, legacy_peak_level, scale, strong_relief,
	                               cell, cell_width, cell_height, left_offs, right_offs);
}

}

void map_init(void)
{
	ivMap -> accept(0,I_RES_Y - 1);
//	ivMap -> render(0,I_RES_Y - 1);
}

void scale_buf(int sx,int sy,int sc,unsigned char* p,unsigned char* p1)
{
	int i,j,index = 0,t,t1;
	if(sc == 256){
		for(i = 0; i < sy; i ++){
			for(j = 0; j < sx; j ++){
				p1[index] = p[index];
				index ++;
			}
		}
	}
	else {
		for(i = 0; i < sy; i ++){
			for(j = 0; j < sx; j ++){
				t = p[index] - HFONT_NULL_LEVEL;
				t1 = (t * sc) >> 8;
				p1[index] = t1 + HFONT_NULL_LEVEL;
				index ++;
			}
		}
	}
}

void dscale_buf(int sx,int sy,int sc,unsigned char* p,unsigned char* p1,int dx,int dy,int lev)
{
	int i,j,index = 0,t,t1,ey = sy - dy;
	if(sc == 256){
		for(i = 0; i < ey; i ++){
			for(j = 0; j < sx - 1; j ++){
				p1[index + j] = p[index + j];
			}
			p1[index + sx - 1] = p[index + sx - 1];
			index += sx;
		}
	}
	else {
		for(i = 0; i < ey; i ++){
			for(j = 0; j < dx; j ++){
				p1[index + j] = p[index + j];
			}
			for(j = dx; j < sx - 1; j ++){
				t = p[index + j] - HFONT_NULL_LEVEL;
				t1 = (t * sc) >> 8;
				p1[index + j] = t1 + HFONT_NULL_LEVEL + lev;
			}
			p1[index + sx - 1] = p[index + sx - 1];
			index += sx;
		}
	}
}

void put_buf(int x,int y,int sx,int sy,unsigned char* buf,unsigned char* mask,int lev,int hide_mode)
{
//std::cout<<"put_buf "<<hide_mode<<std::endl;
	int i,j,index = 0;
	int t,m;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	switch(hide_mode){
		case 0:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					m = mask[index] - HFONT_NULL_LEVEL;
					if(t || m){
						t += lev;
						if(t < 0) t = 0;
						else if(t > 255) t = 255;
						lta[yy][xx] = t;
					}
					index ++;
				}
			}
			break;
		case 1:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					if(t){
						t += lev;
						if(t < 0) t = 0;
						else if(t > 255) t = 255;
						lta[yy][xx] = t;
					}
					index ++;
				}
			}
			break;
		case 2:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					t += lev;
					if(t < 0) t = 0;
					else if(t > 255) t = 255;
					lta[yy][xx] = t;
					index ++;
				}
			}
			break;
	}
}

void change_terrain(int x,int y,int sx,int sy,int terr1,int terr2)
{
	int i,j,t,yy,xx;
	unsigned char** lta = ivMap -> lineT;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			t = GET_TERRAIN(lta[yy][xx + H_SIZE]);
			if(t == terr1){
				SET_TERRAIN(lta[yy][xx + H_SIZE],terr2 << TERRAIN_OFFSET);
			}
		}
	}
}

void put_tbuf(int x,int y,int sx,int sy,unsigned char* buf,unsigned char* mask,int lev,int hide_mode,int terr)
{
	int i,j,index = 0;
	int t,m;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	switch(hide_mode){
		case 0:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					m = mask[index] - HFONT_NULL_LEVEL;
					if(t || m){
						t += lev;
						lta[yy][xx] = (t < 0) ? 0 : t;
						SET_TERRAIN(lta[yy][xx + H_SIZE],terr << TERRAIN_OFFSET);
					}
					index ++;
				}
			}
			break;
		case 1:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					if(t){
						t += lev;
						lta[yy][xx] = (t < 0) ? 0 : t;
						SET_TERRAIN(lta[yy][xx + H_SIZE],terr << TERRAIN_OFFSET);
					}
					index ++;
				}
			}
			break;
		case 2:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					t += lev;
					lta[yy][xx] = (t < 0) ? 0 : t;
					index ++;
				}
			}
			break;
	}
}

void add_buf(int x,int y,int sx,int sy,unsigned char* buf,int lev)
{
	int i,j,index = 0;
	int t,t1;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			t = buf[index];
			if(t){
				t1 = lta[yy][xx] + t - lev;
				if(t1 < 0) t1 = 0;
				if(t1 > 255) t1 = 255;
				lta[yy][xx] = t1;
			}
			index ++;
		}
	}
}

void put_buf2col(int x,int y,int sx,int sy,unsigned char* buf,int null_lev,int n_mode)
{
	int i,j,index = 0;
	unsigned t,l,n = null_lev;
	unsigned char** lta = ivMap -> lineT;
	unsigned char** ltc = ivMap -> lineTcolor;
	int yy,xx;

	if(!n_mode){
		if(n > 255){
			for(j = 0; j < sy; j ++){
				yy = y + j;
				for(i = 0; i < sx; i ++){
					t = buf[index];
					if(t) ltc[yy][x + i] = t;
					index ++;
				}
			}
		}
		else {
			for(j = 0; j < sy; j ++){
				yy = y + j;
				for(i = 0; i < sx; i ++){
					xx = x + i;
					t = buf[index];
					l = lta[yy][xx];
					if(t && n > l)
						ltc[yy][xx] = t;

					index ++;
				}
			}
		}
	}
	else {
		if(n_mode == 1){
			for(j = 0; j < sy; j ++){
				yy = y + j;
				for(i = 0; i < sx; i ++){
					xx = x + i;
					l = lta[yy][xx];
					if(n > l)
						ltc[yy][xx] = buf[index];

					index ++;
				}
			}
		}
		else {
			for(j = 0; j < sy; j ++){
				yy = y + j;
				for(i = 0; i < sx; i ++){
					xx = x + i;
					ltc[yy][xx] = buf[index];

					index ++;
				}
			}
		}
	}
}

void put_frame(int x,int y,int sx,int sy,unsigned char* buf,int null_lev,int n_mode)
{
	int i,j,index = 0,scr_offs = iScreenOffs;
	unsigned t,l,n = null_lev;
	unsigned char** lta = ivMap -> lineT;
	unsigned char** ltc = ivMap -> lineTcolor;
	int yy,xx;

	if(!n_mode){
		for(j = 0; j < sy; j ++){
			yy = iYCYCL(y + j);
			for(i = 0; i < sx; i ++){
				xx = iXCYCL(x + i);
				if(xx >= scr_offs && xx < scr_offs + I_RES_X){
					t = buf[index];
					l = lta[yy][xx];
					if(t && n > l)
						XGR_SetPixel(xx - scr_offs,yy,t);
					else
						XGR_SetPixel(xx - scr_offs,yy,ltc[yy][xx]);
				}
				index ++;
			}
		}
	}
	else {
		for(j = 0; j < sy; j ++){
			yy = iYCYCL(y + j);
			for(i = 0; i < sx; i ++){
				xx = iXCYCL(x + i);
				if(xx >= scr_offs && xx < scr_offs + I_RES_X){
					t = buf[index];
					l = lta[yy][xx];
					if(n > l)
						XGR_SetPixel(xx - scr_offs,yy,t);
					else
						XGR_SetPixel(xx - scr_offs,yy,ltc[yy][xx]);
				}
				index ++;
			}
		}
	}
}

void put_sframe(int x,int y,int sx,int sy,unsigned char* buf,int null_lev,char* shape,int n_mode)
{
	int i,j,s = 0,index = 0;
	int t = 0,l;
	unsigned char** lta = ivMap -> lineT;
	unsigned char** ltc = ivMap -> lineTcolor;
	int yy,xx;

	int scr_offs = iScreenOffs;

	if(shape){
		if(!n_mode){
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					if(xx >= scr_offs && xx < scr_offs + I_RES_X){
						s = shape[index];
						if(s){
							if(s != 1){
								t = buf[index];
								l = ltc[yy][xx] - s;
								if(l < 0) l = 0;
								if(t && null_lev > l)
									XGR_SetPixel(xx,yy,l);
								else
									XGR_SetPixel(xx,yy,ltc[yy][xx]);
							}
							else {
								t = buf[index];
								l = lta[yy][xx];
								if(t && null_lev > l)
									XGR_SetPixel(xx,yy,t);
								else
									XGR_SetPixel(xx,yy,ltc[yy][xx]);
							}
						}
						else
							XGR_SetPixel(xx,yy,ltc[yy][xx]);
					}

					index ++;
				}
			}
		}
		else {
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					if(xx >= scr_offs && xx < scr_offs + I_RES_X){
						s = shape[index];
						if(s){
							if(s != 1){
								l = ltc[yy][xx] - s;
								if(l < 0) l = 0;
								if(null_lev > l)
									XGR_SetPixel(xx,yy,l);
								else
									XGR_SetPixel(xx,yy,ltc[yy][xx]);
							}
							else {
								t = buf[index];
								l = lta[yy][xx];
								if(null_lev > l)
									XGR_SetPixel(xx,yy,t);
								else
									XGR_SetPixel(xx,yy,ltc[yy][xx]);
							}
						}
						else
							XGR_SetPixel(xx,yy,ltc[yy][xx]);
					}
					index ++;
				}
			}
		}
	}
	else {
		if(!n_mode){
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					if(xx >= scr_offs && xx < scr_offs + I_RES_X){
						t = buf[index];
						l = lta[yy][xx];
						if(t && null_lev > l)
							XGR_SetPixel(xx,yy,t);
						else
							XGR_SetPixel(xx,yy,ltc[yy][xx]);
					}

					index ++;
				}
			}
		}
		else {
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					if(xx >= scr_offs && xx < scr_offs + I_RES_X){
						t = buf[index];
						l = lta[yy][xx];
						if(null_lev > l)
							XGR_SetPixel(xx,yy,t);
						else
							XGR_SetPixel(xx,yy,ltc[yy][xx]);
					}
					index ++;
				}
			}
		}
	}
}

void put_frame2scr(int x,int y,int sx,int sy,unsigned char* buf)
{
	x -= iScreenOffs;
	XGR_PutSpr(x,y,sx,sy,buf,XGR_BLACK_FON);
}

void put_buf2buf(int x,int y,int sx,int sy,int bsx,int bsy,unsigned char* buf,unsigned char* buf_to)
{
	int i,scr_index,index,start_x,start_y,end_x,end_y,size_x;

	start_x = (x < 0) ? 0 : x;
	start_y = (y < 0) ? 0 : y;
	end_x = ((x + sx) > bsx) ? bsx : (x + sx);
	end_y = ((y + sy) > bsy) ? bsy : (y + sy);

	size_x = end_x - start_x;
	index = start_x - x + (start_y - y) * sx;
	scr_index = start_x + start_y * bsx;

	if(size_x <= 0)
		return;

	for(i = start_y; i < end_y; i ++){
		memcpy(buf_to + scr_index,buf + index,size_x);
		scr_index += bsx;
		index += sx;
	}
}

void h_put_buf2buf(int x,int y,int sx,int sy,int bsx,int bsy,unsigned char* buf,unsigned char* buf_to)
{
	int i,j,scr_index,index,start_x,start_y,end_x,end_y,size_x;
	int c = 0;

	start_x = (x < 0) ? 0 : x;
	start_y = (y < 0) ? 0 : y;
	end_x = ((x + sx) > bsx) ? bsx : (x + sx);
	end_y = ((y + sy) > bsy) ? bsy : (y + sy);

	size_x = end_x - start_x;
	index = -x + (start_y - y) * sx;
	scr_index = start_y * bsx;

	if(size_x <= 0)
		return;

	for(i = start_y; i < end_y; i ++){
		for(j = start_x; j < end_x; j ++){
			c = buf[index + j];
			if(c != HFONT_NULL_LEVEL)
				buf_to[scr_index + j] = c;
		}
		scr_index += bsx;
		index += sx;
	}
}

void get_fon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			buf[index + i] = lta[yy][xx];
		}
		index += sx;
	}
}

void get_terr_fon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			buf[index + i] = lta[yy][xx + H_SIZE];
		}
		index += sx;
	}
}

void put_fon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	for(j = 0; j < sy; j ++){
		memcpy(lta[y + j] + x,buf + index,sx);
		index += sx;
	}
}

void put_attr_fon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	for(j = 0; j < sy; j ++){
		memcpy(lta[y + j] + x + H_SIZE,buf + index,sx);
		index += sx;
	}
}

void put_terr_fon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			lta[yy][xx + H_SIZE] = buf[index + i];
		}
		index += sx;
	}
}

/*
void put2fon(int x,int y,int sx,int sy,unsigned char* buf,int lev)
{
	int _x,_y,tmp,index,buf_index;

	index = x + y * I_MAP_RES_X;
	buf_index = 0;

	for(_y = 0; _y < sy; _y ++){
		for(_x = 0; _x < sx; _x ++){
			tmp = buf[buf_index + _x] - HFONT_NULL_LEVEL + lev;
			if(tmp < 0) tmp = 0;
			iScrDisp -> FonData[index + _x] = tmp;
		}
		index += I_MAP_RES_X;
		buf_index += sx;
	}
}
*/

void put_map(int x,int y,int sx,int sy)
{
	int j;
	unsigned char** ltc = ivMap -> lineTcolor;
	int yy,x0,x1,bsx,index;

	int scr_offs = iScreenOffs;

	x0 = x;
	x1 = x + sx;

	if(x0 < scr_offs) x0 = scr_offs;
	if(x1 >= scr_offs + I_RES_X) x1 = scr_offs + I_RES_X;
	bsx = x1 - x0;

	if(bsx > 0){
		index = (x0 - scr_offs) + y * XGR_MAXX;

		for(j = 0; j < sy; j ++){
			yy = iYCYCL(y + j);
			unsigned char* dest = XGR_VIDEOBUF + index;
			if(ltc[yy] == NULL){
				memset(dest, 0, bsx);
			}else{
				auto* source = ltc[yy] + x0;
				memcpy(dest, source, bsx);
			}
			index += XGR_MAXX;
		}
	}
}

void put_level(int x,int y,int sx,int sy,int lev)
{
	int i,j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			lta[yy][xx] = lev;
			index ++;
		}
	}
}

void get_buf(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	int vy,xx;

	for(j = 0; j < sy; j ++){
		vy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			buf[index] = *(lta[vy] + xx);
			index ++;
		}
	}
}

void get_col_buf(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,j,index = 0;
	unsigned char** ltc = ivMap -> lineTcolor;
	int vy,xx;

	for(j = 0; j < sy; j ++){
		vy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			buf[index] = *(ltc[vy] + xx);
			index ++;
		}
	}
}

void iPutStr(int x,int y,int fnt,unsigned char* str,int mode,int scale,int space,int level,int hide_mode)
{
	int _x,_y,i,sx,sy,ss,sz = strlen((char*)str);
	HFont* p = HFntTable[fnt];
	HChar* ch;
	unsigned char* ptr,*ptr1;
	auto face = iscreen_get_hfont_ttf_face(fnt);
	std::vector<unsigned char> ttf_cell;
	int left_offs = 0,right_offs = 0;
	const int legacy_peak_level = iscreen_hfont_peak_level(p);

	_x = x;
	_y = y;
	for(i = 0; i < sz; i ++){
		if(str[i] == '\n'){
			_x = x;
			_y += p -> SizeY + space;
			continue;
		}

		if(face){
			build_hfont_ttf_cell(*face, str[i], iscreen_hfont_encoding(), p -> SizeY, legacy_peak_level, scale, false,
			                     ttf_cell, sx, sy, left_offs, right_offs);
			put_buf(_x - left_offs,_y,sx,sy,ttf_cell.data(),ttf_cell.data(),level,hide_mode);
			_x += sx - right_offs - left_offs + space;
			continue;
		}

		ch = p -> data[str[i]];
		ptr = ch -> HeightMap;

		sx = ch -> SizeX;
		sy = ch -> SizeY;
		ss = sx * sy;

		_iALLOC_A_(unsigned char,ptr1,ss);
		scale_buf(sx,sy,scale,ptr,ptr1);
		put_buf(_x - ch -> LeftOffs,_y,sx,sy,ptr1,ptr,level,hide_mode);
		_iFREE_A_(unsigned char,ptr1,ss);
		_x += sx - ch -> RightOffs - ch -> LeftOffs + space;
	}
}

void iPutStrUtf8(int x,int y,int fnt,std::string_view text_value,int mode,int scale,int space,int level,int hide_mode,bool strong_relief)
{
	HFont* p = HFntTable[fnt];
	auto face = iscreen_get_hfont_ttf_face(fnt);
	if(!face){
		std::string legacy_text = text::utf8_to_legacy_lossy(text_value, iscreen_hfont_encoding(), ' ');
		iPutStr(x, y, fnt, (unsigned char*)legacy_text.c_str(), mode, scale, space, level, hide_mode);
		return;
	}

	std::vector<unsigned char> ttf_cell;
	int left_offs = 0,right_offs = 0,sx = 0,sy = 0;
	const int legacy_peak_level = iscreen_hfont_peak_level(p);
	int _x = x - iscreen_utf8_leading_trim_hfont(*face, text_value);
	int _y = y;
	size_t offset = 0;
	uint32_t codepoint = 0;

	iscreen_debug_log_hfont_title_render_once("direct", text_value, *face, p->SizeY, legacy_peak_level, scale, space, strong_relief);

	while(text::utf8_next(text_value, offset, codepoint)){
		if(codepoint == '\r')
			continue;
		if(codepoint == '\n'){
			_x = x;
			_y += p->SizeY + space;
			continue;
		}

		build_hfont_ttf_cell_codepoint(*face, codepoint, p->SizeY, legacy_peak_level, scale, strong_relief, ttf_cell, sx, sy, left_offs, right_offs);
		put_buf(_x - left_offs, _y, sx, sy, ttf_cell.data(), ttf_cell.data(), level, hide_mode);
		_x += sx - right_offs - left_offs + space;
	}
}

void i_terrPutStr(int x,int y,int fnt,unsigned char* str,int mode,int scale,int space,int level,int hide_mode,int terr)
{
	int _x,_y,i,sx,sy,ss,sz = strlen((char*)str);
	HFont* p = HFntTable[fnt];
	HChar* ch;
	unsigned char* ptr,*ptr1;
	auto face = iscreen_get_hfont_ttf_face(fnt);
	std::vector<unsigned char> ttf_cell;
	int left_offs = 0,right_offs = 0;
	const int legacy_peak_level = iscreen_hfont_peak_level(p);

	_x = x;
	_y = y;
	for(i = 0; i < sz; i ++){
		if(str[i] == '\n'){
			_x = x;
			_y += p -> SizeY + space;
			continue;
		}

		if(face){
			build_hfont_ttf_cell(*face, str[i], iscreen_hfont_encoding(), p -> SizeY, legacy_peak_level, scale, false,
			                     ttf_cell, sx, sy, left_offs, right_offs);
			put_tbuf(_x - left_offs,_y,sx,sy,ttf_cell.data(),ttf_cell.data(),level,hide_mode,terr);
			_x += sx - right_offs - left_offs + space;
			continue;
		}

		ch = p -> data[str[i]];
		ptr = ch -> HeightMap;

		sx = ch -> SizeX;
		sy = ch -> SizeY;
		ss = sx * sy;

		_iALLOC_A_(unsigned char,ptr1,ss);
		scale_buf(sx,sy,scale,ptr,ptr1);
		put_tbuf(_x - ch -> LeftOffs,_y,sx,sy,ptr1,ptr,level,hide_mode,terr);
		_iFREE_A_(unsigned char,ptr1,ss);
		_x += sx - ch -> RightOffs - ch -> LeftOffs + space;
	}
}

void i_terrPutStrUtf8(int x,int y,int fnt,std::string_view text_value,int mode,int scale,int space,int level,int hide_mode,int terr,bool strong_relief)
{
	HFont* p = HFntTable[fnt];
	auto face = iscreen_get_hfont_ttf_face(fnt);
	if(!face){
		std::string legacy_text = text::utf8_to_legacy_lossy(text_value, iscreen_hfont_encoding(), ' ');
		i_terrPutStr(x, y, fnt, (unsigned char*)legacy_text.c_str(), mode, scale, space, level, hide_mode, terr);
		return;
	}

	std::vector<unsigned char> ttf_cell;
	int left_offs = 0,right_offs = 0,sx = 0,sy = 0;
	const int legacy_peak_level = iscreen_hfont_peak_level(p);
	int _x = x - iscreen_utf8_leading_trim_hfont(*face, text_value);
	int _y = y;
	size_t offset = 0;
	uint32_t codepoint = 0;

	iscreen_debug_log_hfont_title_render_once("terrain", text_value, *face, p->SizeY, legacy_peak_level, scale, space, strong_relief);

	while(text::utf8_next(text_value, offset, codepoint)){
		if(codepoint == '\r')
			continue;
		if(codepoint == '\n'){
			_x = x;
			_y += p->SizeY + space;
			continue;
		}

		build_hfont_ttf_cell_codepoint(*face, codepoint, p->SizeY, legacy_peak_level, scale, strong_relief, ttf_cell, sx, sy, left_offs, right_offs);
		put_tbuf(_x - left_offs, _y, sx, sy, ttf_cell.data(), ttf_cell.data(), level, hide_mode, terr);
		_x += sx - right_offs - left_offs + space;
	}
}

void iPutStr2buf(int x,int y,int fnt,int bsx,int bsy,unsigned char* str,unsigned char* buf_to,int mode,int scale,int space)
{
	int _x,_y,i,sx,sy,ss,sz = strlen((char*)str);
	HFont* p = HFntTable[fnt];
	HChar* ch;
	unsigned char* ptr;
	auto face = iscreen_get_hfont_ttf_face(fnt);
	std::vector<unsigned char> ttf_cell;
	int left_offs = 0,right_offs = 0;
	const int legacy_peak_level = iscreen_hfont_peak_level(p);

	_x = x;
	_y = y;
	for(i = 0; i < sz; i ++){
		if(str[i] == '\n'){
			_x = x;
			_y += p -> SizeY + space;
			continue;
		}

		if(face){
			build_hfont_ttf_cell(*face, str[i], iscreen_hfont_encoding(), p -> SizeY, legacy_peak_level, 256, false,
			                     ttf_cell, sx, sy, left_offs, right_offs);
			h_put_buf2buf(_x - left_offs,_y,sx,sy,bsx,bsy,ttf_cell.data(),buf_to);
			_x += sx - right_offs - left_offs + space;
			continue;
		}

		ch = p -> data[str[i]];
		ptr = ch -> HeightMap;

		sx = ch -> SizeX;
		sy = ch -> SizeY;
		ss = sx * sy;

		h_put_buf2buf(_x - ch -> LeftOffs,_y,sx,sy,bsx,bsy,ptr,buf_to);
		_x += sx - ch -> RightOffs - ch -> LeftOffs + space;
	}
}

void iPutStr2bufUtf8(int x,int y,int fnt,int bsx,int bsy,std::string_view text_value,unsigned char* buf_to,int mode,int scale,int space)
{
	HFont* p = HFntTable[fnt];
	auto face = iscreen_get_hfont_ttf_face(fnt);
	if(!face){
		std::string legacy_text = text::utf8_to_legacy_lossy(text_value, iscreen_hfont_encoding(), ' ');
		iPutStr2buf(x, y, fnt, bsx, bsy, (unsigned char*)legacy_text.c_str(), buf_to, mode, scale, space);
		return;
	}

	std::vector<unsigned char> ttf_cell;
	int left_offs = 0,right_offs = 0,sx = 0,sy = 0;
	const int legacy_peak_level = iscreen_hfont_peak_level(p);
	int _x = x - iscreen_utf8_leading_trim_hfont(*face, text_value);
	int _y = y;
	size_t offset = 0;
	uint32_t codepoint = 0;

	iscreen_debug_log_hfont_title_render_once("buffer", text_value, *face, p->SizeY, legacy_peak_level, 256, space, false);

	while(text::utf8_next(text_value, offset, codepoint)){
		if(codepoint == '\r')
			continue;
		if(codepoint == '\n'){
			_x = x;
			_y += p->SizeY + space;
			continue;
		}

		build_hfont_ttf_cell_codepoint(*face, codepoint, p->SizeY, legacy_peak_level, 256, false, ttf_cell, sx, sy, left_offs, right_offs);
		h_put_buf2buf(_x - left_offs, _y, sx, sy, bsx, bsy, ttf_cell.data(), buf_to);
		_x += sx - right_offs - left_offs + space;
	}
}

int iStrLen(unsigned char* p,int f,int space)
{
	int i,len = 0,sz = strlen((char*)p);
	unsigned char s;

	HFont* fnt = HFntTable[f];

	for(i = 0; i < sz; i ++){
		s = p[i];
		len += space + fnt -> SizeX - (fnt -> data[s] -> LeftOffs + fnt -> data[s] -> RightOffs);
	}
	return len;
}

int iUtf8StrLen(std::string_view text_value,int f,int space)
{
	HFont* p = HFntTable[f];
	auto face = iscreen_get_hfont_ttf_face(f);
	if(face)
		return iscreen_measure_hfont_utf8_width(*face, text_value, p->SizeY, iscreen_hfont_peak_level(p), space);

	std::string legacy_text = text::utf8_to_legacy_lossy(text_value, iscreen_hfont_encoding(), ' ');
	return iStrLen((unsigned char*)legacy_text.c_str(), f, space);
}

int iS_StrLen(unsigned char* str,int font,int space)
{
	int s,sz = strlen((char*)str),len = 0;
	for(s = 0; s < sz; s ++)
		len += iScrFontTable[font] -> SizeX - (iScrFontTable[font] -> LeftOffs[str[s]] + iScrFontTable[font] -> RightOffs[str[s]]) + space;
	return len;
}

void iPutS_Str(int x,int y,int font,int color,unsigned char* str,int bsx,int bsy,unsigned char* buf,int space)
{
	int i,j,s,xs,ys,offs = 0,sz = strlen((char*)str),X = 0,col = color;
	int buf_index = 0,buf_offs = 0;

	if(iScrFontTable[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	char* p = iScrFontTable[font] -> data;
	xs = iScrFontTable[font] -> SizeX;
	ys = iScrFontTable[font] -> SizeY;

	buf_index = x + y * bsx;

	for(s = 0; s < sz; s ++){
		buf_offs = 0;
		offs = ys * str[s];

		col = color;

		X -= iScrFontTable[font] -> LeftOffs[str[s]];

		offs = ys * str[s];
		for(i = 0; i < ys; i ++){
			for(j = 0; j < xs; j ++){
				if(p[offs] & (1 << (7 - j)))
					buf[X + j + buf_index + buf_offs] = col;
			}
			offs ++;
			buf_offs += bsx;
		}
		X += xs + space - iScrFontTable[font] -> RightOffs[str[s]];
	}
/*
	XStream fh("s_str.bmp",XS_OUT);
	fh < (short)bsx < (short)bsy;
	fh.write(buf,bsx * bsy);
	fh.close();
*/
}

#ifdef _DEBUG
void map_rectangle(int x,int y,int sx,int sy,int col)
{
	int i,scr_offs = iScreenOffs;
	unsigned char** ltc = ivMap -> lineTcolor;
	int yy,xx;

	yy = iYCYCL(y);
	for(i = 0; i < sx; i ++){
		xx = iXCYCL(x + i);
		if(xx >= scr_offs && xx < scr_offs + I_RES_X){
			ltc[yy][xx] = col;
		}
	}
	yy = iYCYCL(y + sy);
	for(i = 0; i < sx; i ++){
		xx = iXCYCL(x + i);
		if(xx >= scr_offs && xx < scr_offs + I_RES_X){
			ltc[yy][xx] = col;
		}
	}

	xx = iXCYCL(x);
	if(xx >= scr_offs && xx < scr_offs + I_RES_X){
		for(i = 0; i < sy; i ++){
			yy = iYCYCL(y + i);
			ltc[yy][xx] = col;
		}
	}
	xx = iXCYCL(x + sx);
	if(xx >= scr_offs && xx < scr_offs + I_RES_X){
		for(i = 0; i < sy; i ++){
			yy = iYCYCL(y + i);
			ltc[yy][xx] = col;
		}
	}
}
#endif
