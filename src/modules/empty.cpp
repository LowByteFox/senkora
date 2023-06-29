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
#include "empty.hpp"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"

#include <Senkora.hpp>
#include "modules.hpp"
#include <v8.h>

namespace dummy {
    std::vector<v8::Local<v8::String>> getExports(v8::Isolate *isolate) {
        std::vector<v8::Local<v8::String>> exports;

        exports.push_back(v8::String::NewFromUtf8(isolate, "hello").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "default").ToLocalChecked());
        return exports;
    }

    v8::MaybeLocal<v8::Value> init(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod) {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::Local<v8::Object> default_exports = v8::Object::New(isolate);

        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, "hello").ToLocalChecked();
        v8::Local<v8::Value> val = v8::String::NewFromUtf8(isolate, "I am dummy").ToLocalChecked();
        default_exports->Set(ctx, name, val).Check();
        Senkora::Modules::setModuleExport(mod, ctx, isolate, name, val);

        Senkora::Modules::setModuleExport(mod, ctx, isolate, v8::String::NewFromUtf8(isolate, "default").ToLocalChecked(), default_exports);

        return v8::Boolean::New(ctx->GetIsolate(), true);
    }
}
