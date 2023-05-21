#include <cstdio>

#include <iostream>
#include <fstream>
#include <iterator>
#include <js/CharacterEncoding.h>
#include <js/Context.h>
#include <js/Modules.h>
#include <js/PropertyAndElement.h>
#include <js/RootingAPI.h>
#include <js/TypeDecls.h>
#include <js/Utility.h>
#include <js/Value.h>
#include <jsapi.h>
#include <js/CompilationAndEvaluation.h>
#include <js/SourceText.h>
#include <js/Conversions.h>
#include <js/Object.h>
#include <mozilla/Utf8.h>
#include <Senkora.hpp>
#include <string>

#include "boilerplate.hpp"
#include "moduleResolver.hpp"

static bool executeCode(JSContext *ctx, const char* code, const char* fileName) {
    JS::RootedObject mod(ctx, Senkora::CompileModule(ctx, fileName, code));
    if (!mod) {
        boilerplate::ReportAndClearException(ctx);
        return false;
    }

    if (!JS::ModuleInstantiate(ctx, mod)) { 
        boilerplate::ReportAndClearException(ctx);
        return false;
    }

    JS::RootedValue rval(ctx);
    if (!JS::ModuleEvaluate(ctx, mod, &rval)) {
        boilerplate::ReportAndClearException(ctx);
        return false;
    }

    return true;
}

static bool print(JSContext* ctx, unsigned argc, JS::Value* vp) {
  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::HandleValue val = args.get(0);
  if (val.isString()) {
      JS::RootedString str(ctx, val.toString());
      JS::UniqueChars chars(JS_EncodeStringToUTF8(ctx, str));
      std::cout << chars.get() << std::endl;
  }
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
    JS::RootedObject privateMod(ctx, JS_NewPlainObject(ctx));
    JS::RootedValue v(ctx);
    v.setObject(*privateMod);

    JS_DefineFunction(ctx, privateMod, "print", &print, 0, 0);

    JS_SetProperty(ctx, global, "__PRIVATE", v);

    std::string code = Senkora::readFile(fileName);
    return executeCode(ctx, code.c_str(), fileName);
}

int main(int argc, const char* argv[]) {
    if (!boilerplate::Run(run, true, argc, argv)) {
        return 1;
    }
    return 0;
}
