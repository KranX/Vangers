#include "lang.h"
#include "localization/catalog.h"

namespace {
Language _lang = ENGLISH;
}

Language lang() {
	return _lang;
}

void setLang(Language lang) {
	_lang = lang;
	vangers::localization::request_locale(lang == RUSSIAN ? "ru" : lang == GERMAN ? "de" : "en");
}

void setLocale(const std::string &id) {
	vangers::localization::request_locale(id);
	_lang = vangers::localization::catalog().legacy_russian() ? RUSSIAN : ENGLISH;
}
