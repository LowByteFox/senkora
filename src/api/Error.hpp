#ifndef SENKORA_ERROR
#define SENKORA_ERROR

#include <v8.h>
#include "v8-container.h"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-script.h"
#include "v8-template.h"
#include "v8-value.h"

namespace Senkora::Error {
    v8::Local<v8::Array> returnError(v8::Isolate *isolate, v8::Local<v8::Value> val, std::string msg);
    v8::Local<v8::FunctionTemplate> makeError(v8::Isolate *isolate);

}

#endif
