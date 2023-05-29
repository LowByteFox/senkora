#ifndef MODULES_API
#define MODULES_API

#include <string>
#include <v8.h>

namespace Senkora::Modules {
    void metadataHook(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod, v8::Local<v8::Object> meta);

    v8::MaybeLocal<v8::Module> moduleResolver(v8::Local<v8::Context> ctx, v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> ref);

    v8::MaybeLocal<v8::Module> createModule(
        v8::Local<v8::Context> ctx,
        std::string module_name,
        std::vector<v8::Local<v8::String>> export_names,
        v8::Module::SyntheticModuleEvaluationSteps step
    );

    void initBuiltinModules(v8::Isolate *isolate);
}

#endif
