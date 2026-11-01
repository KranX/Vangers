#include "../global.h"

#include "../3d/3d_math.h"
#include "../3d/3dgraph.h"
#include "../3d/3dobject.h"
#include "../3d/parser.h"
#include "../backg.h"
#include "../runtime.h"
#include "../text/utf8.h"
#include "../units/effect.h"
#include "../units/hobj.h"
#include "raster_text.h"

extern int FrameCount;
namespace {
constexpr int CHAR_SPEED = 2;
}

void aWriteHelpString(
	int count,
	int *x,
	int *y,
	int font,
	unsigned char **pstr,
	int timer,
	int color,
	int space,
	unsigned int *pcolor
) {
	int i, total_len = 0;
	int draw_len = (CHAR_SPEED * timer) / GAME_TIME_COEFF;
	std::string tmp;
	int _x;
	int _color = color;

	for (i = 0; i < count; i++) {
		if (pstr[i]) {
			if (pcolor[i])
				_color = pcolor[i];

			total_len += vangers::text::text_length((char *)pstr[i]);
			if (total_len <= draw_len) {
				aOutStrUtf8(x[i], y[i], font, _color, pstr[i], space);
			} else {
				int d = total_len - draw_len;
				const std::string_view text((char *)pstr[i]);
				tmp.assign(text.substr(
					0, vangers::text::prefix_bytes(text, vangers::text::text_length(text) - d)
				));

				aOutStrUtf8(x[i], y[i], font, _color, tmp.c_str(), space);
				_x = x[i] + aStrLenUtf8(tmp.c_str(), font, space);

				EffD.FireBallData[2].Show2(_x, y[i], FrameCount);
				EffD.FireBallData[2].CheckOut(FrameCount);

				return;
			} //  end if
		} //  end if pstr[i]
	} //  end for
}
