#include "modules.hpp"
#include "empty.hpp"
#include "fs/mod.hpp"
#include "toml/mod.hpp"
#include "../../config.h"
#include "v8-message.h"
#include "v8-primitive.h"
#include "v8-script.h"
#include "v8-value.h"

#include <map>
#include <v8.h>
#include <Senkora.hpp>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

extern const Senkora::SharedGlobals globals;

namespace Senkora::Modules {
    void metadataHook(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod, v8::Local<v8::Object> meta) {
        Senkora::MetadataObject *obj = globals.moduleMetadatas[mod->ScriptId()];
        auto modMeta = obj->getMeta();
        for (auto it = modMeta.begin(); it != modMeta.end(); it++)
        {
            v8::Maybe<bool> out = meta->CreateDataProperty(ctx, it->second.key, it->second.value);
            if (!out.ToChecked())
                return;
        }
    }

    v8::MaybeLocal<v8::Module> moduleResolver(v8::Local<v8::Context> ctx, v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> ref) {
        v8::String::Utf8Value val(ctx->GetIsolate(), specifier);
        std::string name(*val);

        Senkora::MetadataObject *obj = globals.moduleMetadatas[ref->ScriptId()];
        v8::String::Utf8Value val2(ctx->GetIsolate(), obj->Get("url"));
        std::string urlPath(*val2);

        std::string base = name;

        if (!base.compare(0, 8, "senkora:"))
        {
            if (globals.moduleCache.find(base) == globals.moduleCache.end()) {
                std::string msg = "Module \"";
                msg += base.c_str();
                msg += "\" was not found!";
                Senkora::throwAndPrintException(ctx, msg.c_str());
                exit(1);
            }
            return globals.moduleCache[base];
        }

        if (base.c_str()[0] != '/')
        {
            base = fs::path(urlPath).parent_path();
            base += "/" + name;
            base = fs::path(base).lexically_normal();
        }

        if (globals.moduleCache.find(base) != globals.moduleCache.end())
        {
            return globals.moduleCache[base];
        }

        std::string code = Senkora::readFile(base);

        Senkora::MetadataObject *meta = new Senkora::MetadataObject();
        v8::Local<v8::Value> url = v8::String::NewFromUtf8(ctx->GetIsolate(), base.c_str()).ToLocalChecked();

        meta->Set(ctx, "url", url);

        v8::Local<v8::Module> mod = Senkora::compileScript(ctx, code).ToLocalChecked();

        Senkora::Scent *scent = new Senkora::Scent();
        scent->num = mod->ScriptId();
        meta->setScent(scent);

        globals.moduleCache[base] = mod;

        globals.moduleMetadatas[mod->ScriptId()] = meta;

        return mod;
    }

    v8::MaybeLocal<v8::Module> createModule(
        v8::Local<v8::Context> ctx,
        std::string module_name,
        std::vector<v8::Local<v8::String>> export_names,
        v8::Module::SyntheticModuleEvaluationSteps step
    ) {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, module_name.c_str()).ToLocalChecked();
        v8::Local<v8::Module> mod = v8::Module::CreateSyntheticModule(isolate, name, export_names, step);

        return mod;
    }

    // INIT builtin modules here
    void initBuiltinModules(v8::Isolate *isolate) {
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        globals.moduleCache["senkora:__empty"] = createModule(ctx, 
            "senkora:__empty",
            dummy::getExports(isolate), dummy::init).ToLocalChecked();

        #ifdef ENABLE_FS
        globals.moduleCache["senkora:fs"] = createModule(ctx, 
            "senkora:fs",
            fsMod::getExports(isolate), fsMod::init).ToLocalChecked();
        #endif

        #ifdef ENABLE_TOML
        globals.moduleCache["senkora:toml"] = createModule(ctx,
            "senkora:toml",
            tomlMod::getExports(isolate), tomlMod::init).ToLocalChecked();
        #endif
    }
}
