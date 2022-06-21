//
// Created by caiiiycuk on 21.06.22.
//

#ifndef VANGERS_SYS_MODULES_H
#define VANGERS_SYS_MODULES_H

#include <duktape.h>

#include <string>
#include <unordered_map>

void setScriptsFolder(duk_context* ctx, const char* folder);
const std::unordered_map<std::string, std::string>& getScriptToPathMapping();

void initSysModules(duk_context* ctx);

#endif  // VANGERS_SYS_MODULES_H
