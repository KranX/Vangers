#include "locale_catalog.h"

#include "language_policy.h"
#include "unicode.h"

#include <fstream>
#include <string>
#include <unordered_map>

namespace text
{

namespace
{

struct LocaleCatalog
{
	std::unordered_map<std::string,std::string> strings;
	std::unordered_map<std::string,std::string> text_paths;
};

LocaleCatalog g_iscreen_catalog;
Language g_iscreen_catalog_language = ENGLISH;
bool g_iscreen_catalog_loaded = false;

std::string trim_copy(std::string_view value)
{
	size_t begin = 0;
	size_t end = value.size();

	while(begin < end && (value[begin] == ' ' || value[begin] == '\t' || value[begin] == '\r'))
		begin++;
	while(end > begin && (value[end - 1] == ' ' || value[end - 1] == '\t' || value[end - 1] == '\r'))
		end--;

	return std::string(value.substr(begin, end - begin));
}

void load_catalog_file(const std::string& path,LocaleCatalog& catalog)
{
	std::ifstream input(path, std::ios::binary);
	if(!input)
		return;

	enum class Section
	{
		None,
		Strings,
		TextPaths
	};

	Section section = Section::None;
	std::string line;
	bool first_line = true;

	while(std::getline(input, line)){
		if(first_line){
			first_line = false;
			const size_t bom = utf8_bom_size(line);
			if(bom)
				line.erase(0, bom);
		}

		if(!line.empty() && line.back() == '\r')
			line.pop_back();

		const std::string trimmed = trim_copy(line);
		if(trimmed.empty() || trimmed[0] == '#' || trimmed[0] == ';')
			continue;

		if(trimmed == "[strings]"){
			section = Section::Strings;
			continue;
		}

		if(trimmed == "[text_paths]"){
			section = Section::TextPaths;
			continue;
		}

		const size_t eq = trimmed.find('=');
		if(eq == std::string::npos)
			continue;

		const std::string key = trim_copy(std::string_view(trimmed).substr(0, eq));
		const std::string value = trim_copy(std::string_view(trimmed).substr(eq + 1));
		if(key.empty())
			continue;

		switch(section){
			case Section::Strings:
				catalog.strings[key] = value;
				break;
			case Section::TextPaths:
				catalog.text_paths[key] = value;
				break;
			default:
				break;
		}
	}
}

void ensure_iscreen_catalog_loaded(void)
{
	const Language current_language = lang();
	if(g_iscreen_catalog_loaded && g_iscreen_catalog_language == current_language)
		return;

	g_iscreen_catalog = LocaleCatalog();
	g_iscreen_catalog_language = current_language;
	g_iscreen_catalog_loaded = true;

	if(!language_prefers_utf8_assets(current_language))
		return;

	const char* code = language_code(current_language);
	if(!code || !*code)
		return;

	const std::string path = std::string("resource/iscreen/locale/") + code + "/frontend.txt";
	load_catalog_file(path, g_iscreen_catalog);
}

const std::string* lookup_catalog_value(const std::unordered_map<std::string,std::string>& values,std::string_view key)
{
	const auto it = values.find(std::string(key));
	if(it == values.end())
		return nullptr;
	return &it->second;
}

}

const std::string* iscreen_locale_string(std::string_view source_text)
{
	ensure_iscreen_catalog_loaded();
	return lookup_catalog_value(g_iscreen_catalog.strings, source_text);
}

const std::string* iscreen_locale_text_path(std::string_view source_path)
{
	ensure_iscreen_catalog_loaded();
	return lookup_catalog_value(g_iscreen_catalog.text_paths, source_path);
}

}
