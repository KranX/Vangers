//
// Created by caiiiycuk on 21.06.22.
//

#include <duktape.h>
#include <xerrhand.h>

#include "sys.h"

using namespace vss;

QuantResult::QuantResult(std::shared_ptr<Context>& context) : context(context) {
  ctx = context ? context->ctx : nullptr;
  notHandled = ctx == nullptr || !duk_is_object(ctx, -1);
  preventDefault = ctx != nullptr && duk_is_string(ctx, -1) &&
                   strcmp(duk_get_string(ctx, -1), "preventDefault") == 0;
}

QuantResult::~QuantResult() {
  if (ctx != nullptr) {
    duk_pop(ctx);
  }
}

bool QuantResult::isNotHandled() { return notHandled; }

bool QuantResult::isPreventDefault() { return preventDefault; }

int QuantResult::getInt(const char* name, int defaultValue) {
  if (notHandled || preventDefault) {
    return defaultValue;
  }
  int value = defaultValue;
  if (duk_get_prop_string(ctx, -1, name)) {
    value = duk_to_int(ctx, -1);
  }
  duk_pop(ctx);
  return value;
}

bool QuantResult::getBool(const char* name, bool defaultValue) {
  if (notHandled || preventDefault) {
    return defaultValue;
  }
  bool value = defaultValue;
  if (duk_get_prop_string(ctx, -1, name)) {
    value = duk_to_boolean(ctx, -1);
  }
  duk_pop(ctx);
  return value;
}

QuantBuilder::QuantBuilder(std::shared_ptr<Context>& context,
                           const char* eventName)
    : context(context) {
  ctx = context ? context->ctx : nullptr;
  valid = ctx != nullptr && duk_get_global_string(ctx, "onVssQuant");
  if (valid) {
    duk_push_string(ctx, eventName);
    duk_push_object(ctx);
  }
}

QuantBuilder& QuantBuilder::prop(const char* name, int value) {
  if (!valid) {
    return *this;
  }

  duk_push_string(ctx, name);
  duk_push_int(ctx, value);
  if (duk_put_prop(ctx, -3) != 1) {
    ErrH.Abort("vss: unable to set property");
  }
  return *this;
}

QuantBuilder& QuantBuilder::prop(const char* name, bool value) {
  if (!valid) {
    return *this;
  }

  duk_push_string(ctx, name);
  duk_push_boolean(ctx, value);
  if (duk_put_prop(ctx, -3) != 1) {
    ErrH.Abort("vss: unable to set property");
  }
  return *this;
}

QuantResult QuantBuilder::send() {
  if (valid) {
    duk_call(ctx, 2);
  }
  return QuantResult(context);
}