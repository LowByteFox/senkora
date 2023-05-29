#include "dummy.hpp"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"

#include <v8.h>

namespace dummy {
    std::vector<v8::Local<v8::String>> getExports(v8::Isolate *isolate) {
        std::vector<v8::Local<v8::String>> exports;

        exports.push_back(v8::String::NewFromUtf8(isolate, "hello").ToLocalChecked());

        return exports;
    }

    v8::MaybeLocal<v8::Value> init(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod) {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, "hello").ToLocalChecked();
        v8::Local<v8::Value> val = v8::String::NewFromUtf8(isolate, "I am dummy").ToLocalChecked();
        v8::Maybe<bool> fine = mod->SetSyntheticModuleExport(isolate, name, val);

        return v8::Boolean::New(ctx->GetIsolate(), true);
    }
}
