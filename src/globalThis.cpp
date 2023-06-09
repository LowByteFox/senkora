#include "globalThis.hpp"
#include "v8-template.h"

namespace globalObject {
    v8::Local<v8::ObjectTemplate> Init(v8::Isolate *isolate) {
        return v8::ObjectTemplate::New(isolate);
    }

    void AddValue(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global, const char *name, v8::Local<v8::Value> value) {
        global->Set(isolate, name, value);
    }

    void AddFunction(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global, const char *name, v8::Local<v8::FunctionTemplate> callback) {
        global->Set(isolate, name, callback);
    }
}
