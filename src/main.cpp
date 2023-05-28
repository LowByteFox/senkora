#include "api/Senkora.hpp"
#include "v8-data.h"
#include "v8-local-handle.h"
#include "v8-maybe.h"
#include "v8-promise.h"
#include "v8-script.h"
#include <v8.h>
#include <v8-isolate.h>
#include <libplatform/libplatform.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        if (first) {
            first = false;
        } else {
            printf(" ");
        }
        v8::String::Utf8Value str(args.GetIsolate(), args[i]);
        const char* cstr = ToCString(str);
        printf("%s", cstr);
    }
    printf("\n");
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    // Initialize V8.
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();

    v8::Isolate* isolate = v8::Isolate::New(create_params);
    {
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, Print));

        v8::Local<v8::Context> ctx = v8::Context::New(isolate, nullptr, global);
        v8::Context::Scope context_scope(ctx);

        v8::MaybeLocal<v8::Module> mod = Senkora::compileScript(ctx, "print('Ahoj');");
        v8::Local<v8::Module> mod2;
        if (!mod.ToLocal(&mod2)) {
            goto end;
        }
        v8::Maybe<bool> out = mod2->InstantiateModule(ctx, [](v8::Local<v8::Context> context, v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer) {
                return v8::MaybeLocal<v8::Module>();
        });
        v8::MaybeLocal<v8::Value> res = mod2->Evaluate(ctx);
    }

end:
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
    return 0;
}
