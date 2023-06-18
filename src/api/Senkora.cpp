#include "Senkora.hpp"
#include <v8.h>
#include "v8-context.h"
#include "v8-debug.h"
#include "v8-exception.h"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-script.h"
#include "v8-message.h"
#include "v8-value.h"
#include <ObjectBuilder.hpp>
#include <cstdio>
#include <cstring>
#include <functional>
#include <sstream>
#include <string>
#include <fstream>
#include <filesystem>

extern const Senkora::SharedGlobals globals;

using Senkora::Object::ObjectBuilder;
namespace fs = std::filesystem;

namespace Senkora
{
    void MetadataObject::Set(v8::Local<v8::Context> ctx, const char *key, v8::Local<v8::Value> val)
    {
        v8::Isolate *isolate = ctx->GetIsolate();

        this->meta[key] = (Metadata){
            .key = v8::String::NewFromUtf8(isolate, key).ToLocalChecked(),
            .value = val};
    }

    void MetadataObject::setScent(std::unique_ptr<Scent> scnt)
    {
        this->scent = std::move(scnt);
    }

    v8::Local<v8::Value> MetadataObject::Get(const std::string_view& key) {
        return this->meta[key].value;
    }

    std::map<std::string_view, Metadata>  MetadataObject::getMeta() const {
        return this->meta;
    }

    std::unique_ptr<Scent> MetadataObject::getScent()
    {
        return std::move(this->scent);
    }

    std::string readFile(const std::string& name)
    {
        std::ifstream file(name);
        if (!file.good())
            return "";

        std::stringstream ss;

        ss << file.rdbuf();

        std::string out = ss.str();

        file.close();

        return out;
    }

    v8::MaybeLocal<v8::Module> compileScript(v8::Local<v8::Context> ctx, const std::string& code)
    {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::ScriptOrigin origin(isolate,
                v8::Local<v8::Integer>(),
                0, 0, false, globals.lastScriptId, v8::Local<v8::Value>(), false, false, true);
        globals.lastScriptId++;

        v8::ScriptCompiler::Source source(v8::String::NewFromUtf8(isolate, code.c_str()).ToLocalChecked(), origin);

        return v8::ScriptCompiler::CompileModule(isolate, &source);
    }

    v8::Local<v8::Value> throwException(v8::Local<v8::Context> ctx, const char* message, ExceptionType type) {
        v8::Isolate *isolate = ctx->GetIsolate();
        v8::Local<v8::Value> err;
        v8::Local<v8::String> str = v8::String::NewFromUtf8(isolate, message).ToLocalChecked();

        switch (type) {
            using enum Senkora::ExceptionType;
            case RANGE:
                err = v8::Exception::RangeError(str);
                break;
            case REFERENCE:
                err = v8::Exception::ReferenceError(str);
                break;
            case SYNTAX:
                err = v8::Exception::SyntaxError(str);
                break;
            case TYPE:
                err = v8::Exception::TypeError(str);
                break;
            default:
                err = v8::Exception::Error(str);
                break;
        }

        isolate->ThrowException(err);

        return err;
    }

    void printException(v8::Local<v8::Context> ctx, v8::Local<v8::Value> exception) {
        v8::Isolate *isolate = ctx->GetIsolate();
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Context::Scope context_scope(ctx);

        v8::String::Utf8Value str(isolate, exception);
        const char* cstr = *str;

        if (!exception->IsNativeError()) {
            printf("Error: %s\n", cstr);
            return;
        }

        v8::Local<v8::Object> obj = exception->ToObject(ctx).ToLocalChecked();
        v8::Local<v8::Value> name = obj->Get(ctx, v8::String::NewFromUtf8(isolate, "name").ToLocalChecked()).ToLocalChecked();
        v8::String::Utf8Value nameStr(isolate, name);

        v8::Local<v8::Message> msg = v8::Exception::CreateMessage(isolate, exception);
        int scriptId = msg->GetScriptOrigin().ScriptId();
        const auto& metadata = globals.moduleMetadatas[scriptId];

        int line = msg->GetLineNumber(ctx).FromJust();
        int col = msg->GetStartColumn(ctx).FromJust();

        printf("-------------------------------------\n");
        if (!metadata) {
            printf("%s\n", cstr);
        } else {
            v8::Local<v8::Value> filename = metadata->Get("url");
            v8::String::Utf8Value filenameStr(isolate, filename);
            auto path = fs::path(*filenameStr);
            printf("[%s:%d:%d] %s\n", path.filename().c_str(), line, col, cstr);
        }

        v8::Local<v8::StackTrace> stack = msg->GetStackTrace();
        int length = stack->GetFrameCount();

        for (int i = 0; i < length; i++) {
            v8::Local<v8::StackFrame> frame = stack->GetFrame(isolate, i);
            v8::Local<v8::String> funcName = frame->GetFunctionName();
            int id = frame->GetScriptId();
            const auto& tempMetadata = globals.moduleMetadatas[id];
            if (tempMetadata) {
                v8::Local<v8::Value> filename = tempMetadata->Get("url");
                v8::String::Utf8Value filenameStr(isolate, filename);
                auto path = fs::path(*filenameStr);
                if (funcName.IsEmpty()) {
                    printf("    at %s [%d:%d]\n", path.filename().c_str(), frame->GetLineNumber(), frame->GetColumn());
                } else {
                    v8::String::Utf8Value funcNameStr(isolate, funcName);
                    printf("    at %s() [%s:%d:%d]\n", *funcNameStr, path.filename().c_str(), frame->GetLineNumber(), frame->GetColumn());
                }
            }
        }
        printf("=====================================\n");
        // code
        if (col == -1) return;
        if (!metadata) return;

        v8::MaybeLocal<v8::String> maybeSource = msg->GetSource(ctx);
        if (maybeSource.IsEmpty()) return;

        v8::Local<v8::String> source = maybeSource.ToLocalChecked();
        v8::String::Utf8Value sourceStr(isolate, source);
        int startLine = line - 5;
        if (startLine < 0) startLine = 0;

        std::vector<std::string> lines;
        std::stringstream f(*sourceStr);
        std::string lineStr;
        while (std::getline(f, lineStr, '\n')) {
            lines.push_back(lineStr);
        }

        char buff[12];
        sprintf(buff, "%d", line);
        size_t width = strlen(buff);

        for (int i = startLine; i < line; i++) {
            printf("%*d |%s\n", (int) width, i + 1, lines[i].c_str());
        }
        printf("%*c |", (int) width, ' ');
        for (int i = 0; i < col; i++) {
            putchar(' ');
        }
        for (int i = col; i < msg->GetEndColumn(); i++) {
            putchar('^');
        }
        putchar('\n');

        printf("=====================================\n");

        // if it is AggregateError, iterate and call itself with the exception
        if (!strcmp("AggregateError", *nameStr)) {
            v8::Local<v8::Value> errors = obj->Get(ctx, v8::String::NewFromUtf8(isolate, "errors").ToLocalChecked()).ToLocalChecked();
            v8::Local<v8::Array> errorsArray = v8::Local<v8::Array>::Cast(errors);
            int errorArrLength = errorsArray->Length();
            for (int i = 0; i < errorArrLength; i++) {
                v8::Local<v8::Value> error = errorsArray->Get(ctx, i).ToLocalChecked();
                printException(ctx, error);
            }
        }
    }

    void throwAndPrintException(v8::Local<v8::Context> ctx, const char* message, ExceptionType type) {
        v8::Local<v8::Value> exc = throwException(ctx, message, type);
        printException(ctx, exc);
    }
}
