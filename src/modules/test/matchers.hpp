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
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"

#include <Senkora.hpp>
#include "../modules.hpp"
#include <v8.h>

namespace testMatcher
{
    bool toEqual(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toEqualCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

    bool toStrictEqual(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toStrictEqualCallback(const v8::FunctionCallbackInfo<v8::Value> &args);
    
    bool toBeEmpty(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toBeEmptyCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

    // Boolean - Related
    bool toBeTrue(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toBeTrueCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

    bool toBeFalse(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toBeFalseCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

    bool toBeBoolean(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toBeBooleanCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

    // Array - Related
    bool toBeArray(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toBeArrayCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

    bool toBeArrayOfSize(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toBeArrayOfSizeCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

    // Object - Related
    bool toBeObject(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toBeObjectCallback(const v8::FunctionCallbackInfo<v8::Value> &args);
    
    bool toBeOneOf(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toBeOneOfCallback(const v8::FunctionCallbackInfo<v8::Value> &args);
    // Helper functions
    void compareObjects(v8::Local<v8::Context> &ctx, v8::Local<v8::Object> &expectedObject, v8::Local<v8::Object> &actualObject, bool &result, bool strict);
    void compareArrays(v8::Local<v8::Context> &ctx, v8::Local<v8::Array> &expectedArray, v8::Local<v8::Array> &actualArray, bool &result, bool strict);

    std::string stringifyForOutput(v8::Local<v8::Context> ctx, v8::Local<v8::Value> value);
}
