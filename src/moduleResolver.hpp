#ifndef MODULE_RESOLVER
#define MODULE_RESOLVER

#include <js/TypeDecls.h>
#include <map>
#include <string>
#include <jsapi.h>

bool registerBuiltinModule(JSContext *ctx, std::string name, JSObject *module);
JSObject *resolveHook(JSContext *ctx, JS::HandleValue modulePrivate, JS::HandleObject moduleRequest);

#endif
