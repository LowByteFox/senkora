#ifndef TOML_MODULE
#define TOML_MODULE

#include <toml.hpp>
#include <vector>
#include <v8.h>

namespace tomlMod {
    // do not use outside module
    v8::Local<v8::Value> toJsVal(const Senkora::TOML::TomlNode* & node);
    v8::Local<v8::Value> toJsArray(v8::Isolate *isolate, const Senkora::TOML::TomlNode* & node);
    v8::Local<v8::Object> toJsObject(v8::Isolate *isolate, const Senkora::TOML::TomlNode* & node);
    v8::Local<v8::Object> toJsObject(v8::Isolate *isolate, Senkora::TOML::TomlNode* const& node);

    std::vector<v8::Local<v8::String>> getExports(v8::Isolate *isolate);
    v8::MaybeLocal<v8::Value> init(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod);
}

#endif