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
#include "globalThis.hpp"
#include "v8-template.h"

namespace globalObject {
    v8::Local<v8::ObjectTemplate> Init(v8::Isolate *isolate) {
        return v8::ObjectTemplate::New(isolate);
    }

    void AddValue(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global, const char *name, v8::Local<v8::Value> value) {
        global->Set(isolate, name, value);
    }

    void AddFunction(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global, const char *name, v8::Local<v8::FunctionTemplate> callback) {
        global->Set(isolate, name, callback);
    }
}
