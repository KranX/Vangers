#include "lang.h"

namespace {
    Language _lang = ENGLISH;
}

Language lang() {
    return _lang;
}

void setLang(Language lang) {
    _lang = lang;
}
