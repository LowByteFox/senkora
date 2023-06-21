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
}