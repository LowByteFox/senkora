#ifndef SENKORA_API
#define SENKORA_API

#include "Scent.hpp"
#include "../eventLoop.hpp"
#include "v8-exception.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include <map>
#include <v8-context.h>
#include <v8-script.h>
#include <v8-value.h>
#include <string>

namespace Senkora {

    typedef struct {
        v8::Local<v8::Name> key;
        v8::Local<v8::Value> value;
    } Metadata;

    enum class ExceptionType {
        RANGE = 1,
        REFERENCE,
        SYNTAX,
        TYPE,
        ERROR
    };

    class MetadataObject {
        private:
            std::map<std::string_view, Metadata> meta;
            Scent *scent;

        public:
            void Set(v8::Local<v8::Context> ctx, const std::string& key, v8::Local<v8::Value> val);
            v8::Local<v8::Value> Get(const std::string& key);
            std::map<std::string_view, Metadata> getMeta() const;
            void setScent(Scent *scent);
            Scent *getScent();
    };

    typedef struct {
        mutable int lastScriptId = 0;
        mutable int restId = 1;
        mutable std::map<int, MetadataObject*> moduleMetadatas;
        mutable std::map<std::string_view, v8::Local<v8::Module>> moduleCache;
        mutable events::EventLoop *globalLoop = events::Init();
    } SharedGlobals;

    std::string readFile(const std::string& name);
    v8::MaybeLocal<v8::Module> compileScript(v8::Local<v8::Context> ctx, const std::string& code);

    v8::Local<v8::Value> throwException(v8::Local<v8::Context> ctx, const char* message, ExceptionType type = ExceptionType::ERROR);

    void printException(v8::Local<v8::Context> ctx, v8::Local<v8::Value> exception);

    void throwAndPrintException(v8::Local<v8::Context> ctx, const char* message, ExceptionType type = ExceptionType::ERROR);
}
#endif
