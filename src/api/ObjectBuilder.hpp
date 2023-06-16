#ifndef SENKORA_OBJ_API
#define SENKORA_OBJ_API

#include "Senkora.hpp"
#include <v8-template.h>

namespace Senkora::Object {
    class ObjectBuilder {
        public:
            ObjectBuilder(v8::Isolate *isolate);
            v8::Local<v8::Object> Assemble(v8::Local<v8::Context> ctx);
            v8::Local<v8::ObjectTemplate> GetTemplate() { return this->obj; }
            void Dissasemble(v8::Local<v8::Object> obj);
            void Set(const char* name, v8::Local<v8::Value> value);
            void Set(const char* name, v8::Local<v8::FunctionTemplate> value);
            void Set(const char* name, v8::Local<v8::Object> value);
            v8::Local<v8::Value> Get(const char* name);
            v8::Local<v8::Array> GetKeys(v8::Local<v8::Context> ctx);

        private:
            bool isDissasembled = false;
            v8::Local<v8::ObjectTemplate> obj;
            v8::Local<v8::Object> dissasembly;
            v8::Isolate *isolate;
            v8::Local<v8::Array> keys;
    };
}

#endif