#ifndef SENKORA_API
#define SENKORA_API

#include "v8-local-handle.h"
#include <v8-context.h>
#include <v8-script.h>
#include <v8-value.h>

#include <string>

namespace Senkora {
    v8::MaybeLocal<v8::Module> compileScript(v8::Local<v8::Context> ctx, std::string code);
}

#endif
