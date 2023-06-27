#include "Error.hpp"
#include "Senkora.hpp"
#include "v8-container.h"
#include "v8-context.h"
#include "v8-function-callback.h"
#include "v8-function.h"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-maybe.h"
#include "v8-object.h"
#include "v8-primitive.h"
#include "v8-template.h"
#include "v8-value.h"
#include <string>

namespace Senkora::Error {
    v8::Local<v8::Array> returnError(v8::Isolate *isolate, v8::Local<v8::Value> val, std::string msg) {
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope ctx_scope(ctx);

        v8::Local<v8::Array> arr = v8::Array::New(isolate);
        v8::Maybe<bool> didItPass = arr->Set(ctx, 0, val);

        // make the error class
        v8::Local<v8::FunctionTemplate> clsTemplate = makeError(isolate);
        v8::Local<v8::Function> construct = clsTemplate->GetFunction(ctx).ToLocalChecked();
        v8::Local<v8::Value> argv[1];
        argv[0] = v8::String::NewFromUtf8(isolate, msg.c_str()).ToLocalChecked();

        v8::Local<v8::Object> instance = construct->NewInstance(ctx, 1, argv).ToLocalChecked();
        v8::Maybe<bool> _ = instance->SetPrototype(ctx, clsTemplate->InstanceTemplate()->NewInstance(ctx).ToLocalChecked());

        v8::Maybe<bool> setInstance = arr->Set(ctx, 1, instance);
        return arr;
    }

    void constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        if (args.Length() < 1) {
            Senkora::throwException(ctx, "Not enough args");
            return;
        }
        if (args.IsConstructCall()) {
            v8::Local<v8::Function> fn = v8::Local<v8::Function>::Cast(args.This());
            v8::Maybe<bool> ret = fn->Set(ctx, v8::String::NewFromUtf8(isolate, "msg").ToLocalChecked(), args[0]);
            args.GetReturnValue().Set(fn);
        } else {
            Senkora::throwException(ctx, "SenkoraError was not called with 'new'!");
        }
    }

    v8::Local<v8::FunctionTemplate> makeError(v8::Isolate *isolate) {
        v8::Local<v8::FunctionTemplate> cls = v8::FunctionTemplate::New(isolate, constructor);
        cls->SetClassName(v8::String::NewFromUtf8(isolate, "SenkoraError").ToLocalChecked());
        cls->Set(isolate, "msg", v8::String::NewFromUtf8(isolate, "").ToLocalChecked());
        cls->InstanceTemplate()->SetInternalFieldCount(1);

        return cls;
    }
}
