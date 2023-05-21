#include "moduleResolver.hpp"
#include "Senkora.hpp"

#include <codecvt>
#include <cstdio>
#include <filesystem>
#include <js/Modules.h>
#include <js/PropertyAndElement.h>
#include <js/String.h>
#include <js/TypeDecls.h>
#include <js/Utility.h>
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

JSObject *resolveHook(JSContext *ctx, JS::HandleValue modulePrivate, JS::HandleObject moduleRequest) {
    JS::Rooted<JSString *> specifierString(
        ctx, JS::GetModuleRequestSpecifier(ctx, moduleRequest)
    );

    if (!specifierString) return nullptr;

    JS::UniqueTwoByteChars specChars(JS_CopyStringCharsZ(ctx, specifierString));
    if (!specChars) return nullptr;

    std::u16string name(specChars.get());
    std::string converted = To_UTF8(name);
    std::string absolute = fs::absolute(converted);

    auto search = moduleRegistry.find(converted);
    if (search != moduleRegistry.end()) return search->second;

    std::string code = Senkora::readFile(converted);
    JS::RootedObject mod(ctx);
    mod = Senkora::CompileModule(ctx, converted.c_str(), code.c_str());
    if (!mod) return nullptr;

    moduleRegistry.emplace(converted, JS::PersistentRootedObject(ctx, mod));
    return mod;
}
