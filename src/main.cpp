#include <Senkora.hpp>
#include <ObjectBuilder.hpp>
#include "event.hpp"
#include "globalThis.hpp"

#include "cli.hpp"
#include "eventLoop.hpp"
#include "modules/modules.hpp"
#include "v8-container.h"
#include "v8-context.h"
#include "v8-data.h"
#include "v8-exception.h"
#include "v8-function-callback.h"
#include "v8-function.h"
#include "v8-initialization.h"
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
#include <v8-internal.h>
#include <v8.h>
#include <v8-isolate.h>
#include <libplatform/libplatform.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <filesystem>
#include <memory>


namespace fs = std::filesystem;
using Senkora::Object::ObjectBuilder;

const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

void safeEval(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Senkora::throwException(args.GetIsolate()->GetCurrentContext(), "Error is disabled for security reasons");
}

void Print(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate::Scope isolate_scope(args.GetIsolate());
    v8::HandleScope handle_scope(args.GetIsolate());
    for (int i = 0; i < args.Length(); i++) {
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
    v8::Isolate::Scope isolate_scope(args.GetIsolate());
    v8::HandleScope handle_scope(args.GetIsolate());
    Print(args);
    printf("\n");
    fflush(stdout);
}

// console.*
void notImplementedFunc(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Senkora::throwException(args.GetIsolate()->GetCurrentContext(), "Not implemented yet", Senkora::ExceptionType::REFERENCE);
}

inline const Senkora::SharedGlobals globals;

void run(std::string nextArg, std::any data) {
    if (nextArg.length() == 0) {
        printf("Error: missing file\n");
        return;
    }

    v8::Isolate *isolate = std::any_cast<v8::Isolate*>(data);
    v8::Isolate::Scope isolate_scope(isolate);
    v8::HandleScope handle_scope(isolate);

    isolate->SetCaptureStackTraceForUncaughtExceptions(true);

    v8::Local<v8::ObjectTemplate> global = globalObject::Init(isolate);
    globalObject::AddFunction(isolate, global, "print", v8::FunctionTemplate::New(isolate, Print));
    globalObject::AddFunction(isolate, global, "println", v8::FunctionTemplate::New(isolate, Println));
    globalObject::AddFunction(isolate, global, "setTimeout", v8::FunctionTemplate::New(isolate, events::setTimeout));
    globalObject::AddFunction(isolate, global, "setImmediate", v8::FunctionTemplate::New(isolate, events::setImmediate));
    globalObject::AddFunction(isolate, global, "setInterval", v8::FunctionTemplate::New(isolate, events::setInterval));
    globalObject::AddFunction(isolate, global, "clearTimeout", v8::FunctionTemplate::New(isolate, events::clearTimeout));
    globalObject::AddFunction(isolate, global, "clearImmediate", v8::FunctionTemplate::New(isolate, events::clearImmediate));
    globalObject::AddFunction(isolate, global, "clearInterval", v8::FunctionTemplate::New(isolate, events::clearInterval));
    
    v8::Local<v8::ObjectTemplate> senkoraObj = v8::ObjectTemplate::New(isolate);
    senkoraObj->Set(isolate, "version", v8::String::NewFromUtf8(isolate, "0.0.1").ToLocalChecked());
    global->Set(isolate, "Senkora", senkoraObj);

    isolate->SetHostInitializeImportMetaObjectCallback(Senkora::Modules::metadataHook);

    v8::Local<v8::Context> ctx = v8::Context::New(isolate, nullptr, global);
    ctx->AllowCodeGenerationFromStrings(false);
    ctx->SetErrorMessageForCodeGenerationFromStrings(v8::String::NewFromUtf8(isolate, "both 'eval' and 'Function' constructor are disabled!").ToLocalChecked());
    v8::Context::Scope context_scope(ctx);

    auto glob = ObjectBuilder(isolate);
    glob.Dissasemble(ctx->Global());
    auto console = ObjectBuilder(isolate);
    const char *consoleMethods[] = {"log", "info", "warn", "error", "debug", "trace", "dir", "dirxml", "table", "count", "countReset", "assert", "profile", "profileEnd", "timeLog", "timeEnd", "group", "groupCollapsed", "groupEnd", "clear", "time", "timeStamp", "context"};
    for (int i = 0; i < 22; i++) {
        console.Set(consoleMethods[i], v8::FunctionTemplate::New(isolate, notImplementedFunc));
    }
    glob.Set("console", console.Assemble(ctx));

    Senkora::Modules::initBuiltinModules(isolate);

    std::string currentPath = fs::current_path();
    std::string filePath = nextArg;
    if (nextArg[0] != '/') {
        filePath = fs::path(currentPath + "/" + nextArg).lexically_normal();
    }
    
    std::string code = Senkora::readFile(filePath);
    auto meta = std::make_unique<Senkora::MetadataObject>();
    v8::Local<v8::Value> url = v8::String::NewFromUtf8(isolate, filePath.c_str()).ToLocalChecked();

    meta->Set(ctx, "url", url);

    v8::Local<v8::Module> mod = Senkora::compileScript(ctx, code).ToLocalChecked();

    auto scent = std::make_unique<Senkora::Scent>();
    scent->num = 5454;
    meta->setScent(scent.get());

    globals.moduleCache[filePath.c_str()] = mod;
    globals.moduleMetadatas[mod->ScriptId()] = meta.get();

    v8::Maybe<bool> out = mod->InstantiateModule(ctx, Senkora::Modules::moduleResolver);

    v8::MaybeLocal<v8::Value> res = mod->Evaluate(ctx);

    if (mod->GetStatus() == v8::Module::kErrored) {
        Senkora::printException(ctx, mod->GetException());
        exit(1);
    }

    events::Run(globals.globalLoop);
}

void ArgHandler::printHelp() const {
    printf(R"(Senkora - the JavaScript runtime for the modern age

Usage: senkora [OPTIONS] [ARGS]

OPTIONS:
  help, -h,           Display this help message
  version, -v         Display version
  run <SCRIPT>        Execute <SCRIPT> file
)");
}

int main(int argc, char* argv[]) {
    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::SetFlagsFromString("--use-strict true ");
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();

    v8::Isolate* isolate = v8::Isolate::New(create_params);

    ArgHandler argHandler(argc, argv);
    argHandler.onArg("run", run, isolate);
    argHandler.run();

    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
    return 0;
}
