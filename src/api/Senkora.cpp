#include <cstdio>
#include <cstring>
#include <fstream>
#include <js/CompilationAndEvaluation.h>
#include <js/CompileOptions.h>
#include <js/Modules.h>
#include <js/PropertyAndElement.h>
#include <js/SourceText.h>
#include <js/String.h>
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

        JS::RootedObject mod(ctx, JS::CompileModule(ctx, options, source));
        JS::RootedValue mod2(ctx);
        JS::RootedValue val(ctx);
        JS::RootedString test(ctx, JS_NewStringCopyZ(ctx, fileName));
        JS::RootedValue nig(ctx);
        nig.setString(test);
        JS::RootedObject meta(ctx, JS_NewPlainObject(ctx));
        JS_SetProperty(ctx, meta, "url", nig);
        mod2.setObject(*meta.get());
        JS::SetModulePrivate(mod, mod2);

        return mod;
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
