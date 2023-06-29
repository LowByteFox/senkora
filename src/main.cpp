/*
Senkora - JS runtime for the modern age
Copyright (C) 2023  SenkoraJS

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <Senkora.hpp>
#include <ObjectBuilder.hpp>
#include "event.hpp"
#include "globalThis.hpp"
#include "peekaboo.hpp"

#include "cli.hpp"
#include "eventLoop.hpp"
#include "project.hpp"
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
#include "v8-template.h"
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
#include <unistd.h>


namespace fs = std::filesystem;
using Senkora::Object::ObjectBuilder;
using Senkora::TOML::TomlTypes;

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
        if (v8::Local<v8::Value> val = args[i]; !val->IsObject()) {
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
const std::unique_ptr<Senkora::TOML::TomlNode> projectConfig = project::parseProjectConfig("project.toml");

void createProject(const fs::path& projectName, [[maybe_unused]] std::any data) {
    if (strlen(projectName.c_str()) == 0) {
        printf("Error: missing project name\n");
        return;
    }

    if (fs::exists(projectName)) {
        printf("Error: project already exists\n");
        return;
    }

    fs::create_directory(projectName);
    int out = chdir(projectName.c_str());
    if (out) {
        printf("Error: failed to change directory\n");
        return;
    }

    printf("Creating project...\n");

    std::string configFile = "[project]\nname = \"";
    configFile += projectName;
    configFile += "\"\n";
    // description
    std::string description = Senkora::userin("Type the project description: ");
    configFile += "description = \"";
    configFile += description;
    configFile += "\"\n";

    // main file
    configFile += "main = \"index.js\"\n";

    // author name from computer username
    std::string author = getenv("USER");
    configFile += "authors = [\"";
    configFile += author;
    configFile += "\"]\n";

    // dependencies
    configFile += "\n[dependencies]\n";
    configFile += "# project dependencies (work in progress)\n";

    // senkora config
    configFile += "\n[senkora]\n";
    configFile += "# senkora config (work in progress)\n";

    Senkora::writeFile("project.toml", configFile.c_str());
    Senkora::writeFile("index.js", "println(\"Kon kon Senkora!\");");

    printf("Project created!\n");
    printf("To get started:\n  cd %s\n  senkora run index.js\n", projectName.c_str());

    out = chdir("..");
    if (out) {
        printf("Error: failed to change directory\n");
        return;
    }
}

void handleProjectConfig(std::string& nextArg, Senkora::TOML::TomlNode* const& project) {
    if (project->type == TomlTypes::TOML_TABLE && project->value.t.find("main") != project->value.t.end()) {
        const auto& main = project->value.t["main"];
        if (main->type == TomlTypes::TOML_STRING) {
            nextArg = main->value.s;
        }
    }
}

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
    senkoraObj->Set(isolate, "peekaboo", v8::FunctionTemplate::New(isolate, peekaboo));
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
    if (!code.length()) {
        Senkora::throwAndPrintException(ctx, "Error: file not found", Senkora::ExceptionType::REFERENCE);
        exit(1);
    }
    auto meta = std::make_unique<Senkora::MetadataObject>();
    v8::Local<v8::Value> url = v8::String::NewFromUtf8(isolate, filePath.c_str()).ToLocalChecked();

    meta->Set(ctx, "url", url);

    v8::MaybeLocal<v8::Module> maybeMod = Senkora::compileScript(ctx, code);
    if (maybeMod.IsEmpty()) {
        exit(1);
    }
    v8::Local<v8::Module> mod = maybeMod.ToLocalChecked();

    globals.moduleCache[filePath.c_str()] = mod;
    globals.moduleMetadatas[mod->ScriptId()] = std::move(meta);

    {
        v8::Isolate::Scope isolate_scope(isolate);
        v8::Context::Scope ctx_scope(ctx);
        v8::HandleScope handle_scope(isolate);

        v8::TryCatch tryCatch(isolate);
        if (v8::Maybe<bool> out = mod->InstantiateModule(ctx, Senkora::Modules::moduleResolver); out.IsNothing()) {
            if (v8::Module::kUninstantiated == mod->GetStatus()) {
                Senkora::printException(ctx, tryCatch.Exception());
                exit(1);
            }
        }

        if (v8::MaybeLocal<v8::Value> res = mod->Evaluate(ctx); mod->GetStatus() == v8::Module::kErrored && !res.IsEmpty()) {
            if (v8::Module::kErrored == mod->GetStatus()) {
                Senkora::printException(ctx, mod->GetException());
                exit(1);
            }
        }

        events::Run(globals.globalLoop.get());
    }
}

void runDot(std::string nextArg, std::any args) {
    if (projectConfig.get()->type != TomlTypes::TOML_NONE) {
        const auto& projectConf = projectConfig.get();
        if (projectConf->type == TomlTypes::TOML_TABLE && projectConf->value.t.find("project") != projectConf->value.t.end()) {
            const auto& project = projectConf->value.t["project"];
            handleProjectConfig(nextArg, project.get());
        }
    }

    run(nextArg, args);
}

void printVersion([[maybe_unused]] std::string nextArg, [[maybe_unused]] std::any args) {
    printf(R"(Senkora Copyright (C) 2023  SenkoraJS
This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
This is free software, and you are welcome to redistribute it
under certain conditions; type `show c' for details.
)");
    printf("0.0.1\n");
}

void ArgHandler::printHelp() const {
    printf(R"(Senkora - the JavaScript runtime for the modern age

Usage: senkora [OPTIONS] [ARGS]

OPTIONS:
  help, -h,           Display this help message
  version, -v         Display version
  run <SCRIPT>        Execute <SCRIPT> file
  create <NAME>       Create a new project with the name <NAME>
)");
}

void printHelp([[maybe_unused]] std::string nextArg, [[maybe_unused]] std::any args) {
    printf(R"(Senkora - the JavaScript runtime for the modern age

Usage: senkora [OPTIONS] [ARGS]

OPTIONS:
  help, -h,           Display this help message
  version, -v         Display version
  run <SCRIPT>        Execute <SCRIPT> file
  create <NAME>       Create a new project with the name <NAME>
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

    std::vector<std::any> args{isolate, false};

    ArgHandler argHandler(argc, argv);
    argHandler.onArg("help", printHelp, args);
    argHandler.onArg("-h", printHelp, args);
    argHandler.onArg("version", printVersion, args);
    argHandler.onArg("-v", printVersion, args);
    argHandler.onArg(".", runDot, isolate);
    argHandler.onArg("run", run, isolate);
    argHandler.onArg("create", createProject, nullptr);
    argHandler.run();

    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    return 0;
}
