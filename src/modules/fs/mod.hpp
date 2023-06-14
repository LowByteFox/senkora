#ifndef FS_MODULE
#define FS_MODULE

#include <v8-isolate.h>
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include <v8.h>
#include <vector>

namespace fsMod {
    std::vector<v8::Local<v8::String>> getExports(v8::Isolate *isolate);
    v8::MaybeLocal<v8::Value> init(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod);
}

#endif
