#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"

#include <Senkora.hpp>
#include "../modules.hpp"
#include <v8.h>
#include "matchers.hpp"
#include "constants.hpp"

extern const Senkora::SharedGlobals globals;

namespace testMatcher
{
    std::string stringifyForOutput(v8::Local<v8::Context> ctx, v8::Local<v8::Value> value)
    {
       if (value->IsObject()) {
            auto _value = value.As<v8::Object>();
            return *v8::String::Utf8Value(ctx->GetIsolate(), v8::JSON::Stringify(ctx, _value).ToLocalChecked());
        } else {
            return *v8::String::Utf8Value(ctx->GetIsolate(), value);
       }
    }

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

        // print error message
        std::string _lineone = testConst::getColor("red") + testConst::getColor("bold") + "Expected: " + testConst::getColor("reset") + testConst::getColor("red") + lineone + testConst::getColor("reset") + "\n";
        std::string _linetwo = testConst::getColor("yellow") + testConst::getColor("bold") + "Received: " + testConst::getColor("reset") + testConst::getColor("yellow") + linetwo + testConst::getColor("reset") + "\n";
        std::string _lastline = "   at " + testConst::getColor("gray") + fileName + ":" + lineAsStr + testConst::getColor("reset") + "\n";

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
        if (expectedArray->Length() != actualArray->Length())
        {
            result = false;
            return;
        }

        for (uint32_t i = 0; i < expectedArray->Length(); i++)
        {
            v8::Local<v8::Value> expectedValue = expectedArray->Get(ctx, i).ToLocalChecked();
            v8::Local<v8::Value> actualValue = actualArray->Get(ctx, i).ToLocalChecked();

            if (expectedValue->IsObject() && actualValue->IsObject()) {
                auto _expVal = expectedValue.As<v8::Object>();
                auto _acpVal = actualValue.As<v8::Object>();

                compareObjects(ctx, _expVal, _acpVal, result);
            } else if (expectedValue->IsArray() && actualValue->IsArray()) {
                auto _expVal = expectedValue.As<v8::Array>();
                auto _acpVal = actualValue.As<v8::Array>();

                compareArrays(ctx, _expVal, _acpVal, result);
            } else {
                result = expectedValue->StrictEquals(actualValue);
            }

            if (!result)
                return;
        }
    }

    void compareObjects(v8::Local<v8::Context> &ctx, v8::Local<v8::Object> &expectedObject, v8::Local<v8::Object> &actualObject, bool &result)
    {
        v8::Local<v8::Array> expectedKeys = expectedObject->GetOwnPropertyNames(ctx).ToLocalChecked();
        v8::Local<v8::Array> actualKeys = actualObject->GetOwnPropertyNames(ctx).ToLocalChecked();

        if (expectedKeys->Length() != actualKeys->Length())
        {
            result = false;
            return;
        }

        for (uint32_t i = 0; i < expectedKeys->Length(); i++)
        {
            v8::Local<v8::Value> expectedKey = expectedKeys->Get(ctx, i).ToLocalChecked();
            v8::Local<v8::Value> actualKey = actualKeys->Get(ctx, i).ToLocalChecked();

            if (!expectedKey->StrictEquals(actualKey))
            {
                result = false;
                return;
            }

            v8::Local<v8::Value> expectedValue = expectedObject->Get(ctx, expectedKey).ToLocalChecked();
            v8::Local<v8::Value> actualValue = actualObject->Get(ctx, actualKey).ToLocalChecked();

            if (expectedValue->IsObject() && actualValue->IsObject()) {
                auto _expVal = expectedValue.As<v8::Object>();
                auto _acpVal = actualValue.As<v8::Object>();

                compareObjects(ctx, _expVal, _acpVal, result);
            } else if (expectedValue->IsArray() && actualValue->IsArray()) {
                auto _expVal = expectedValue.As<v8::Array>();
                auto _acpVal = actualValue.As<v8::Array>();

                compareArrays(ctx, _expVal, _acpVal, result);
            } else {
                result = expectedValue->StrictEquals(actualValue);
            }

            if (!result)
                return;
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

        if (expected->IsArray() && actual->IsArray())
        {
            v8::Local<v8::Array> expectedArray = expected.As<v8::Array>();
            v8::Local<v8::Array> actualArray = actual.As<v8::Array>();

            compareArrays(ctx, expectedArray, actualArray, result);
        }
        else if (expected->IsObject() && actual->IsObject())
        {
            v8::Local<v8::Object> expectedObject = expected.As<v8::Object>();
            v8::Local<v8::Object> actualObject = actual.As<v8::Object>();

            compareObjects(ctx, expectedObject, actualObject, result);
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
        ctx->SetEmbedderData(testConst::getTestEmbedderNum("error"), v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        std::string notOrNada = negate ? "[Not] " : "";
        std::string outExpected = notOrNada + stringifyForOutput(ctx, expected); *v8::String::Utf8Value(ctx->GetIsolate(), expected);
        std::string outReceived = stringifyForOutput(ctx, actual);

        ctx->SetEmbedderData(testConst::getTestEmbedderNum("errorStr"), callbackErrOutput(ctx, outExpected, outReceived));
        return;
    }

    bool toBeEmpty(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handleScope(isolate);

        bool negate = getNegate(ctx, args.Holder());
        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());

        bool result = expected->IsUndefined() || expected->IsNull() ||
                      expected->IsNullOrUndefined() || expected->IsFalse() ||
                      expected->IsTrue() || expected->IsString() && expected.As<v8::String>()->Length() == 0 ||
                      expected->IsArray() && expected.As<v8::Array>()->Length() == 0 ||
                      expected->IsObject() && 
                      expected.As<v8::Object>()->GetOwnPropertyNames(ctx).ToLocalChecked()->Length() == 0;

        if (negate)
            result = !result;

        args.GetReturnValue().Set(result ? v8::True(isolate) : v8::False(isolate));
        return result;
    }

    void toBeEmptyCallback(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Context> ctx = args.GetIsolate()->GetCurrentContext();

        bool r = toBeEmpty(args, ctx);

        if (r)
            return;

        bool negate = getNegate(ctx, args.Holder());
        ctx->SetEmbedderData(testConst::getTestEmbedderNum("error"), v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        std::string notOrNada = negate ? "[Not] " : "";
        std::string outExpected = notOrNada + stringifyForOutput(ctx, expected);
        std::string outReceived = stringifyForOutput(ctx, actual);

        ctx->SetEmbedderData(testConst::getTestEmbedderNum("errorStr"), callbackErrOutput(ctx, outExpected, outReceived));
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
        ctx->SetEmbedderData(testConst::getTestEmbedderNum("error"), v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());

        std::string notOrNada = negate ? "[Not] " : "";
        std::string outExpected = notOrNada + "boolean";
        std::string outReceived = stringifyForOutput(ctx, expected->TypeOf(ctx->GetIsolate()));

        ctx->SetEmbedderData(testConst::getTestEmbedderNum("errorStr"), callbackErrOutput(ctx, outExpected, outReceived));
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
        ctx->SetEmbedderData(testConst::getTestEmbedderNum("error"), v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        std::string notOrNada = negate ? "[Not] " : "";
        std::string outExpected = notOrNada + "true";
        std::string outReceived = stringifyForOutput(ctx, expected);

        ctx->SetEmbedderData(testConst::getTestEmbedderNum("errorStr"), callbackErrOutput(ctx, outExpected, outReceived));
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
        ctx->SetEmbedderData(testConst::getTestEmbedderNum("error"), v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        std::string notOrNada = negate ? "[Not] " : "";
        std::string outExpected = notOrNada + "false";
        std::string outReceived = stringifyForOutput(ctx, expected);

        ctx->SetEmbedderData(testConst::getTestEmbedderNum("errorStr"), callbackErrOutput(ctx, outExpected, outReceived));
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

        ctx->SetEmbedderData(testConst::getTestEmbedderNum("error"), v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        std::string notOrNada = negate ? "[Not] " : "";

        std::string outExpected = notOrNada + "Array";
        std::string outReceived = stringifyForOutput(ctx, actual);

        ctx->SetEmbedderData(testConst::getTestEmbedderNum("errorStr"), callbackErrOutput(ctx, outExpected, outReceived));
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
        ctx->SetEmbedderData(testConst::getTestEmbedderNum("error"), v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        std::string notOrNada = negate ? "[Not] " : "";
        std::string outExpected = notOrNada + "Array of size " + std::to_string(actual->ToInteger(ctx).ToLocalChecked()->Value());
        std::string outReceived = "";

        if (expected->IsArray())
            outReceived = "Array of size " + std::to_string(v8::Local<v8::Array>::Cast(expected)->Length());
        else
            outReceived = stringifyForOutput(ctx, expected);

        ctx->SetEmbedderData(testConst::getTestEmbedderNum("errorStr"), callbackErrOutput(ctx, outExpected, outReceived));
        return;
    }

    bool toBeObject(const v8::FunctionCallbackInfo<v8::Value> &args, v8::Local<v8::Context> ctx)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope handleScope(isolate);

        bool negate = getNegate(ctx, args.Holder());
        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());

        bool result = expected->IsObject() && !expected->IsArray();

        if (negate)
            result = !result;

        args.GetReturnValue().Set(result ? v8::True(isolate) : v8::False(isolate));
        return result;
    }

    void toBeObjectCallback(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Context> ctx = args.GetIsolate()->GetCurrentContext();

        bool r = toBeObject(args, ctx);

        if (r)
            return;

        bool negate = getNegate(ctx, args.Holder());

        ctx->SetEmbedderData(testConst::getTestEmbedderNum("error"), v8::Boolean::New(args.GetIsolate(), r));

        v8::Local<v8::Value> expected = getExpected(ctx, args.Holder());
        v8::Local<v8::Value> actual = args[0];

        std::string notOrNada = negate ? "[Not] " : "";

        std::string outExpected = notOrNada + "Object";
        std::string outReceived = stringifyForOutput(ctx, actual);

        ctx->SetEmbedderData(testConst::getTestEmbedderNum("errorStr"), callbackErrOutput(ctx, outExpected, outReceived));
        return;
    }
}
