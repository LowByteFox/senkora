#ifndef TOML_MODULE
#define TOML_MODULE

#include <vector>
#include <v8.h>

namespace tomlMod {
    std::vector<v8::Local<v8::String>> getExports(v8::Isolate *isolate);
    v8::MaybeLocal<v8::Value> init(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod);
}

#endif