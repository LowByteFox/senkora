#include "Senkora.hpp"
#include "v8-context.h"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-script.h"
#include "v8-message.h"
#include <cstdio>
#include <string>

static int lastScriptId;

namespace Senkora {
    void MetadataObject::Set(v8::Local<v8::Context> ctx, std::string key, v8::Local<v8::Value> val) {
        v8::Isolate *isolate = ctx->GetIsolate();

        this->meta[key] = (Metadata) {
            .key = v8::String::NewFromUtf8(isolate, key.c_str()).ToLocalChecked(),
            .value = val
        };
    }

    v8::Local<v8::Value> MetadataObject::Get(std::string key) {
        Metadata meta = this->meta[key];
        return meta.value;
    }

    std::map<std::string, Metadata>::iterator MetadataObject::begin() {
        return this->meta.begin();
    }

    std::map<std::string, Metadata>::iterator MetadataObject::end() {
        return this->meta.end();
    }

    v8::MaybeLocal<v8::Module> compileScript(v8::Local<v8::Context> ctx, std::string code) {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::ScriptOrigin origin(isolate, v8::Undefined(isolate), 0, 0, false, lastScriptId, v8::Local<v8::Value>(), false, false, true);
        lastScriptId++;

        v8::ScriptCompiler::Source source(v8::String::NewFromUtf8(isolate, code.c_str()).ToLocalChecked(), origin);

        return v8::ScriptCompiler::CompileModule(isolate, &source);
    }
} 
