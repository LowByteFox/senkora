#include <cstdio>

#include <iostream>
#include <fstream>
#include <js/CharacterEncoding.h>
#include <js/TypeDecls.h>
#include <jsapi.h>
#include <js/CompilationAndEvaluation.h>
#include <js/SourceText.h>
#include <js/Conversions.h>
#include <js/Object.h>
#include <mozilla/Utf8.h>

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
    JS::CompileOptions options(ctx);
    options.setFileAndLine(fileName, 1);

    JS::SourceText<mozilla::Utf8Unit> source;
    if (!source.init(ctx, code, strlen(code), JS::SourceOwnership::Borrowed)) {
        return false;
    }

    JS::RootedValue rval(ctx);
    if (!JS::Evaluate(ctx, options, source, &rval)) return false;

    return true;
}

static bool run(JSContext *ctx, int argc, const char **argv) {
    if (argc == 1) return false;
    const char *fileName = argv[1];

    JS::RootedObject global(ctx, boilerplate::CreateGlobal(ctx));
    if (!global) return false;

    JSAutoRealm ar(ctx, global);

    std::string code = readFile(fileName);
    return executeCode(ctx, code.c_str(), fileName);
}

int main(int argc, const char* argv[]) {
    if (!boilerplate::Run(run, true, argc, argv)) {
        return 1;
    }
    return 0;
}
