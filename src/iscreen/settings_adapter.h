#ifndef VANGERS_ISCREEN_SETTINGS_ADAPTER_H
#define VANGERS_ISCREEN_SETTINGS_ADAPTER_H

#include <string_view>

namespace vangers::settings {

void apply_settings_to_interface();
void capture_settings_from_interface();
void apply_settings_to_controls();
void capture_settings_from_controls();
void report_settings_diagnostic(std::string_view diagnostic);
bool save_settings();

} // namespace vangers::settings

#endif
