#include <cstdio>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <iterator>
#include <js/CallAndConstruct.h>
#include <js/CharacterEncoding.h>
#include <js/Class.h>
#include <js/CompileOptions.h>
#include <js/Context.h>
#include <js/ErrorInterceptor.h>
#include <js/ErrorReport.h>
#include <js/Exception.h>
#include <js/GCVector.h>
#include <js/GlobalObject.h>
#include <js/Id.h>
#include <js/Initialization.h>
#include <js/Modules.h>
#include <js/PropertyAndElement.h>
#include <js/RootingAPI.h>
#include <js/String.h>
#include <js/TypeDecls.h>
#include <js/Warnings.h>
#include <js/Utility.h>
#include <js/Value.h>
#include <js/ValueArray.h>
#include <jsapi.h>
#include <jsfriendapi.h>
#include <js/CompilationAndEvaluation.h>
#include <js/SourceText.h>
#include <js/Conversions.h>
#include <js/Object.h>
#include <mozilla/Utf8.h>
#include <Senkora.hpp>
#include <mozjs-102/js/Context.h>
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

    if (!JS::ModuleLink(ctx, mod)) {
        boilerplate::ReportAndClearException(ctx);
        return false;
    }

    JS::RootedValue rval(ctx);
    if (!JS::ModuleEvaluate(ctx, mod, &rval)) {
        boilerplate::ReportAndClearException(ctx);
        return false;
    }
    JS::RootedValue exception(ctx);
    JS_ClearPendingException(ctx);
    auto pending = JS_IsExceptionPending(ctx);
    if (pending) {
    }
    return true;
}

const char* printNullOrUndefined(JS::HandleValue val) {
    if (val.isUndefined()) {
        return "undefined";
    }

    return "null";
}

void printFunc(JSContext *ctx, JS::HandleValue val, int depth = 0, bool isNext = false) {
    if (val.isString()) {
        std::string str = Senkora::jsToString(ctx, val.toString());
        if (depth) {
            printf("\"%s\"", str.c_str());
        } else
            printf("%s\n", str.c_str());
    } else if (val.isNumber()) {
        if (depth) {
            if (val.isDouble()) {
                printf("%f", val.toNumber());
            } else printf("%d", val.toInt32());
        } else {
            if (val.isDouble()) printf("%f\n", val.toNumber());
            else printf("%d\n", val.toInt32());
        }
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
            std::string type = "Function";

            if (!name.length()) {
                name = "anonymous";
            }

            JS::MutableHandleValue val2 = Senkora::toMutableHandle(&v);
            JS_GetProperty(ctx, ob, "constructor", val2);
            JSObject *construct = &val2.toObject();
            JS::HandleObject constructor = Senkora::toHandle(&construct);

            JS::MutableHandleValue val3 = Senkora::toMutableHandle(&v);
            JS_GetProperty(ctx, constructor, "toString", val3);
            JSObject *funcObj = &val3.toObject();
            if (js::IsFunctionObject(funcObj)) {
                JSFunction *func = JS_GetObjectFunction(funcObj);
                JS::HandleFunction funcHandle = Senkora::toHandle(&func);

                JS::Value rval = JS::Value();
                JS::MutableHandleValue rval2 = Senkora::toMutableHandle(&rval);
                JS_CallFunction(ctx, ob, funcHandle, JS::HandleValueArray::empty(), rval2);
                if (rval2.isString()) {

                    std::string code = Senkora::jsToString(ctx, rval2.toString());
                    if (!code.rfind("class", 0)) {
                        type = "Class";
                    }
                }
            }

            if (depth) {
                printf("(%s: %s)", type.c_str(), name.c_str());
            } else
                printf("(%s: %s)\n", type.c_str(), name.c_str());
        } else {
            JS::StackGCVector<JS::PropertyKey> idOfProps(ctx);
            JS::MutableHandleIdVector props = Senkora::toMutableHandle(&idOfProps);
            
            if(!js::GetPropertyKeys(ctx, ob, JSITER_SYMBOLS, props)) {
                boilerplate::ReportAndClearException(ctx);
                return;
            }
            depth += 2;
            printf("{\n");
            for (int i = 0; i < props.length() - 1; i++) {
                auto prop = props[i].get();

                for (int j = 0; j < depth; j++) {
                    printf(" ");
                }

                printf("%s: ", Senkora::jsToString(ctx, prop.toString()).c_str());
                JS::Value v = JS::Value();
                JS::Handle<JS::PropertyKey> key = Senkora::toHandle(&prop);
                JS::MutableHandleValue val = Senkora::toMutableHandle(&v);

                if(!JS_GetPropertyById(ctx, ob, key, val)) {
                    boilerplate::ReportAndClearException(ctx);
                    return;
                };
                printFunc(ctx, val, depth, true);
                printf(",\n");
            }

            auto lastProp = props[props.length() - 1].get();
            for (int j = 0; j < depth; j++) {
                printf(" ");
            }

            printf("%s: ", Senkora::jsToString(ctx, lastProp.toString()).c_str());
            JS::Value v = JS::Value();
            JS::Handle<JS::PropertyKey> key = Senkora::toHandle(&lastProp);
            JS::MutableHandleValue val = Senkora::toMutableHandle(&v);

            if(!JS_GetPropertyById(ctx, ob, key, val)) {
                boilerplate::ReportAndClearException(ctx);
                return;
            }
            printFunc(ctx, val, depth);
            if (!val.isObject() || js::IsFunctionObject(&val.toObject()))
                printf("\n");
            depth -= 2;
            for (int i = 0; i < depth; i++) printf(" ");
            printf("}");
            if (!isNext) printf("\n");
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

class ErrorHandler : public JSErrorInterceptor {
    void interceptError(JSContext* ctx, JS::HandleValue error) {
        if (error.isString()) {
            std::string msg = Senkora::jsToString(ctx, error.toString());
            printf("%s\n", msg.c_str());
            JS_DestroyContext(ctx);
            JS_ShutDown();
            exit(1);
        }
        JSObject *obj = &error.toObject();
        JS::HandleObject ob = Senkora::toHandle(&obj);
        JS::Value v = JS::Value();
        JS::MutableHandleValue toStr = Senkora::toMutableHandle(&v);

        JS_GetProperty(ctx, ob, "toString", toStr);

        if(toStr.isObject()) {
            JSFunction *f = JS_GetObjectFunction(&toStr.toObject());
            JS::HandleFunction func = Senkora::toHandle(&f);

            JS::Value r = JS::Value();
            JS::MutableHandleValue rval = Senkora::toMutableHandle(&r);

            JS_CallFunction(ctx, ob, func, JS::HandleValueArray::empty(), rval);

            if (rval.isString()) {
                std::string out = Senkora::jsToString(ctx, rval.toString());
                printf("%s\n", out.c_str());
                JS_DestroyContext(ctx);
                JS_ShutDown();
                exit(1);
            }
        }
    }
};

static bool run(JSContext *ctx, int argc, const char **argv) {
    if (argc == 1) return false;
    const char *fileName = argv[1];

    if (!js::UseInternalJobQueues(ctx)) return false;

    if (!JS::InitSelfHostedCode(ctx)) return false;

    JS::RootedObject global(ctx, boilerplate::CreateGlobal(ctx));
    if (!global) return false;

    JSAutoRealm ar(ctx, global);
    JS_SetErrorInterceptorCallback(JS_GetRuntime(ctx), new ErrorHandler());
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
    if (!boilerplate::Run(run, false, argc, argv)) {
        return 1;
    }
    return 0;
}
