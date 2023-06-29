/*
Senkora - JS runtime for the modern age
Copyright (C) 2023  SenkoraJS

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "modules.hpp"
#include "empty.hpp"
#include "fs/mod.hpp"
#include "toml/mod.hpp"
#include "test/mod.hpp"
#include "../../config.h"
#include "v8-local-handle.h"
#include "v8-message.h"
#include "v8-primitive.h"
#include "v8-script.h"
#include "v8-value.h"

#include <map>
#include <v8.h>
#include <bits/stdc++.h>
#include <Senkora.hpp>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

extern const Senkora::SharedGlobals globals;

namespace Senkora::Modules {
    void metadataHook(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod, v8::Local<v8::Object> meta) {
        const auto& obj = globals.moduleMetadatas[mod->ScriptId()];
        auto modMeta = obj->getMeta();
        for (const auto& [key, value]: modMeta) {
            v8::Maybe<bool> out = meta->CreateDataProperty(ctx, value.key, value.value);
            if (!out.ToChecked())
                return;
        }
    }

    v8::MaybeLocal<v8::Module> moduleResolver(
        v8::Local<v8::Context> ctx,
        v8::Local<v8::String> specifier,
        [[maybe_unused]] v8::Local<v8::FixedArray> import_assertions,
        v8::Local<v8::Module> ref
    ) {
        v8::String::Utf8Value val(ctx->GetIsolate(), specifier);
        std::string name(*val);

        const auto& obj = globals.moduleMetadatas[ref->ScriptId()];
        v8::String::Utf8Value val2(ctx->GetIsolate(), obj->Get("url"));
        std::string urlPath(*val2);

        std::string base = name;

        if (!base.compare(0, 8, "senkora:"))
        {
            if (!globals.moduleCache.contains(base)) {
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

        if (globals.moduleCache.contains(base))
        {
            return globals.moduleCache[base];
        }

        std::string code = Senkora::readFile(base);
        if (!code.length()) {
            std::string msg = "File \"";
            msg += base.c_str();
            msg += "\" was not found!";
            Senkora::throwAndPrintException(ctx, msg.c_str());
            exit(1);
        }
        auto meta = std::make_unique<Senkora::MetadataObject>();
        v8::Local<v8::Value> url = v8::String::NewFromUtf8(ctx->GetIsolate(), base.c_str()).ToLocalChecked();

        meta->Set(ctx, "url", url);

        v8::Local<v8::Module> mod = Senkora::compileScript(ctx, code).ToLocalChecked();

        auto scent = std::make_unique<Senkora::Scent>();
        scent->num = mod->ScriptId();
        meta->setScent(std::move(scent));

        globals.moduleCache[base] = mod;

        globals.moduleMetadatas[mod->ScriptId()] = std::move(meta);

        return mod;
    }

    v8::MaybeLocal<v8::Module> createModule(
        v8::Local<v8::Context> ctx,
        const std::string& module_name,
        const std::vector<v8::Local<v8::String>>& export_names,
        const v8::Module::SyntheticModuleEvaluationSteps& step
    ) {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, module_name.c_str()).ToLocalChecked();
        v8::Local<v8::Module> mod = v8::Module::CreateSyntheticModule(isolate, name, export_names, step);

        return mod;
    }

    void setModuleExport(
        v8::Local<v8::Module> mod,
        v8::Local<v8::Context> ctx,
        v8::Isolate *isolate,
        v8::Local<v8::String> export_name,
        v8::Local<v8::Value> export_value
    ) {
        if (v8::Maybe<bool> fine = mod->SetSyntheticModuleExport(isolate, export_name, export_value); !fine.IsNothing() && !fine.FromJust()) {
            Senkora::throwException(ctx, "Failed to set synthetic module export");
        }
    }

    void setModuleExport(
        v8::Local<v8::Module> mod,
        v8::Local<v8::Context> ctx,
        v8::Local<v8::Object> default_exports,
        v8::Isolate *isolate,
        v8::Local<v8::String> export_name,
        v8::Local<v8::Value> export_value
    ) {
        if (v8::Maybe<bool> fine = mod->SetSyntheticModuleExport(isolate, export_name, export_value); !fine.IsNothing() && !fine.FromJust()) {
            Senkora::throwException(ctx, "Failed to set synthetic module export");
        }

        if (v8::Maybe<bool> fine = default_exports->Set(ctx, export_name, export_value); !fine.IsNothing() && !fine.FromJust()) {
            Senkora::throwException(ctx, "Failed to set synthetic module export");
        }
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
        globals.moduleExports["senkora:fs"] = fsMod::getExports(isolate);
        #endif

        #ifdef ENABLE_TOML
        globals.moduleCache["senkora:toml"] = createModule(ctx,
            "senkora:toml",
            tomlMod::getExports(isolate), tomlMod::init).ToLocalChecked();
        globals.moduleExports["senkora:toml"] = tomlMod::getExports(isolate);
        #endif

        #ifdef ENABLE_TEST
        globals.moduleCache["senkora:test"] = createModule(ctx,
            "senkora:test",
            testMod::getExports(isolate), testMod::init).ToLocalChecked();
        globals.moduleExports["senkora:test"] = testMod::getExports(isolate);
        #endif
    }
}
