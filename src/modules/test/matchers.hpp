#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"

#include <iostream>
#include <Senkora.hpp>
#include "../modules.hpp"
#include <v8.h>

namespace testMatcher
{
    bool toEqual(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toEqualCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

    // Boolean - Related
    bool toBeTrue(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toBeTrueCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

    bool toBeFalse(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toBeFalseCallback(const v8::FunctionCallbackInfo<v8::Value> &args);

    bool toBeBoolean(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx);
    void toBeBooleanCallback(const v8::FunctionCallbackInfo<v8::Value> &args);
}