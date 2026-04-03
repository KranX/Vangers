#ifndef __TEXT_LOCALE_CATALOG_H__
#define __TEXT_LOCALE_CATALOG_H__

#include <string>
#include <string_view>

namespace text
{

const std::string* iscreen_locale_string(std::string_view source_text);
const std::string* iscreen_locale_text_path(std::string_view source_path);
const std::string* actint_locale_string(std::string_view source_text);

}

#endif
