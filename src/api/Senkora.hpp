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
#include <memory>

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
            std::unique_ptr<Scent> scent;

        public:
            void Set(v8::Local<v8::Context> ctx, const char *key, v8::Local<v8::Value> val);
            v8::Local<v8::Value> Get(const std::string_view& key);
            std::map<std::string_view, Metadata> getMeta() const;
            void setScent(std::unique_ptr<Scent> scnt);
            std::unique_ptr<Scent> getScent();
    };

    typedef struct {
        mutable int lastScriptId = 0;
        mutable int restId = 1;
        mutable std::map<int, std::unique_ptr<MetadataObject>> moduleMetadatas;
        mutable std::map<std::string_view, v8::Local<v8::Module>> moduleCache;
        mutable std::unique_ptr<events::EventLoop> globalLoop = events::Init();
        mutable std::map<std::string_view, std::vector<v8::Local<v8::String>>> moduleExports;
    } SharedGlobals;

    std::string readFile(const std::string& name);
    void writeFile(const std::string& name, const std::string& content);
    std::string userin(const std::string& prompt);
    v8::MaybeLocal<v8::Module> compileScript(v8::Local<v8::Context> ctx, const std::string& code);

    v8::Local<v8::Value> throwException(v8::Local<v8::Context> ctx, const char* message, ExceptionType type = ExceptionType::ERROR);

    void printException(v8::Local<v8::Context> ctx, v8::Local<v8::Value> exception);

    void throwAndPrintException(v8::Local<v8::Context> ctx, const char* message, ExceptionType type = ExceptionType::ERROR);
}
#endif
