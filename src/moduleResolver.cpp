#include "v8-context.h"
#include "v8-local-handle.h"
#include "v8-maybe.h"
#include "v8-object.h"
#include "v8-primitive.h"
#include "v8-script.h"
#include <Senkora.hpp>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "moduleResolver.hpp"
#include "v8-value.h"

namespace fs = std::filesystem;

extern std::map<int, Senkora::MetadataObject*> moduleMetadatas;
extern std::map<int, v8::Persistent<v8::Module>> moduleCache;

namespace moduleResolution {
    void metadataHook(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod, v8::Local<v8::Object> meta) {
        Senkora::MetadataObject *obj = moduleMetadatas[mod->ScriptId()];
        auto modMeta = obj->getMeta();
        for (auto it = modMeta.begin(); it != modMeta.end(); it++) {
            v8::Maybe<bool> out = meta->CreateDataProperty(ctx, it->second.key, it->second.value);
            if (!out.ToChecked()) return;
        }
    }

    v8::MaybeLocal<v8::Module> moduleResolver(v8::Local<v8::Context> ctx, v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> ref) {
        v8::String::Utf8Value val(ctx->GetIsolate(), specifier);
        std::string name(*val);

        Senkora::MetadataObject *obj = moduleMetadatas[ref->ScriptId()];
        v8::String::Utf8Value val2(ctx->GetIsolate(), obj->Get("url"));
        std::string urlPath(*val2);

        std::string base = name;

        if (base.c_str()[0] != '/') {
            base = fs::path(urlPath).parent_path();
            base += "/" + name;
            base = fs::path(base).lexically_normal();
        }

        std::string code = Senkora::readFile(base);

        Senkora::MetadataObject *meta = new Senkora::MetadataObject();
        v8::Local<v8::Value> url = v8::String::NewFromUtf8(ctx->GetIsolate(), base.c_str()).ToLocalChecked();

        meta->Set(ctx, "url", url);

        v8::Local<v8::Module> mod = Senkora::compileScript(ctx, code).ToLocalChecked();

        moduleMetadatas[mod->ScriptId()] = meta;

        return mod;
    }
}
