#include "moduleResolver.hpp"
#include "Senkora.hpp"

#include <codecvt>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <js/CharacterEncoding.h>
#include <js/Context.h>
#include <js/Modules.h>
#include <js/PropertyAndElement.h>
#include <js/String.h>
#include <js/TypeDecls.h>
#include <js/Utility.h>
#include <jsapi.h>
#include <locale>
#include <stack>

namespace fs = std::filesystem;

std::map<std::string, JS::PersistentRootedObject> moduleRegistry;
extern std::stack<std::string> dirs;

bool registerBuiltinModule(JSContext *ctx, std::string name, JSObject *module) {
    JS::RootedObject mod(ctx, module);
    moduleRegistry.emplace(name, JS::PersistentRootedObject(ctx, mod));
    return true;
}

std::string To_UTF8(const std::u16string &s)
{
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
    return conv.to_bytes(s);
}

bool metadataHook(JSContext* ctx, JS::HandleValue private_ref, JS::HandleObject meta) {
    return true;
}

JSObject *resolveHook(JSContext *ctx, JS::HandleValue modulePrivate, JS::HandleObject moduleRequest) {

    JS::Rooted<JSString *> specifierString(
        ctx, JS::GetModuleRequestSpecifier(ctx, moduleRequest)
    );

    if (!specifierString) return nullptr;

    JS::UniqueTwoByteChars specChars(JS_CopyStringCharsZ(ctx, specifierString));
    if (!specChars) return nullptr;

    std::u16string name(specChars.get());
    std::string converted = To_UTF8(name);
    JS::RootedObject obj(ctx, &modulePrivate.toObject());

    JS::RootedValue val(ctx);
    JS_GetProperty(ctx, obj, "url", &val);
    std::string str = Senkora::jsToString2(ctx, val.toString());
    std::string base = fs::path(str).parent_path();
    base += "/" + converted;
    base = fs::path(base).lexically_normal();

    auto search = moduleRegistry.find(base);
    if (search != moduleRegistry.end()) return search->second;

    std::string code = Senkora::readFile(base);
    JS::RootedObject mod(ctx);
    mod = Senkora::CompileModule(ctx, base.c_str(), code.c_str());
    if (!mod) return nullptr;

    moduleRegistry.emplace(base, JS::PersistentRootedObject(ctx, mod));
    return mod;
}
