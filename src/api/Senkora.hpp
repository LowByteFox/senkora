#ifndef SENKORA_API
#define SENKORA_API

#include "v8-local-handle.h"
#include "v8-primitive.h"
#include <map>
#include <v8-context.h>
#include <v8-script.h>
#include <v8-value.h>

#include <string>

namespace Senkora
{
    typedef struct
    {
        v8::Local<v8::Name> key;
        v8::Local<v8::Value> value;
    } Metadata;

    class MetadataObject
    {
    private:
        std::map<std::string, Metadata> meta;

    public:
        void Set(v8::Local<v8::Context> ctx, std::string key, v8::Local<v8::Value> val);
        v8::Local<v8::Value> Get(std::string key);
        std::map<std::string, Metadata> getMeta();
    };

    std::string readFile(std::string name);
    v8::MaybeLocal<v8::Module> compileScript(v8::Local<v8::Context> ctx, std::string code);

    v8::MaybeLocal<v8::Module> createModule(
        v8::Local<v8::Context> ctx,
        std::string module_name,
        std::vector<v8::Local<v8::String>> export_names,
        v8::Module::SyntheticModuleEvaluationSteps step
    );

}
#endif
