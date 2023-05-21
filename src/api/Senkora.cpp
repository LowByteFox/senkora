#include <cstring>
#include <fstream>
#include <js/CompilationAndEvaluation.h>
#include <js/CompileOptions.h>
#include <js/Modules.h>
#include <js/SourceText.h>
#include <js/TypeDecls.h>
#include <jsapi.h>
#include <mozilla/Utf8.h>

#include "Senkora.hpp"

namespace Senkora {
    JSScript *CompileScript(JSContext *ctx, const char *fileName, const char *code) {
        JS::CompileOptions options(ctx);
        options.setFileAndLine(fileName, 1);

        JS::SourceText<mozilla::Utf8Unit> source;
        if (!source.init(ctx, code, strlen(code), JS::SourceOwnership::Borrowed)) return nullptr;

        return JS::Compile(ctx, options, source);
    }

    JSObject *CompileModule(JSContext *ctx, const char *fileName, const char *code) {
        JS::CompileOptions options(ctx);
        options.setFileAndLine(fileName, 1);

        JS::SourceText<mozilla::Utf8Unit> source;
        if (!source.init(ctx, code, strlen(code), JS::SourceOwnership::Borrowed)) return nullptr;

        return JS::CompileModule(ctx, options, source);
    }

    std::string readFile(std::string name) {
        std::ifstream file(name);
        if (!file.good()) return "";

        std::stringstream ss;

        ss << file.rdbuf();

        std::string out = ss.str();

        file.close();

        return out;
    }
}
