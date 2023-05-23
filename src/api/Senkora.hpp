#ifndef SENKORA_API
#define SENKORA_API

#include <js/Modules.h>
#include <js/RootingAPI.h>
#include <js/TypeDecls.h>
#include <jsapi.h>

namespace Senkora {
    JSScript *CompileScript(JSContext *ctx, const char *fileName, const char *code);
    JSObject *CompileModule(JSContext *ctx, const char *fileName, const char *code);
    std::string readFile(std::string name);
    std::string jsToString(JSContext *ctx, JS::HandleString str);
    std::string jsToString(JSContext *ctx, JSString *str);
    JSString *stringToJS(JSContext *ctx, const char *str);

    // C++ bruh
    template<typename T>
    JS::Handle<T> toHandle(T *base) {
        return JS::Handle<T>::fromMarkedLocation(base);
    }

    template<typename T>
    JS::MutableHandle<T> toMutableHandle(T *base) {
        return JS::MutableHandle<T>::fromMarkedLocation(base);
    }


    template<typename T>
    JS::Rooted<T> toRooted(JSContext *ctx, T base) {
        return JS::Rooted<T>(ctx, base);
    }
}

#endif
