#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"

#include <Senkora.hpp>
#include "../modules.hpp"
#include <v8.h>

namespace testMod {
    void describe(const v8::FunctionCallbackInfo<v8::Value>& args) {
              v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 2) {
            Senkora::throwException(ctx, "Expected 2 arguments");
            return;
        }

        if (!args[0]->IsString()) {
            Senkora::throwException(ctx, "Expected argument 2 to be a string");
            return;
        }

        if (!args[1]->IsFunction()) {
            Senkora::throwException(ctx, "Expected argument 1 to be a function");
            return;
        }

        v8::Local<v8::Function> fn = args[1].As<v8::Function>();
        fn->Call(ctx, ctx->Global(), 0, nullptr);
    }

    std::vector<v8::Local<v8::String>> getExports(v8::Isolate *isolate) {
        std::vector<v8::Local<v8::String>> exports;

        exports.push_back(v8::String::NewFromUtf8(isolate, "describe").ToLocalChecked());
//        exports.push_back(v8::String::NewFromUtf8(isolate, "it").ToLocalChecked());
//        exports.push_back(v8::String::NewFromUtf8(isolate, "expect").ToLocalChecked());

        return exports;
    }

    v8::MaybeLocal<v8::Value> init(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod) {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::Local<v8::Object> default_exports = v8::Object::New(isolate);

        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, "describe").ToLocalChecked();
        v8::Local<v8::Value> val = v8::FunctionTemplate::New(isolate, describe)->GetFunction(ctx).ToLocalChecked();
        default_exports->Set(ctx, name, val).Check();
        Senkora::Modules::setModuleExport(mod, ctx, isolate, name, val);

        Senkora::Modules::setModuleExport(mod, ctx, isolate, v8::String::NewFromUtf8(isolate, "default").ToLocalChecked(), default_exports);

        return v8::Boolean::New(ctx->GetIsolate(), true);
    }
}
