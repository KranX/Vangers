#include "xgraph.h"

extern XGR_TextWidthHandler XGR_TextXFnc;
extern XGR_TextHeightHandler XGR_TextYFnc;

XGR_MousePromptData::XGR_MousePromptData(int x, int y, int sx, int sy, int fnt)
	: XGR_MousePromptData() {
	StartX = x;
	StartY = y;

	SizeX = sx;
	SizeY = sy;

	font = fnt;
}

XGR_MousePromptData::XGR_MousePromptData(void) {
	ID = 0;
	StartX = StartY = 0;
	SizeX = SizeY = 0;
	textSizeX = textSizeY = 0;
	flags = 0;

	font = XGR_FONT0_8x16;

	textData = NULL;
}

XGR_MousePromptData::~XGR_MousePromptData(void) {
	if (textData && flags & XGR_PROMPT_MEM_ALLOC)
		delete[] textData;
}

void XGR_MousePromptData::init_text(const char *p) {
	// Inventory hover can submit the same text every frame.
	if (textData && (flags & XGR_PROMPT_MEM_ALLOC) && !strcmp(textData, p)) {
		init();
		return;
	}

	// Copy first: p may refer to our current buffer (or a substring of it).
	auto *copy = new char[strlen(p) + 1];
	strcpy(copy, p);
	if (flags & XGR_PROMPT_MEM_ALLOC)
		delete[] textData;
	textData = copy;

	flags |= XGR_PROMPT_MEM_ALLOC;
	init();
}

void XGR_MousePromptData::set_text(const char *p) {
	if (p == textData) {
		init();
		return;
	}
	if (flags & XGR_PROMPT_MEM_ALLOC)
		delete[] textData;
	textData = const_cast<char *>(p);
	flags &= ~XGR_PROMPT_MEM_ALLOC;

	init();
}

void XGR_MousePromptData::init(void) {
	if (!textData)
		return;

	if (XGR_TextXFnc)
		textSizeX = (*XGR_TextXFnc)(textData, font, 1);
	else
		textSizeX = XGR_TextWidth(textData, font);

	if (XGR_TextYFnc)
		textSizeY = (*XGR_TextYFnc)(textData, font, 1);
	else
		textSizeY = XGR_TextHeight(textData, font);
}

int XGR_MousePromptData::check_xy(int x, int y) {
	if (x >= StartX && x < (StartX + SizeX) && y >= StartY && y < (StartY + SizeY))
		return 1;
	return 0;
}
