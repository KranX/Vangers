#ifndef __TEXT_LANGUAGE_POLICY_H__
#define __TEXT_LANGUAGE_POLICY_H__

#include "legacy_codec.h"
#include "lang.h"

#include <string>

namespace text
{

LegacyEncoding runtime_legacy_encoding(Language language);
bool language_uses_russian_assets(Language language);
bool language_prefers_utf8_assets(Language language);
bool language_prefers_japanese_fonts(Language language);
const char* language_code(Language language);
std::string localized_asset_path(const char* english_path,const char* russian_path = nullptr,const char* japanese_path = nullptr);

inline LegacyEncoding runtime_legacy_encoding(void)
{
	return runtime_legacy_encoding(lang());
}

inline bool language_uses_russian_assets(void)
{
	return language_uses_russian_assets(lang());
}

inline bool language_prefers_utf8_assets(void)
{
	return language_prefers_utf8_assets(lang());
}

inline bool language_prefers_japanese_fonts(void)
{
	return language_prefers_japanese_fonts(lang());
}

}

#endif
