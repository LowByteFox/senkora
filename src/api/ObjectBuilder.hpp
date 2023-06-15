#ifndef SENKORA_OBJ_API
#define SENKORA_OBJ_API

#include "Senkora.hpp"

namespace Senkora::Object {
    class ObjectBuilder {
        private:
            v8::Local<v8::ObjectTemplate> obj;
    };
}

#endif