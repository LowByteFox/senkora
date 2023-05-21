#include <cstdio>

#include <iostream>
#include <fstream>
#include <iterator>
#include <js/CharacterEncoding.h>
#include <js/PropertyAndElement.h>
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

#include "boilerplate.hpp"

std::string readFile(std::string name) {
    std::ifstream file(name);
    if (!file.good()) return "";

    std::string out;
    std::string line;

    while (std::getline(file, line)) {
        out += line + "\n";
    }

    file.close();

    return out;
}

static bool executeCode(JSContext *ctx, const char* code, const char* fileName) {
    JSScript *compiledScript = Senkora::CompileScript(ctx, fileName, code);
    JS::RootedScript script(ctx);
    script.set(compiledScript);

    if (!JS_ExecuteScript(ctx, script)) return false;

    return true;
}

static bool print(JSContext* ctx, unsigned argc, JS::Value* vp) {
  JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
  JS::HandleValue val = args.get(0);
  if (val.isString()) {
      JS::RootedString str(ctx);
      str.set(val.toString());
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

    JS_DefineFunction(ctx, global, "print", &print, 1, 1);
    std::string code = readFile(fileName);
    return executeCode(ctx, code.c_str(), fileName);
}

int main(int argc, const char* argv[]) {
    if (!boilerplate::Run(run, true, argc, argv)) {
        return 1;
    }
    return 0;
}
