#include "language_policy.h"

#include <cstdio>
#include <string>

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

std::string localized_asset_variant_path(const char* base_path,const char* japanese_suffix)
{
	if(!base_path || !*base_path)
		return "";

	if(!language_prefers_utf8_assets() || !japanese_suffix || !*japanese_suffix)
		return base_path;

	std::string localized_path = base_path;
	const std::string::size_type slash_pos = localized_path.find_last_of("/\\");
	const std::string::size_type dot_pos = localized_path.find_last_of('.');

	if(dot_pos == std::string::npos || (slash_pos != std::string::npos && dot_pos < slash_pos))
		localized_path += japanese_suffix;
	else
		localized_path.insert(dot_pos, japanese_suffix);

	if(asset_exists(localized_path.c_str()))
		return localized_path;

	return base_path;
}

}
