#ifndef VANGERS_ACTINT_MOUSE_PROMPT_H
#define VANGERS_ACTINT_MOUSE_PROMPT_H

#include "../settings/text_encoding.h"
#include "xgraph.h"

namespace vangers::actint {

// XGR is a legacy CP866 boundary. It must own the converted bytes, not borrow
// the temporary string returned by the codec.
inline void set_mouse_prompt(XGR_MousePromptData *prompt, const char *text) {
	prompt->init_text(vangers::settings::utf8_to_cp866(text).c_str());
}

} // namespace vangers::actint

#endif
