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
    
    // Helper functions
    void compareObjects(v8::Local<v8::Context> &ctx, v8::Local<v8::Object> &expectedObject, v8::Local<v8::Object> &actualObject, bool &result);
    void compareArrays(v8::Local<v8::Context> &ctx, v8::Local<v8::Array> &expectedArray, v8::Local<v8::Array> &actualArray, bool &result);

    std::string stringifyForOutput(v8::Local<v8::Context> ctx, v8::Local<v8::Value> value);
}