#ifndef MODULES_API
#define MODULES_API

#include "v8-data.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-script.h"
#include <string>
#include <v8.h>
#include <vector>

namespace Senkora::Modules {
    void metadataHook(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod, v8::Local<v8::Object> meta);

    v8::MaybeLocal<v8::Module> moduleResolver(
        v8::Local<v8::Context> ctx,
        v8::Local<v8::String> specifier,
        [[maybe_unused]] v8::Local<v8::FixedArray> import_assertions,
        v8::Local<v8::Module> ref
    );

    v8::MaybeLocal<v8::Module> createModule(
        v8::Local<v8::Context> ctx,
        const std::string& module_name,
        const std::vector<v8::Local<v8::String>>& export_names,
        const v8::Module::SyntheticModuleEvaluationSteps& step
    );

    void setModuleExport(
        v8::Local<v8::Module> mod,
        v8::Local<v8::Context> ctx,
        v8::Isolate *isolate,
        v8::Local<v8::String> export_name,
        v8::Local<v8::Value> export_value
    );

    void setModuleExport(
        v8::Local<v8::Module> mod,
        v8::Local<v8::Context> ctx,
        v8::Local<v8::Object> default_exports,
        v8::Isolate *isolate,
        v8::Local<v8::String> export_name,
        v8::Local<v8::Value> export_value
    );

    void initBuiltinModules(v8::Isolate *isolate);
}

#endif
