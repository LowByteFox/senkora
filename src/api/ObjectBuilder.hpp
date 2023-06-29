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
#ifndef SENKORA_OBJ_API
#define SENKORA_OBJ_API

#include "Senkora.hpp"
#include <v8-template.h>

namespace Senkora::Object {
    class ObjectBuilder {
        public:
            explicit ObjectBuilder(v8::Isolate *isolate);
            v8::Local<v8::Object> Assemble(v8::Local<v8::Context> ctx);
            v8::Local<v8::ObjectTemplate> GetTemplate() { return this->obj; }
            void Dissasemble(v8::Local<v8::Object> object);
            void Set(const char* name, v8::Local<v8::Value> value);
            void Set(const char* name, v8::Local<v8::FunctionTemplate> value);
            void Set(const char* name, v8::Local<v8::Object> value);
            void Set(const char* name, v8::Local<v8::ObjectTemplate> value);
            v8::Local<v8::Value> Get(const char* name);
            v8::Local<v8::Array> GetKeys(v8::Local<v8::Context> ctx);

        private:
            bool isDissasembled = false;
            v8::Local<v8::ObjectTemplate> obj;
            v8::Local<v8::Object> dissasembly;
            v8::Isolate *isolate;
            v8::Local<v8::Array> keys;
    };
}

#endif
