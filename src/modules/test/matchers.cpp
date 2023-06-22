#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"

#include <iostream>
#include <Senkora.hpp>
#include "../modules.hpp"
#include <v8.h>
#include "matchers.hpp"

extern const Senkora::SharedGlobals globals;

namespace testMatcher
{
    v8::Local<v8::Value> callbackErrOutput(v8::Local<v8::Context> ctx, std::string lineone, std::string linetwo)
    {
        // get file path and line number
        v8::Local<v8::StackTrace> stackTrace = v8::StackTrace::CurrentStackTrace(ctx->GetIsolate(), 1, v8::StackTrace::kScriptName);
        v8::Local<v8::StackFrame> stackFrame = stackTrace->GetFrame(ctx->GetIsolate(), 0);

        const auto &obj = globals.moduleMetadatas[stackFrame->GetScriptId()];
        auto fullPath = obj->Get("url");

        std::string fileName = *v8::String::Utf8Value(ctx->GetIsolate(), fullPath);

        int lineNumber = stackFrame->GetLineNumber();
        std::string lineAsStr = std::to_string(lineNumber);

        // ANSI color codes for different text styles
        std::string reset = "\033[0m";
        std::string bold = "\033[1m";
        std::string red = "\033[31m";
        std::string green = "\033[32m";
        std::string yellow = "\033[33m";
        std::string gray = "\033[90m";

        // print error message
        std::string _lineone = red + bold + "Expected: " + reset + red + lineone + reset + "\n";
        std::string _linetwo = yellow + bold + "Received: " + reset + yellow + linetwo + reset + "\n";
        std::string _lastline = "   at " + gray + fileName + ":" + lineAsStr + reset + "\n";

        return v8::String::NewFromUtf8(ctx->GetIsolate(), (_lineone + _linetwo + _lastline).c_str()).ToLocalChecked();
    }

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

        if (!holder->Has(ctx, expectedKey).FromMaybe(false))
        {
            return v8::Undefined(isolate);
        }

        v8::Local<v8::Value> val = holder->Get(ctx, expectedKey).ToLocalChecked();
        return val;
    }

    void compareArrays(v8::Local<v8::Context> &ctx, v8::Local<v8::Array> &expectedArray, v8::Local<v8::Array> &actualArray, bool &result)
    {
        int length = expectedArray->Length();
        for (uint32_t i = 0; i < length; i++)
        {
            v8::Local<v8::Value> exAr = expectedArray->Get(ctx, i).ToLocalChecked();
            v8::Local<v8::Value> acAr = actualArray->Get(ctx, i).ToLocalChecked();

            if (!expectedArray->Get(ctx, i).ToLocalChecked()->StrictEquals(actualArray->Get(ctx, i).ToLocalChecked()))
            {
                result = false;
                break;
            }
        }
    }

    void compareObjects(v8::Local<v8::Context> &ctx, v8::Local<v8::Array> &expectedKeys, v8::Local<v8::Array> &actualKeys,
                        v8::Local<v8::Object> &expectedObject, v8::Local<v8::Object> &actualObject, bool &result)
    {
        for (uint32_t i = 0; i < expectedKeys->Length(); i++)
        {
            v8::Local<v8::Value> expectedKey = expectedKeys->Get(ctx, i).ToLocalChecked();
            v8::Local<v8::Value> actualKey = actualKeys->Get(ctx, i).ToLocalChecked();

            if (!expectedKey->StrictEquals(actualKey))
            {
                result = false;
                break;
            }

            v8::Local<v8::Value> expectedValue = expectedObject->Get(ctx, expectedKey).ToLocalChecked();
            v8::Local<v8::Value> actualValue = actualObject->Get(ctx, actualKey).ToLocalChecked();

            if (!expectedValue->StrictEquals(actualValue))
            {
                result = false;
                break;
            }
        }
    }

    bool toEqual(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handleScope(isolate);

        bool negate = getNegate(ctx, args.Holder());

        if (args.Length() < 0 || args.Length() > 1)
        {
            Senkora::throwException(ctx, "Expected 1 argument");
            return false;
        }

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        bool result = true;

        if (expected->IsArray())
        {
            v8::Local<v8::Array> expectedArray = expected.As<v8::Array>();
            v8::Local<v8::Array> actualArray = actual.As<v8::Array>();

            if (expectedArray->Length() != actualArray->Length())
            {
                result = false;
            }
            else
            {
                compareArrays(ctx, expectedArray, actualArray, result);
            }
        }
        else if (expected->IsObject())
        {
            v8::Local<v8::Object> expectedObject = expected.As<v8::Object>();
            v8::Local<v8::Object> actualObject = actual.As<v8::Object>();

            v8::Local<v8::Array> expectedKeys = expectedObject->GetOwnPropertyNames(ctx).ToLocalChecked();
            v8::Local<v8::Array> actualKeys = actualObject->GetOwnPropertyNames(ctx).ToLocalChecked();

            if (expectedKeys->Length() != actualKeys->Length())
            {
                result = false;
            }
            else
            {
                compareObjects(ctx, expectedKeys, actualKeys, expectedObject, actualObject, result);
            }
        }
        else
        {
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

        if (r)
            return;

        bool negate = getNegate(ctx, args.Holder());
        ctx->SetEmbedderData(159, v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        std::string notOrNada = negate ? "[Not] " : "";
        std::string outExpected = notOrNada + *v8::String::Utf8Value(ctx->GetIsolate(), expected);
        std::string outReceived = *v8::String::Utf8Value(ctx->GetIsolate(), actual);

        ctx->SetEmbedderData(160, callbackErrOutput(ctx, outExpected, outReceived));
        return;
    }

    bool toBeBoolean(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handleScope(isolate);

        bool negate = getNegate(ctx, args.Holder());
        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());

        bool result = expected->IsBoolean();

        if (negate)
            result = !result;

        args.GetReturnValue().Set(result ? v8::True(isolate) : v8::False(isolate));
        return result;
    }

    void toBeBooleanCallback(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Context> ctx = args.GetIsolate()->GetCurrentContext();

        bool r = toBeBoolean(args, ctx);

        if (r)
            return;

        bool negate = getNegate(ctx, args.Holder());
        ctx->SetEmbedderData(159, v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        std::string notOrNada = negate ? "[Not] " : "";
        std::string outExpected = notOrNada + "Boolean";
        std::string outReceived = *v8::String::Utf8Value(ctx->GetIsolate(), actual);

        ctx->SetEmbedderData(160, callbackErrOutput(ctx, outExpected, outReceived));
        return;
    }

    bool toBeTrue(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handleScope(isolate);

        if (args.Length() > 0)
        {
            Senkora::throwException(ctx, "toBeTrue() requires no arguments");
            return false;
        }

        bool negate = getNegate(ctx, args.Holder());
        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());

        bool result = expected->IsTrue();

        if (negate)
            result = !result;

        args.GetReturnValue().Set(result ? v8::True(isolate) : v8::False(isolate));
        return result;
    }

    void toBeTrueCallback(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Context> ctx = args.GetIsolate()->GetCurrentContext();

        bool r = toBeTrue(args, ctx);

        if (r)
            return;

        bool negate = getNegate(ctx, args.Holder());
        ctx->SetEmbedderData(159, v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        std::string notOrNada = negate ? "[Not] " : "";
        std::string outExpected = notOrNada + "true";
        std::string outReceived = *v8::String::Utf8Value(ctx->GetIsolate(), expected);

        ctx->SetEmbedderData(160, callbackErrOutput(ctx, outExpected, outReceived));
        return;
    }

    bool toBeFalse(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handleScope(isolate);

        bool negate = getNegate(ctx, args.Holder());
        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());

        bool result = expected->IsFalse();

        if (negate)
            result = !result;

        args.GetReturnValue().Set(result ? v8::True(isolate) : v8::False(isolate));
        return result;
    }

    void toBeFalseCallback(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Context> ctx = args.GetIsolate()->GetCurrentContext();

        bool r = toBeFalse(args, ctx);

        if (r)
            return;

        bool negate = getNegate(ctx, args.Holder());
        ctx->SetEmbedderData(159, v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        std::string notOrNada = negate ? "[Not] " : "";
        std::string outExpected = notOrNada + "false";
        std::string outReceived = *v8::String::Utf8Value(ctx->GetIsolate(), expected);

        ctx->SetEmbedderData(160, callbackErrOutput(ctx, outExpected, outReceived));
        return;
    }

    bool toBeArray(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handleScope(isolate);

        bool negate = getNegate(ctx, args.Holder());
        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());

        bool result = expected->IsArray();

        if (negate)
            result = !result;

        args.GetReturnValue().Set(result ? v8::True(isolate) : v8::False(isolate));
        return result;
    }

    void toBeArrayCallback(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Context> ctx = args.GetIsolate()->GetCurrentContext();

        bool r = toBeArray(args, ctx);

        if (r)
            return;

        bool negate = getNegate(ctx, args.Holder());

        ctx->SetEmbedderData(159, v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        std::string notOrNada = negate ? "[Not] " : "";

        std::string outExpected = notOrNada + "Array";
        std::string outReceived = *v8::String::Utf8Value(ctx->GetIsolate(), actual);

        ctx->SetEmbedderData(160, callbackErrOutput(ctx, outExpected, outReceived));
        return;
    }

    bool toBeArrayOfSize(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handleScope(isolate);

        if (args.Length() != 1)
        {
            Senkora::throwException(ctx, "toBeArrayOfSize() requires 1 argument");
            return false;
        }

        bool negate = getNegate(ctx, args.Holder());

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        bool result = expected->IsArray();

        if (result)
        {
            v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(expected);
            result = array->Length() == actual->NumberValue(ctx).FromJust();
        }

        if (negate)
            result = !result;

        args.GetReturnValue().Set(result ? v8::True(isolate) : v8::False(isolate));
        return result;
    }

    void toBeArrayOfSizeCallback(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Context> ctx = args.GetIsolate()->GetCurrentContext();

        bool r = toBeArrayOfSize(args, ctx);

        if (r)
            return;

        bool negate = getNegate(ctx, args.Holder());
        ctx->SetEmbedderData(159, v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        std::string notOrNada = negate ? "[Not] " : "";
        std::string outExpected = notOrNada + "Array of size " + std::to_string(actual->ToInteger(ctx).ToLocalChecked()->Value());
        std::string outReceived = "";

        if (expected->IsArray())
            outReceived = "Array of size " + std::to_string(v8::Local<v8::Array>::Cast(expected)->Length());
        else
            outReceived = *v8::String::Utf8Value(ctx->GetIsolate(), expected);

        ctx->SetEmbedderData(160, callbackErrOutput(ctx, outExpected, outReceived));
        return;
    }
}