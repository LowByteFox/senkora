#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"

#include <iostream>
#include <Senkora.hpp>
#include "../modules.hpp"
#include <v8.h>
#include "matchers.hpp"

namespace testMatcher
{
    bool getNegate(v8::Local<v8::Context> ctx, v8::Local<v8::Object> holder)
    {
        v8::Isolate *isolate = ctx->GetIsolate();

        if (!holder->Has(ctx, v8::String::NewFromUtf8(isolate, "negate").ToLocalChecked()).FromMaybe(false))
        {
            return false;
        }

        v8::Local<v8::Value> negateValue = holder->Get(ctx, v8::String::NewFromUtf8(isolate, "negate").ToLocalChecked()).ToLocalChecked();

        if (!negateValue->IsBoolean())
        {
            Senkora::throwException(ctx, "Expected a boolean value for `negate` property");
            return false;
        }

        return negateValue.As<v8::Boolean>()->Value();
    }

    v8::Local<v8::Value> getExpected(v8::Local<v8::Context> ctx, v8::Local<v8::Object> holder)
    {
        v8::Isolate *isolate = ctx->GetIsolate();
        v8::Local<v8::String> expectedKey = v8::String::NewFromUtf8(isolate, "expected").ToLocalChecked();

        if (!holder->Has(ctx, expectedKey).FromMaybe(false)) {;
            return v8::Undefined(isolate);
        }

        v8::Local<v8::Value> val = holder->Get(ctx, expectedKey).ToLocalChecked();
        return val;
    }

    bool toEqual(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handleScope(isolate);

        bool negate = getNegate(ctx, args.Holder());

        if (args.Length() < 0 || args.Length() > 1) {
            std::cout << "args.Length() < 2" << std::endl;
            Senkora::throwException(ctx, "Expected 1 argument");
            return false;
        }

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        bool result = true;

        if (expected->IsArray()) {
            v8::Local<v8::Array> expectedArray = expected.As<v8::Array>();
            v8::Local<v8::Array> actualArray = actual.As<v8::Array>();

            if (expectedArray->Length() != actualArray->Length()) {
                result = false;
            } else {
                for (uint32_t i = 0; i < expectedArray->Length(); i++) {
                    
                    v8::Local<v8::Value> exAr = expectedArray->Get(ctx, i).ToLocalChecked();
                    v8::Local<v8::Value> acAr = actualArray->Get(ctx, i).ToLocalChecked();

                    if (!expectedArray->Get(ctx, i).ToLocalChecked()->StrictEquals(actualArray->Get(ctx, i).ToLocalChecked())) {
                        result = false;
                        break;
                    }
                }
            }
        } else if (expected->IsObject()) {
            v8::Local<v8::Object> expectedObject = expected.As<v8::Object>();
            v8::Local<v8::Object> actualObject = actual.As<v8::Object>();

            v8::Local<v8::Array> expectedKeys = expectedObject->GetOwnPropertyNames(ctx).ToLocalChecked();
            v8::Local<v8::Array> actualKeys = actualObject->GetOwnPropertyNames(ctx).ToLocalChecked();

            if (expectedKeys->Length() != actualKeys->Length()) {
                result = false;
            } else {
                for (uint32_t i = 0; i < expectedKeys->Length(); i++) {
                    v8::Local<v8::Value> expectedKey = expectedKeys->Get(ctx, i).ToLocalChecked();
                    v8::Local<v8::Value> actualKey = actualKeys->Get(ctx, i).ToLocalChecked();

                    if (!expectedKey->StrictEquals(actualKey)) {
                        result = false;
                        break;
                    }

                    v8::Local<v8::Value> expectedValue = expectedObject->Get(ctx, expectedKey).ToLocalChecked();
                    v8::Local<v8::Value> actualValue = actualObject->Get(ctx, actualKey).ToLocalChecked();

                    if (!expectedValue->StrictEquals(actualValue)) {
                        result = false;
                        break;
                    }
                }
            } 
        } else {
            result = expected->StrictEquals(actual);
        }

        if (negate)
            result = !result;

        args.GetReturnValue().Set(result ? v8::True(isolate) : v8::False(isolate));
        return result;
    }


    void toEqualCallback(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Context> ctx = args.GetIsolate()->GetCurrentContext();
        bool r = toEqual(args, ctx);
        bool negate = getNegate(ctx, args.Holder());

        if (r == false) {
            v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
            v8::Local<v8::Value> actual = args[0];
            std::string notOrNada = negate ? "[NOT] " : "";
            std::cout << notOrNada << "Expected: " << *v8::String::Utf8Value(ctx->GetIsolate(), expected) << std::endl;
            std::cout << notOrNada << "Actual: " << *v8::String::Utf8Value(ctx->GetIsolate(), actual) << std::endl;
        } else {
            std::cout << "Test passed" << std::endl;
        }

        return;
    }


}