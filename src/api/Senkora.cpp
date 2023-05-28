#include "Senkora.hpp"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-script.h"
#include "v8-message.h"

namespace Senkora {
    v8::MaybeLocal<v8::Module> compileScript(v8::Local<v8::Context> ctx, std::string code) {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::ScriptOrigin origin(isolate, v8::Undefined(isolate), 0, 0, false, -1, v8::Local<v8::Value>(), false, false, true);

        v8::ScriptCompiler::Source source(v8::String::NewFromUtf8(isolate, code.c_str()).ToLocalChecked(), origin);

        return v8::ScriptCompiler::CompileModule(isolate, &source);
    }
} 
