#include "language_policy.h"

#include <cstdio>

namespace text
{

namespace
{

bool asset_exists(const char* path)
{
	if(!path || !*path)
		return false;

	FILE* fh = fopen(path, "rb");
	if(!fh)
		return false;

	fclose(fh);
	return true;
}

}

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

std::string localized_asset_path(const char* english_path,const char* russian_path,const char* japanese_path)
{
	if(language_prefers_utf8_assets() && japanese_path && *japanese_path && asset_exists(japanese_path))
		return japanese_path;

	if(language_uses_russian_assets() && russian_path && *russian_path)
		return russian_path;

	return english_path ? english_path : "";
}

}
