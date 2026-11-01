#ifndef VANGERS_ACTINT_RASTER_TEXT_H
#define VANGERS_ACTINT_RASTER_TEXT_H

// UTF-8 entry points for actint. The original bitmap APIs remain CP866 for
// iscreen, chat, ACS and other not-yet-migrated callers.
int aStrLenUtf8(const void *text, int font, int space);
int aStrLen32Utf8(const void *text, int font, int space);
void aPutStrUtf8(
	int x,
	int y,
	int font,
	int color,
	const void *text,
	int bsx,
	unsigned char *buf,
	int space
);
void aPutStr32Utf8(
	int x,
	int y,
	int font,
	int color,
	int color_size,
	const void *text,
	int bsx,
	void *buf,
	int space
);
void aOutStrUtf8(int x, int y, int font, int color, const void *text, int space);
void aOutText32Utf8(int x, int y, int color, const void *text, int font, int hspace, int vspace);
int aTextWidth32Utf8(const void *text, int font, int hspace);
int aTextHeight32Utf8(const void *text, int font, int vspace);

#endif
