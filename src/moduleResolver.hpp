#ifndef MODULE_RESOLVER
#define MODULE_RESOLVER

#include <v8.h>

namespace moduleResolution {
    void metadataHook(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod, v8::Local<v8::Object> meta);

    v8::MaybeLocal<v8::Module> moduleResolver(v8::Local<v8::Context> ctx, v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> ref);
}

#endif
