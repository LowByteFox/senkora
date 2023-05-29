#include "v8-context.h"
#include "v8-local-handle.h"
#include "v8-object.h"
#include "v8-script.h"
#include <Senkora.hpp>
#include <cstdio>
#include <vector>

#include "moduleResolver.hpp"

extern std::map<int, Senkora::MetadataObject*> moduleMetadatas;

namespace moduleResolution {
    void metadataHook(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod, v8::Local<v8::Object> meta) {
        Senkora::MetadataObject *obj = moduleMetadatas[mod->ScriptId()];
        auto modMeta = obj->getMeta();
        for (auto it = modMeta.begin(); it != modMeta.end(); it++) {
            v8::Maybe<bool> out = meta->CreateDataProperty(ctx, it->second.key, it->second.value);
            if (!out.ToChecked()) return;
        }
    }
}
