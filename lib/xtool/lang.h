#ifndef __LANG_H
#define __LANG_H

#include <string>

enum Language {
	ENGLISH = 0,
	RUSSIAN = 1,
	GERMAN = 2,
};

Language lang();
void setLang(Language lang);
void setLocale(const std::string &id);
#endif
