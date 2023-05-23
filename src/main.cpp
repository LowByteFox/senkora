#include <cstdio>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <iterator>
#include <js/CharacterEncoding.h>
#include <js/Class.h>
#include <js/CompileOptions.h>
#include <js/Context.h>
#include <js/GCVector.h>
#include <js/Id.h>
#include <js/Modules.h>
#include <js/PropertyAndElement.h>
#include <js/RootingAPI.h>
#include <js/String.h>
#include <js/TypeDecls.h>
#include <js/Utility.h>
#include <js/Value.h>
#include <jsapi.h>
#include <jsfriendapi.h>
#include <js/CompilationAndEvaluation.h>
#include <js/SourceText.h>
#include <js/Conversions.h>
#include <js/Object.h>
#include <mozilla/Utf8.h>
#include <Senkora.hpp>
#include <ostream>
#include <stack>
#include <string>
#include <unistd.h>

#include "boilerplate.hpp"
#include "moduleResolver.hpp"

#define DEBUG(str) printf("%s\n", str);

namespace fs = std::filesystem;

/* Global variables */
std::stack<std::string> dirs;

static bool executeCode(JSContext *ctx, const char* code, const char* fileName) {
    JS::CompileOptions options(ctx);

    JS::RootedObject mod(ctx, Senkora::CompileModule(ctx, fileName, code));
    if (!mod) {
        boilerplate::ReportAndClearException(ctx);
        return false;
    }

    return true;
}

const char* printNullOrUndefined(JS::HandleValue val) {
    if (val.isUndefined()) {
        return "undefined";
    }

    return "null";
}

void printFunc(JSContext *ctx, JS::HandleValue val, int depth = 0) {
    if (val.isString()) {
        std::string str = Senkora::jsToString(ctx, val.toString());
        if (depth) {
            printf("\"%s\"", str.c_str());
        } else
            printf("%s\n", str.c_str());
    } else if (val.isNumber()) {
        if (depth) {
            printf("%f", val.toNumber());
        } else
            printf("%f\n", val.toNumber());
    } else if (val.isBoolean()) {
        if (depth) {
            printf("%s", val.toBoolean() ? "true" : "false");
        } else
            printf("%s\n", val.toBoolean() ? "true" : "false");
    } else if (val.isNullOrUndefined()) {
        if (depth) {
            printf("%s", printNullOrUndefined(val));
        } else 
            printf("%s\n", printNullOrUndefined(val));
    } else if (val.isObject()) {
        JSObject *obj = &val.toObject();
        JS::HandleObject ob = Senkora::toHandle(&obj);
        if (js::IsFunctionObject(obj)) {
            JS::Value v = JS::Value();
            JS::MutableHandleValue val = Senkora::toMutableHandle(&v);
            JS_GetProperty(ctx, ob, "name", val);

            std::string name = Senkora::jsToString(ctx, val.toString());

            if (!name.length()) {
                name = "anonymous";
            }

            if (depth) {
                printf("(Function: %s)", name.c_str());
            } else
                printf("(Function: %s)\n", name.c_str());
        } else {
            JS::StackGCVector<JS::PropertyKey> idOfProps(ctx);
            JS::MutableHandleIdVector props = Senkora::toMutableHandle(&idOfProps);
            
            js::GetPropertyKeys(ctx, ob, JSITER_SYMBOLS, props);
            depth += 2;
            printf("{\n");
            for (auto prop : props) {
                for (int i = 0; i < depth; i++) {
                    printf(" ");
                }

                printf("%s: ", Senkora::jsToString(ctx, prop.toString()).c_str());
                JS::Value v = JS::Value();
                JS::Handle<JS::PropertyKey> key = Senkora::toHandle(&prop);
                JS::MutableHandleValue val = Senkora::toMutableHandle(&v);

                JS_GetPropertyById(ctx, ob, key, val);
                printFunc(ctx, val, depth);
                printf(",\n");
            }
            depth -= 2;
            for (int i = 0; i < depth; i++) printf(" ");
            printf("}\n");
        }
    }
}

static bool print(JSContext* ctx, unsigned argc, JS::Value* vp) {
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::HandleValue val = args.get(0);

    printFunc(ctx, val);

    args.rval().setNull();
    return true;
}

static bool run(JSContext *ctx, int argc, const char **argv) {
    if (argc == 1) return false;
    const char *fileName = argv[1];

    JS::RootedObject global(ctx, boilerplate::CreateGlobal(ctx));
    if (!global) return false;

    JSAutoRealm ar(ctx, global);

    JS::SetModuleResolveHook(JS_GetRuntime(ctx), resolveHook);
    JS::SetModuleMetadataHook(JS_GetRuntime(ctx), metadataHook);
    JS::RootedObject privateMod(ctx, JS_NewPlainObject(ctx));
    JS::RootedValue v(ctx);

    v.setObject(*privateMod);
    const JSClass *obj = JS::GetClass(&v.getObjectPayload());

    JS_DefineFunction(ctx, privateMod, "print", &print, 1, 0);

    JS_SetProperty(ctx, global, "__PRIVATE_CUZ_FF_STUPID", v);

    std::string currentPath = fs::current_path();
    std::string filePath = fileName;
    if (fileName[0] != '/') {
        filePath = fs::path(currentPath + "/" + fileName).lexically_normal();
    }
    std::string code = Senkora::readFile(filePath);
    if (code.length() == 0) return false;

    return executeCode(ctx, code.c_str(), filePath.c_str());
}

int main(int argc, const char* argv[]) {
    if (!boilerplate::Run(run, true, argc, argv)) {
        return 1;
    }
    return 0;
}
