#ifndef SENKORA_API
#define SENKORA_API

#include <js/Modules.h>
#include <js/TypeDecls.h>
#include <jsapi.h>

namespace Senkora {
    JSScript *CompileScript(JSContext *ctx, const char *fileName, const char *code);
    JSObject *CompileModule(JSContext *ctx, const char *fileName, const char *code);
}

#endif
