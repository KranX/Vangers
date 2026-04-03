#include "language_policy.h"

namespace text
{

LegacyEncoding runtime_legacy_encoding(Language language)
{
	return language == RUSSIAN ? LegacyEncoding::CP866 : LegacyEncoding::ASCII;
}

bool language_uses_russian_assets(Language language)
{
	return language == RUSSIAN;
}

bool language_prefers_utf8_assets(Language language)
{
	return language == JAPANESE;
}

bool language_prefers_japanese_fonts(Language language)
{
	return language == JAPANESE;
}

const char* language_code(Language language)
{
	switch(language){
		case ENGLISH:
			return "eng";
		case RUSSIAN:
			return "rus";
		case GERMAN:
			return "ger";
		case JAPANESE:
			return "jpn";
	}

	return "eng";
}

}
