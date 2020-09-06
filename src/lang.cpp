#include "lang.h"

namespace {
    Language _lang = RUSSIAN;
}

Language lang() {
    return _lang;
}

void setLang(Language lang) {
    _lang = lang;
}
