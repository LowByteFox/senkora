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
extern "C" {
    #include "api.h"
}
#include "mod.hpp"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"

#include <v8.h>
#include <Senkora.hpp>
#include "../modules.hpp"

namespace fsMod {
    void writeToFileJS(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 2) {
            Senkora::throwException(ctx, "Expected 2 arguments");
            return;
        }

        if (!args[0]->IsString()) {
            Senkora::throwException(ctx, "Expected argument 1 to be a string");
            return;
        }

        if (!args[1]->IsString()) {
            Senkora::throwException(ctx, "Expected argument 2 to be a string");
            return;
        }

        v8::Local<v8::String> path = args[0]->ToString(ctx).ToLocalChecked();
        v8::Local<v8::String> content = args[1]->ToString(ctx).ToLocalChecked();
        
        v8::String::Utf8Value pathUtf8(isolate, path);
        v8::String::Utf8Value contentUtf8(isolate, content);

        writeToFile(*pathUtf8, *contentUtf8);

        args.GetReturnValue().Set(v8::Undefined(isolate));
    }

    void readFromFileJS(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 1) {
            Senkora::throwException(ctx, "Expected 1 argument");
            return;
        }

        if (!args[0]->IsString()) {
            Senkora::throwException(ctx, "Expected argument 1 to be a string");
            return;
        }

        v8::Local<v8::String> path = args[0]->ToString(ctx).ToLocalChecked();
        v8::String::Utf8Value pathUtf8(isolate, path);

        std::string content = readFromFile(*pathUtf8);

        args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, content.c_str()).ToLocalChecked());
    }

    void deleteFileJS(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 1)
        {
            Senkora::throwException(ctx, "Expected 1 argument");
            return;
        }

        if (!args[0]->IsString())
        {
            Senkora::throwException(ctx, "Expected argument 1 to be a string");
            return;
        }

        v8::Local<v8::String> path = args[0]->ToString(ctx).ToLocalChecked();
        v8::String::Utf8Value pathUtf8(isolate, path);

        if (deleteFile(*pathUtf8) == 0)
        {
            Senkora::throwException(ctx, "Failed to delete file");
            return;
        }

        args.GetReturnValue().Set(v8::Undefined(isolate));
    }

    void deleteDirectoryJS(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 1) {
            Senkora::throwException(ctx, "Expected 1 argument");
            return;
        }

        if (!args[0]->IsString()) {
            Senkora::throwException(ctx, "Expected argument 1 to be a string");
            return;
        }

        v8::Local<v8::String> path = args[0]->ToString(ctx).ToLocalChecked();
        v8::String::Utf8Value pathUtf8(isolate, path);

        if (deleteDirectory(*pathUtf8) == 0) {
            Senkora::throwException(ctx, "Failed to delete directory");
            return;
        }

        args.GetReturnValue().Set(v8::Undefined(isolate));
    }

    void existsFileJS(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 1) {
            Senkora::throwException(ctx, "Expected 1 argument");
            return;
        }

        if (!args[0]->IsString()) {
            Senkora::throwException(ctx, "Expected argument 1 to be a string");
            return;
        }

        v8::Local<v8::String> path = args[0]->ToString(ctx).ToLocalChecked();
        v8::String::Utf8Value pathUtf8(isolate, path);

        if (existsFile(*pathUtf8)) {
            args.GetReturnValue().Set(v8::Boolean::New(isolate, true));
        }
        else
        {
            args.GetReturnValue().Set(v8::Boolean::New(isolate, false));
        }
    }

    void existsDirectoryJS(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 1)
        {
            Senkora::throwException(ctx, "Expected 1 argument");
            return;
        }

        if (!args[0]->IsString())
        {
            Senkora::throwException(ctx, "Expected argument 1 to be a string");
            return;
        }

        v8::Local<v8::String> path = args[0]->ToString(ctx).ToLocalChecked();
        v8::String::Utf8Value pathUtf8(isolate, path);

        if (existsDirectory(*pathUtf8))
        {
            args.GetReturnValue().Set(v8::Boolean::New(isolate, true));
        }
        else
        {
            args.GetReturnValue().Set(v8::Boolean::New(isolate, false));
        }
    }

    void existsJS(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 1)
        {
            Senkora::throwException(ctx, "Expected 1 argument");
            return;
        }

        if (!args[0]->IsString())
        {
            Senkora::throwException(ctx, "Expected argument 1 to be a string");
            return;
        }

        v8::Local<v8::String> path = args[0]->ToString(ctx).ToLocalChecked();
        v8::String::Utf8Value pathUtf8(isolate, path);

        if (exists(*pathUtf8))
        {
            args.GetReturnValue().Set(v8::Boolean::New(isolate, true));
        }
        else
        {
            args.GetReturnValue().Set(v8::Boolean::New(isolate, false));
        }
    }

    void createDirectoryJS(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 1)
        {
            Senkora::throwException(ctx, "Expected 1 argument");
            return;
        }

        if (!args[0]->IsString())
        {
            Senkora::throwException(ctx, "Expected argument 1 to be a string");
            return;
        }

        v8::Local<v8::String> path = args[0]->ToString(ctx).ToLocalChecked();
        v8::String::Utf8Value pathUtf8(isolate, path);

        if (createDirectory(*pathUtf8) == 0)
        {
            Senkora::throwException(ctx, "Failed to create directory");
            return;
        }

        args.GetReturnValue().Set(v8::Undefined(isolate));
    }

    std::vector<v8::Local<v8::String>> getExports(v8::Isolate *isolate) {
        std::vector<v8::Local<v8::String>> exports;

        exports.push_back(v8::String::NewFromUtf8(isolate, "writeToFile").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "readFromFile").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "deleteFile").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "deleteDirectory").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "existsFile").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "existsDirectory").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "exists").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "createDirectory").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "default").ToLocalChecked());
        return exports;
    }

    v8::MaybeLocal<v8::Value> init(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod) {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::Local<v8::Object> default_exports = v8::Object::New(isolate);

        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, "writeToFile").ToLocalChecked();
        v8::Local<v8::Value> val = v8::FunctionTemplate::New(isolate, writeToFileJS)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        name = v8::String::NewFromUtf8(isolate, "readFromFile").ToLocalChecked();
        val = v8::FunctionTemplate::New(isolate, readFromFileJS)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        name = v8::String::NewFromUtf8(isolate, "deleteFile").ToLocalChecked();
        val = v8::FunctionTemplate::New(isolate, deleteFileJS)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        name = v8::String::NewFromUtf8(isolate, "deleteDirectory").ToLocalChecked();
        val = v8::FunctionTemplate::New(isolate, deleteDirectoryJS)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        name = v8::String::NewFromUtf8(isolate, "exists").ToLocalChecked();
        val = v8::FunctionTemplate::New(isolate, existsJS)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        name = v8::String::NewFromUtf8(isolate, "existsFile").ToLocalChecked();
        val = v8::FunctionTemplate::New(isolate, existsFileJS)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        name = v8::String::NewFromUtf8(isolate, "existsDirectory").ToLocalChecked();
        val = v8::FunctionTemplate::New(isolate, existsDirectoryJS)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        name = v8::String::NewFromUtf8(isolate, "createDirectory").ToLocalChecked();
        val = v8::FunctionTemplate::New(isolate, createDirectoryJS)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        // default module export
        Senkora::Modules::setModuleExport(mod, ctx, isolate, v8::String::NewFromUtf8(isolate, "default").ToLocalChecked(), default_exports);

        return v8::Boolean::New(isolate, true);
    }
}
