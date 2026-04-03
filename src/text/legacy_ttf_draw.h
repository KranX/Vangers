#ifndef __TEXT_LEGACY_TTF_DRAW_H__
#define __TEXT_LEGACY_TTF_DRAW_H__

#include <string>
#include <string_view>
#include <memory>

#include "font_manager.h"
#include "legacy_codec.h"

namespace text
{

const std::string& default_ui_ttf_font_path(void);
std::shared_ptr<TtfFontFace> default_ui_ttf_face(int target_height,int hinting = TTF_HINTING_NORMAL,bool kerning = false,int outline = 0,int style = TTF_STYLE_NORMAL);
std::shared_ptr<TtfFontFace> default_ui_hfont_ttf_face(int legacy_height,int hinting = TTF_HINTING_NORMAL,bool kerning = false,int outline = 0,int style = TTF_STYLE_NORMAL);
std::shared_ptr<TtfFontFace> default_ui_text32_ttf_face(int legacy_height,int hinting = TTF_HINTING_NORMAL,bool kerning = false,int outline = 0,int style = TTF_STYLE_NORMAL);
int default_ui_text32_extra_hspace(void);
const GlyphBitmap* default_ui_renderable_glyph(TtfFontFace& face,uint32_t codepoint);

int measure_legacy_ttf_text_width(std::string_view text,TtfFontFace& face,LegacyEncoding encoding,int hspace);
int measure_legacy_ttf_text_height(std::string_view text,const TtfFontFace& face,int vspace);
void draw_legacy_ttf_text_8bit(int x,int y,int color,std::string_view text,TtfFontFace& face,LegacyEncoding encoding,int hspace,int vspace,bool clip);
int measure_utf8_text_width(std::string_view text,TtfFontFace& face,int hspace);
int measure_utf8_text_height(std::string_view text,const TtfFontFace& face,int vspace);
void draw_utf8_text_8bit(int x,int y,int color,std::string_view text,TtfFontFace& face,int hspace,int vspace,bool clip);

}

#endif
