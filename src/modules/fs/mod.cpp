extern "C" {
    #include "api.h"
}
#include "mod.hpp"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"

#include <v8.h>
#include <Senkora.hpp>

namespace fsMod {
    void writeToFileJS(const v8::FunctionCallbackInfo<v8::Value>& args) {
        // arg 1: file path
        // arg 2: string

        v8::Isolate *isolate = args.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (args.Length() < 2) {
            Senkora::throwException(ctx, "Expected 2 arguments");
            return;
        }

        if (!args[0]->IsString()) {
            Senkora::throwException(ctx, "Expected argument 1 to be a string");
            return;
        }

        if (!args[1]->IsString()) {
            Senkora::throwException(ctx, "Expected argument 2 to be a string");
            return;
        }

        v8::Local<v8::String> path = args[0]->ToString(ctx).ToLocalChecked();
        v8::Local<v8::String> content = args[1]->ToString(ctx).ToLocalChecked();
        // to char * both
        v8::String::Utf8Value pathUtf8(isolate, path);
        v8::String::Utf8Value contentUtf8(isolate, content);

        writeToFile(*pathUtf8, (unsigned char*) *contentUtf8);

        args.GetReturnValue().Set(v8::Undefined(isolate));
    }

    std::vector<v8::Local<v8::String>> getExports(v8::Isolate *isolate) {
        std::vector<v8::Local<v8::String>> exports;

        exports.push_back(v8::String::NewFromUtf8(isolate, "writeToFile").ToLocalChecked());
        return exports;
    }

    v8::MaybeLocal<v8::Value> init(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod) {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, "writeToFile").ToLocalChecked();
        v8::Local<v8::Value> val = v8::FunctionTemplate::New(isolate, writeToFileJS)->GetFunction(ctx).ToLocalChecked();
        v8::Maybe<bool> fine = mod->SetSyntheticModuleExport(isolate, name, val);

        return v8::Boolean::New(ctx->GetIsolate(), true);
    }
}
