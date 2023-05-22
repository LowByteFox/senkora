#ifndef SENKORA_API
#define SENKORA_API

#include <js/Modules.h>
#include <js/TypeDecls.h>
#include <jsapi.h>

namespace Senkora {
    JSScript *CompileScript(JSContext *ctx, const char *fileName, const char *code);
    JSObject *CompileModule(JSContext *ctx, const char *fileName, const char *code);
    std::string readFile(std::string name);
    std::string jsToString(JSContext *ctx, JS::HandleString str);
    std::string jsToString2(JSContext *ctx, JSString *str);
}

#endif
