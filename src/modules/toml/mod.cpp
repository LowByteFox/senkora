#include "mod.hpp"

#include <v8.h>
#include <Senkora.hpp>
#include <toml.hpp>

namespace tomlMod {
    void parseTOML(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 1) {
            Senkora::throwException(ctx, "Expected 1 argument");
            return;
        }

        if (!args[0]->IsString()) {
            Senkora::throwException(ctx, "Expected argument 1 to be a string");
            return;
        }

        v8::Local<v8::String> path = args[0]->ToString(ctx).ToLocalChecked();
        v8::String::Utf8Value pathUtf8(isolate, path);

        auto node = Senkora::TOML::parse(*pathUtf8);

        args.GetReturnValue().Set(v8::Undefined(isolate));
    }

    std::vector<v8::Local<v8::String>> getExports(v8::Isolate *isolate) {
        std::vector<v8::Local<v8::String>> exports;

        exports.push_back(v8::String::NewFromUtf8(isolate, "parse").ToLocalChecked());
        return exports;
    }

    v8::MaybeLocal<v8::Value> init(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod) {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, "parse").ToLocalChecked();
        v8::Local<v8::Value> val = v8::FunctionTemplate::New(isolate, parseTOML)->GetFunction(ctx).ToLocalChecked();
        if (v8::Maybe<bool> fine = mod->SetSyntheticModuleExport(isolate, name, val); !fine.IsNothing() && !fine.FromJust()) {
            Senkora::throwException(ctx, "Failed to set synthetic module export");
            return v8::MaybeLocal<v8::Value>();
        }

        return v8::Boolean::New(isolate, true);
    }
}