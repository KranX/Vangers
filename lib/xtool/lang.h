#ifndef __LANG_H
#define __LANG_H

enum Language {
    ENGLISH = 0,
    RUSSIAN = 1,
    GERMAN = 2,
};

Language lang();
void setLang(Language lang);
#endif
