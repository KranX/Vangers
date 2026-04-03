#ifndef __TEXT_LOCALE_CATALOG_H__
#define __TEXT_LOCALE_CATALOG_H__

#include <initializer_list>
#include <string>
#include <string_view>

namespace text
{

const std::string* iscreen_locale_string(std::string_view source_text);
const std::string* iscreen_locale_text_path(std::string_view source_path);
const std::string* actint_locale_string(std::string_view source_text);
const std::string* actint_locale_format(std::string_view format_key);
std::string apply_locale_format(std::string_view pattern,std::initializer_list<std::string_view> args);

}

#endif
