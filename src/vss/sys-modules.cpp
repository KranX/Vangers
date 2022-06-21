//
// Created by caiiiycuk on 21.06.22.
//
#include "sys-modules.h"

#include <extras/console/duk_console.h>
#include <extras/module-node/duk_module_node.h>
#include <xerrhand.h>

#include <filesystem>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

std::string scriptsRoot;
std::unordered_map<std::string, std::string> scriptToPath;

std::string readScriptFile(const std::string& fileName) {
  auto jsEntry = scriptToPath.find(fileName);
  if (jsEntry == scriptToPath.end()) {
    ErrH.Abort((std::string("Scripts loader '") + fileName + "' not found in " +
                scriptsRoot)
                   .c_str());
  }
  auto path = jsEntry->second;
  std::ifstream file;
  file.open(path);
  if (file.fail()) {
    ErrH.Abort((std::string("Unable to open file '") + path + "'").c_str());
  }

  std::stringstream contents;
  contents << file.rdbuf();
  file.close();

  return contents.str();
}

duk_ret_t duk_resolve_module(duk_context* ctx) {
  const char* moduleId;
  moduleId = duk_require_string(ctx, 0);
  duk_push_sprintf(ctx, "%s.js", moduleId);
  return 1;
}

duk_ret_t duk_load_module(duk_context* ctx) {
  const char* fileName;
  duk_get_prop_string(ctx, 2, "filename");
  fileName = duk_require_string(ctx, -1);

  std::string canonicalFile =
      fileName[0] == '.' ? std::string(fileName + 2) : std::string(fileName);

  auto contents = readScriptFile(canonicalFile);
  duk_push_string(ctx, contents.c_str());
  return 1;
}

void setScriptsFolder(duk_context* ctx, const char* folder) {
  scriptsRoot = folder;
  if (!fs::is_directory(scriptsRoot)) {
    ErrH.Abort("-vss flag should point to valid directory");
    return;
  }

  for (const auto& entry : fs::directory_iterator(scriptsRoot)) {
    if (entry.is_regular_file()) {
      const char* fileName = entry.path().filename().c_str();
      const char* filePath = entry.path().c_str();
      scriptToPath.insert(std::make_pair<>(fileName, filePath));
    }
  }

  auto loader = std::string("var exports = {};\n") + readScriptFile("main.js");
  duk_push_string(ctx, loader.c_str());
  if (duk_peval(ctx) != 0) {
    std::string error = std::string("Error while executing main.js: ") +
                        duk_safe_to_string(ctx, -1);
    ErrH.Abort(error.c_str());
  }
  duk_pop(ctx);
}

const std::unordered_map<std::string, std::string>& getScriptToPathMapping() {
  return scriptToPath;
}

void initSysModules(duk_context* ctx) {
  // init console
  duk_console_init(ctx, 0);

  // init node style module resolve
  duk_push_object(ctx);
  duk_push_c_function(ctx, duk_resolve_module, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "resolve");
  duk_push_c_function(ctx, duk_load_module, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "load");
  duk_module_node_init(ctx);
}
