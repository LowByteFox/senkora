#include "peekaboo.hpp"
#include "Senkora.hpp"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-promise.h"

#include <v8.h>

void peekaboo(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate *isolate = args.GetIsolate();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Context::Scope ctx_scope(ctx);

    if (args.Length() != 1) {
        Senkora::throwException(ctx, "Expected only 1 argument");
        return;
    }

    if (!args[0]->IsPromise()) {
        Senkora::throwException(ctx, "Expected argument 1 to be Promise");
        return;
    }

    v8::Local<v8::Promise> prom = v8::Local<v8::Promise>::Cast(args[0]);

    if (prom->State() != v8::Promise::PromiseState::kFulfilled) {
        Senkora::throwException(ctx, "Promise must be fulfilled");
        return;
    } else {
        args.GetReturnValue().Set(prom->Result());
    }
}
