#include <cstdio>

#include <iostream>
#include <js/CharacterEncoding.h>
#include <js/TypeDecls.h>
#include <jsapi.h>
#include <js/CompilationAndEvaluation.h>
#include <js/SourceText.h>
#include <js/Conversions.h>
#include <js/Object.h>

#include "boilerplate.hpp"

// This example illustrates the bare minimum you need to do to execute a
// JavaScript program using embedded SpiderMonkey. It does no error handling and
// simply exits if something goes wrong.
//
// See 'boilerplate.cpp' for the parts of this example that are reused in many
// simple embedding examples.
//
// To use the interpreter you need to create a context and a global object, and
// do some setup on both of these. You also need to enter a "realm" (environment
// within one global object) before you can execute code.


std::string FormatString(JSContext* cx, JS::HandleString string) {
    std::string buf = "";

    JS::UniqueChars chars(JS_EncodeStringToUTF8(cx, string));
    if (!chars) {
        JS_ClearPendingException(cx);
        return "[invalid string]";
    }

    buf += chars.get();
    return buf;
}

std::string FormatResult(JSContext* cx, JS::HandleValue value) {
    JS::RootedString str(cx);

    /* Special case format for strings */
    if (value.isString()) {
        str = value.toString();
        return FormatString(cx, str);
    }

    str = JS::ToString(cx, value);

    if (!str) {
        JS_ClearPendingException(cx);
        str = JS_ValueToSource(cx, value);
    }

    if (!str) {
        JS_ClearPendingException(cx);
        if (value.isObject()) {
            const JSClass* klass = JS::GetClass(&value.toObject());
            if (klass)
                str = JS_NewStringCopyZ(cx, klass->name);
            else
                return "[unknown object]";
        } else {
            return "[unknown non-object]";
        }
    }

    if (!str) {
        JS_ClearPendingException(cx);
        return "[invalid class]";
    }

    JS::UniqueChars bytes(JS_EncodeStringToUTF8(cx, str));
    if (!bytes) {
        JS_ClearPendingException(cx);
        return "[invalid string]";
    }

    return bytes.get();
}
static bool ExecuteCodePrintResult(JSContext* cx, const char* code) {
    JS::CompileOptions options(cx);
    options.setFileAndLine("noname", 1);

    JS::SourceText<mozilla::Utf8Unit> source;
    if (!source.init(cx, code, strlen(code), JS::SourceOwnership::Borrowed)) {
        return false;
    }

    JS::RootedValue rval(cx);
    if (!JS::Evaluate(cx, options, source, &rval)) return false;

    std::cout << FormatResult(cx, rval) << std::endl;
    return true;
}

static bool HelloExample(JSContext* cx) {
    JS::RootedObject global(cx, boilerplate::CreateGlobal(cx));
    if (!global) {
        return false;
    }

    JSAutoRealm ar(cx, global);

    return ExecuteCodePrintResult(cx, "\"Hello, World\"");
}

int main(int argc, const char* argv[]) {
    if (!boilerplate::RunExample(HelloExample)) {
        return 1;
    }
    return 0;
}
