#include <Senkora.hpp>

#include "cli.hpp"
#include "modules/modules.hpp"
#include "v8-container.h"
#include "v8-context.h"
#include "v8-data.h"
#include "v8-local-handle.h"
#include "v8-maybe.h"
#include "v8-object.h"
#include "v8-persistent-handle.h"
#include "v8-primitive.h"
#include "v8-promise.h"
#include "v8-script.h"
#include "v8-value.h"
#include <any>
#include <system_error>
#include <v8.h>
#include <v8-isolate.h>
#include <libplatform/libplatform.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    for (int i = 0; i < args.Length(); i++) {
        v8::HandleScope handle_scope(args.GetIsolate());
        v8::Local<v8::Value> val = args[i];
        if (!val->IsObject()) {
            v8::String::Utf8Value str(args.GetIsolate(), args[i]);
            const char* cstr = ToCString(str);
            printf("%s", cstr);
        } else {
            v8::Local<v8::Context> ctx = args.GetIsolate()->GetCurrentContext();
            v8::Local<v8::Object> obj = val->ToObject(ctx).ToLocalChecked();
            v8::Isolate *isolate = args.GetIsolate();

            v8::Local<v8::Value> output = v8::JSON::Stringify(ctx, obj, v8::String::NewFromUtf8(isolate, "  ").ToLocalChecked()).ToLocalChecked();
            v8::String::Utf8Value str(args.GetIsolate(), output);

            const char *cstr = ToCString(str);
            printf("%s", cstr);
        }

        if (args.Length() > i) printf(" ");
    }

    fflush(stdout);
}

void Println(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Print(args);
    printf("\n");
    fflush(stdout);
}

static int lastScriptId = 0;
std::map<int, Senkora::MetadataObject*> moduleMetadatas;
std::map<std::string, v8::Local<v8::Module>> moduleCache;

void run(std::string nextArg, std::any data) {
    if (nextArg.length() == 0) {
        printf("Error: missing file\n");
        return;
    }

    v8::Isolate *isolate = std::any_cast<v8::Isolate*>(data);
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, Print)); 
    global->Set(isolate, "println", v8::FunctionTemplate::New(isolate, Println));

    isolate->SetHostInitializeImportMetaObjectCallback(Senkora::Modules::metadataHook);

    v8::Local<v8::Context> ctx = v8::Context::New(isolate, nullptr, global);
    v8::Context::Scope context_scope(ctx);

    Senkora::Modules::initBuiltinModules(isolate);

    std::string currentPath = fs::current_path();
    std::string filePath = nextArg;
    if (nextArg[0] != '/') {
        filePath = fs::path(currentPath + "/" + nextArg).lexically_normal();
    }
    
    std::string code = Senkora::readFile(filePath);
    Senkora::MetadataObject *meta = new Senkora::MetadataObject();
    v8::Local<v8::Value> url = v8::String::NewFromUtf8(isolate, filePath.c_str()).ToLocalChecked();

    meta->Set(ctx, "url", url);

    v8::Local<v8::Module> mod = Senkora::compileScript(ctx, code).ToLocalChecked();

    moduleCache[filePath] = mod;
    moduleMetadatas[mod->ScriptId()] = meta;
    v8::Maybe<bool> out = mod->InstantiateModule(ctx, Senkora::Modules::moduleResolver);

    v8::MaybeLocal<v8::Value> res = mod->Evaluate(ctx);
}

void eval(std::string nextArg, std::any data) {
    if (nextArg.length() == 0) {
        printf("Error: string expression\n");
        return;
    }

    v8::Isolate *isolate = std::any_cast<v8::Isolate*>(data);
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, Print)); 

    isolate->SetHostInitializeImportMetaObjectCallback(Senkora::Modules::metadataHook);

    v8::Local<v8::Context> ctx = v8::Context::New(isolate, nullptr, global);
    v8::Context::Scope context_scope(ctx);

    Senkora::Modules::initBuiltinModules(isolate);

    std::string currentPath = fs::current_path();
    currentPath += "/eval.js";

    std::string oldSubstring = "\\n";
    std::string newSubstring = "\n";

    size_t pos = nextArg.find(oldSubstring);
    while (pos != std::string::npos) {
        nextArg.replace(pos, oldSubstring.length(), newSubstring);
        pos = nextArg.find(oldSubstring, pos + newSubstring.length());
    }

    Senkora::MetadataObject *meta = new Senkora::MetadataObject();
    v8::Local<v8::Value> url = v8::String::NewFromUtf8(isolate, currentPath.c_str()).ToLocalChecked();

    meta->Set(ctx, "url", url);

    v8::Local<v8::Module> mod = Senkora::compileScript(ctx, nextArg).ToLocalChecked();

    moduleCache[currentPath] = mod;
    moduleMetadatas[mod->ScriptId()] = meta;
    v8::Maybe<bool> out = mod->InstantiateModule(ctx, Senkora::Modules::moduleResolver);

    v8::MaybeLocal<v8::Value> res = mod->Evaluate(ctx);
}

void ArgHandler::printHelp() {
    printf(R"(Senkora - the JavaScript runtime for the modern age

Usage: senkora [OPTIONS] [ARGS]

OPTIONS:
  help, -h,           Display this help message
  version, -v         Display version
  eval <EXPR>         Evaluate string expression, returned value will be printed to stdout
  run <SCRIPT>        Execute <SCRIPT> file
)");
}

int main(int argc, char* argv[]) {
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();

    v8::Isolate* isolate = v8::Isolate::New(create_params);

    ArgHandler argHandler(argc, argv);
    argHandler.onArg("run", run, isolate);
    argHandler.onArg("eval", eval, isolate);
    argHandler.run();

    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
    return 0;
}
