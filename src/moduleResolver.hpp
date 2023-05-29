#ifndef MODULE_RESOLVER
#define MODULE_RESOLVER

#include <v8.h>

namespace moduleResolution {
    void metadataHook(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod, v8::Local<v8::Object> meta);
}

#endif
