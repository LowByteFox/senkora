#include "moduleResolver.hpp"
#include "Senkora.hpp"
#include <codecvt>
#include <cstdio>
#include <js/Modules.h>
#include <js/String.h>
#include <js/TypeDecls.h>
#include <js/Utility.h>
#include <locale>

std::map<std::u16string, JS::PersistentRootedObject> moduleRegistry;
bool registerBuiltinModule(JSContext *ctx, std::u16string name, JSObject *module) {
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

    auto search = moduleRegistry.find(name);
    if (search != moduleRegistry.end()) return search->second;

    std::string code = Senkora::readFile(To_UTF8(name));
    JS::RootedObject mod(ctx);
    mod = Senkora::CompileModule(ctx, To_UTF8(name).c_str(), code.c_str());
    if (!mod) return nullptr;

    moduleRegistry.emplace(name, JS::PersistentRootedObject(ctx, mod));
    return mod;
}
