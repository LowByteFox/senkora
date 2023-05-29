#include "Senkora.hpp"
#include "v8-context.h"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-script.h"
#include "v8-message.h"
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>
#include <fstream>

static int lastScriptId;

namespace Senkora
{
    void MetadataObject::Set(v8::Local<v8::Context> ctx, std::string key, v8::Local<v8::Value> val)
    {
        v8::Isolate *isolate = ctx->GetIsolate();

        this->meta[key] = (Metadata){
            .key = v8::String::NewFromUtf8(isolate, key.c_str()).ToLocalChecked(),
            .value = val};
    }

    v8::Local<v8::Value> MetadataObject::Get(std::string key)
    {
        Metadata meta = this->meta[key];
        return meta.value;
    }

    std::map<std::string, Metadata> MetadataObject::getMeta()
    {
        return this->meta;
    }

    std::string readFile(std::string name)
    {
        std::ifstream file(name);
        if (!file.good())
            return "";

        std::stringstream ss;

        ss << file.rdbuf();

        std::string out = ss.str();

        file.close();

        return out;
    }

    v8::MaybeLocal<v8::Module> compileScript(v8::Local<v8::Context> ctx, std::string code)
    {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::ScriptOrigin origin(isolate, v8::Undefined(isolate), 0, 0, false, lastScriptId, v8::Local<v8::Value>(), false, false, true);
        lastScriptId++;

        v8::ScriptCompiler::Source source(v8::String::NewFromUtf8(isolate, code.c_str()).ToLocalChecked(), origin);

        return v8::ScriptCompiler::CompileModule(isolate, &source);
    }

    v8::MaybeLocal<v8::Value> step(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod)
    {
        v8::Isolate *isolate = ctx->GetIsolate();

        // tu nastavuj mrdku
        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, "test").ToLocalChecked();
        v8::Local<v8::Value> val = v8::String::NewFromUtf8(isolate, "im builtin").ToLocalChecked();
        mod->SetSyntheticModuleExport(isolate, name, val);

        return v8::Boolean::New(ctx->GetIsolate(), true);
    }

    v8::MaybeLocal<v8::Module> createModule(v8::Local<v8::Context> ctx, std::string module_name, std::vector<v8::Local<v8::String>> export_names)
    {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, module_name.c_str()).ToLocalChecked();
        v8::Local<v8::Module> mod = v8::Module::CreateSyntheticModule(isolate, name, export_names, step);

        return mod;
    }
}
