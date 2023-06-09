#ifndef GLOBALTHIS
#define GLOBALTHIS

#include "v8-template.h"
#include <v8-local-handle.h>

namespace globalObject {
    v8::Local<v8::ObjectTemplate> Init(v8::Isolate *isolate);

    void AddValue(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global, const char *name, v8::Local<v8::Value> value);
    void AddFunction(v8::Isolate *isolate, v8::Local<v8::ObjectTemplate> global, const char *name, v8::Local<v8::FunctionTemplate> callback);
}
#endif
