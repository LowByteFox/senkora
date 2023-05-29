#include "module.hpp"
#include "v8-context.h"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-promise.h"
#include "v8-script.h"
#include "v8-value.h"
#include <functional>
#include <string>
#include <vector>

namespace Senkora {
    void Module::addExport(std::string name, v8::Local<v8::Value> val) {
        auto item = std::find(this->exportNames.begin(), this->exportNames.end(), name);

        if (item == this->exportNames.end()) {
            throw ModuleError::InvalidKey;
        }

        this->values[name] = val;
    }

    static v8::MaybeLocal<v8::Value> moduleInitialization(v8::Local<v8::Context> ctx, v8::Local<v8::Module> module) {
        return v8::MaybeLocal<v8::Value>();
    }

    v8::Local<v8::Module> Module::buildModule(v8::Isolate *isolate) {
        v8::Local<v8::String> modName = v8::String::NewFromUtf8(isolate, this->moduleName.c_str()).ToLocalChecked();

        std::vector<v8::Local<v8::String>> exportNames;

        for (std::string name : this->exportNames) {
            exportNames.push_back(v8::String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked());
        }

        return v8::Module::CreateSyntheticModule(isolate, modName, exportNames, this->moduleInitialization);
    }
}
