/*
Senkora - JS runtime for the modern age
Copyright (C) 2023  SenkoraJS

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
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
