//
// Created by caiiiycuk on 16.05.2022.
//

#include "sys.h"

#include <duktape.h>
#include <extras/console/duk_console.h>
#include <extras/module-node/duk_module_node.h>

#include <filesystem>
#include <unordered_map>

#include "xgraph.h"

namespace fs = std::filesystem;
using namespace vss;

const char* vss::SYS_EVENT_READY = "sys_ready";
const char* vss::SYS_EVENT_SCALED_RENDERER_CHANGED =
    "sys_scaled_renderer_changed";
const char* vss::SYS_EVENT_RUNTIME_OBJECT_CHANGED =
    "sys_runtime_object_changed";

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

duk_context* ctx = nullptr;

duk_ret_t duk_resolve_module(duk_context* ctx) {
  const char* moduleId;
  const char* parentId;

  moduleId = duk_require_string(ctx, 0);
  parentId = duk_require_string(ctx, 1);

  duk_push_sprintf(ctx, "%s.js", moduleId);
  return 1;
}

duk_ret_t duk_load_module(duk_context* ctx) {
  const char* fileName;
  const char* moduleId;

  moduleId = duk_require_string(ctx, 0);
  duk_get_prop_string(ctx, 2, "filename");
  fileName = duk_require_string(ctx, -1);

  std::string canonicalFile =
      fileName[0] == '.' ? std::string(fileName + 2) : std::string(fileName);

  auto contents = readScriptFile(canonicalFile);
  duk_push_string(ctx, contents.c_str());
  return 1;
}

const duk_function_list_entry vssFunctions[] = {
    {"fatal",
     [](duk_context* ctx) -> duk_ret_t {
       auto message = duk_require_string(ctx, 0);
       ErrH.Abort(message);
       return 0;
     },
     1},
    {"scripts",
     [](duk_context* ctx) -> duk_ret_t {
       auto index = duk_push_array(ctx);
       auto i = 0;
       for (auto& next: scriptToPath) {
         duk_push_string(ctx, next.first.c_str());
         duk_put_prop_index(ctx, index, i);
         ++i;
       }
       return 1;
     },
     1},
};

Sys::Sys() {
  ctx = duk_create_heap(nullptr, nullptr, nullptr, nullptr,
                        [](void* udata, const char* msg) { ErrH.Abort(msg); });
  // init console
  duk_console_init(ctx, 0);

  // init node style module resolve
  duk_push_object(ctx);
  duk_push_c_function(ctx, duk_resolve_module, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "resolve");
  duk_push_c_function(ctx, duk_load_module, DUK_VARARGS);
  duk_put_prop_string(ctx, -2, "load");
  duk_module_node_init(ctx);

  // init bridge object
  duk_push_global_object(ctx);
  duk_push_object(ctx);
  duk_put_function_list(ctx, -1, vssFunctions);
  duk_put_prop_string(ctx, -2, "bridge");
  duk_pop(ctx);
}

Sys::~Sys() { duk_destroy_heap(ctx); }

void Sys::initScripts(const char* folder) {
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

int Sys::rendererWidth() { return XGR_Obj.hdWidth; }

int Sys::rendererHeight() { return XGR_Obj.hdHeight; }

OptionQuantFunction& Sys::getOptionQuantFunction() {
  return optionkQuantFunction;
}

void Sys::setOptionQuantFunction(const OptionQuantFunction& fn) {
  optionkQuantFunction = fn;
}

JoystickQuantFunction& Sys::getJoystickQuantFunction() {
  return joystickQuantFunction;
}

void Sys::setJoystickQuantFunction(const JoystickQuantFunction& fn) {
  joystickQuantFunction = fn;
}

CameraQuantFunction& Sys::getCameraQuantFunction() {
  return cameraQuantFunction;
}

void Sys::setCameraQuantFunction(const CameraQuantFunction& fn) {
  cameraQuantFunction = fn;
}

size_t Sys::addEventListener(const std::function<void(Event)>& listener) {
  auto id = this->listeners.size();
  this->listeners.push_back(listener);
  return id;
}

void Sys::removeEventListener(size_t id) {
  if (id < this->listeners.size()) {
    this->listeners.erase(this->listeners.begin() + id);
  }
}

void Sys::postEvent(const Event& event) {
  for (auto& next : this->listeners) {
    next(event);
  }
}

Sys& vss::sys() {
  static Sys sys;
  return sys;
}

void sys_initScripts(const char* folder) { sys().initScripts(folder); }

void sys_postReadyEvent() { sys().postEvent({.type = SYS_EVENT_READY}); }

void sys_postScaledRendererChangedEvent(bool enabled) {
  sys().postEvent({.type = vss::SYS_EVENT_SCALED_RENDERER_CHANGED,
                   .scaledRenderer = enabled});
}

void sys_postRuntimeObjectChangedEvent(int runtimeObjectId) {
  static int currentRuntimeObjectId = -1;
  if (currentRuntimeObjectId == runtimeObjectId) {
    return;
  }
  currentRuntimeObjectId = runtimeObjectId;
  sys().postEvent({
      .type = vss::SYS_EVENT_RUNTIME_OBJECT_CHANGED,
      .runtimeObjectId = runtimeObjectId,
  });
}
