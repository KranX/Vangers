#ifndef VANGERS_ACTINT_MOUSE_PROMPT_H
#define VANGERS_ACTINT_MOUSE_PROMPT_H

#include "../settings/text_encoding.h"
#include "raster_text.h"
#include "xgraph.h"

namespace vangers::actint {

// Only actint prompts select UTF-8 callbacks; unrelated XGR callers stay CP866.
// In either mode XGR owns the bytes, including any temporary codec result.
inline void set_mouse_prompt(XGR_MousePromptData *prompt, const char *text) {
	static const XGR_TextCallbacks callbacks{
		[](int x, int y, int colour, void *text, int font, int hspace, int vspace) {
			aOutText32Utf8(x, y, colour, text, font, hspace, vspace);
		},
		[](void *text, int font, int space) { return aTextWidth32Utf8(text, font, space); },
		[](void *text, int font, int space) { return aTextHeight32Utf8(text, font, space); }};
	if (xgr::text::enabled) {
		prompt->textCallbacks = &callbacks;
		prompt->init_text(text);
		return;
	}
	prompt->textCallbacks = nullptr;
	prompt->init_text(vangers::settings::utf8_to_cp866(text).c_str());
}

} // namespace vangers::actint

#endif
