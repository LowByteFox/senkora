#ifndef MODULE_API
#define MODULE_API

#include "v8-local-handle.h"
#include "v8-script.h"
#include "v8-value.h"
#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace Senkora {
    typedef enum {
        InvalidKey
    } ModuleError;

    class Module {
        private:
            std::string moduleName;
            std::vector<std::string> exportNames;
            std::map<std::string, v8::Local<v8::Value>> values;

        public:
            Module(std::string moduleName, std::vector<std::string> exportNames) {
                this->moduleName = moduleName;
                this->exportNames = exportNames;
            }

            static v8::MaybeLocal<v8::Value> moduleInitialization(v8::Local<v8::Context> ctx, v8::Local<v8::Module> module);

            void addExport(std::string name, v8::Local<v8::Value> val); 

            v8::Local<v8::Module> buildModule(v8::Isolate *isolate);
    };
}

#endif
