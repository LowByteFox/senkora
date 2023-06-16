#include "Senkora.hpp"
#include "v8-context.h"
#include "v8-debug.h"
#include "v8-exception.h"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-script.h"
#include "v8-message.h"
#include "v8-value.h"
#include <cstdio>
#include <cstring>
#include <functional>
#include <sstream>
#include <string>
#include <fstream>
#include <filesystem>

extern const char* ToCString(const v8::String::Utf8Value& value); 
extern std::map<int, Senkora::MetadataObject *> moduleMetadatas;

namespace fs = std::filesystem;

static int lastScriptId;

namespace Senkora
{
    void MetadataObject::Set(v8::Local<v8::Context> ctx, std::string key, v8::Local<v8::Value> val)
    {
        v8::Isolate *isolate = ctx->GetIsolate();

        this->meta[key] = (Metadata){
            .key = v8::String::NewFromUtf8(isolate, key.c_str()).ToLocalChecked(),
            .value = val};
    }

    v8::Local<v8::Value> MetadataObject::Get(std::string key)
    {
        Metadata meta = this->meta[key];
        return meta.value;
    }

    std::map<std::string, Metadata> MetadataObject::getMeta()
    {
        return this->meta;
    }

    std::string readFile(std::string name)
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

    v8::MaybeLocal<v8::Module> compileScript(v8::Local<v8::Context> ctx, std::string code)
    {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::ScriptOrigin origin(isolate,
                v8::Local<v8::Integer>(),
                0, 0, false, lastScriptId, v8::Local<v8::Value>(), false, false, true);
        lastScriptId++;

        v8::ScriptCompiler::Source source(v8::String::NewFromUtf8(isolate, code.c_str()).ToLocalChecked(), origin);

        return v8::ScriptCompiler::CompileModule(isolate, &source);
    }

    v8::Local<v8::Value> throwException(v8::Local<v8::Context> ctx, const char* message, ExceptionType type) {
        v8::Isolate *isolate = ctx->GetIsolate();
        v8::Local<v8::Value> err;
        v8::Local<v8::String> str = v8::String::NewFromUtf8(isolate, message).ToLocalChecked();

        switch (type) {
            case ExceptionType::RANGE:
                err = v8::Exception::RangeError(str);
                break;
            case ExceptionType::REFERENCE:
                err = v8::Exception::ReferenceError(str);
                break;
            case ExceptionType::SYNTAX:
                err = v8::Exception::SyntaxError(str);
                break;
            case ExceptionType::TYPE:
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
        const char* cstr = ToCString(str);

        if (!exception->IsNativeError()) {
            printf("Error: %s\n", cstr);
            return;
        }

        v8::Local<v8::Message> msg = v8::Exception::CreateMessage(isolate, exception);
        int scriptId = msg->GetScriptOrigin().ScriptId();
        MetadataObject *metadata = moduleMetadatas[scriptId];

        int line = msg->GetLineNumber(ctx).FromJust();
        int col = msg->GetStartColumn(ctx).FromJust();

        printf("-------------------------------------\n");
        if (!metadata) {
            printf("%s\n", cstr);
        } else {
            v8::Local<v8::Value> filename = metadata->Get("url");
            v8::String::Utf8Value filenameStr(isolate, filename);
            fs::path path = fs::path(ToCString(filenameStr));
            printf("[%s:%d:%d] %s\n", path.filename().c_str(), line, col, cstr);
        }

        v8::Local<v8::StackTrace> stack = msg->GetStackTrace();
        int length = stack->GetFrameCount();

        for (int i = 0; i < length; i++) {
            v8::Local<v8::StackFrame> frame = stack->GetFrame(isolate, i);
            v8::Local<v8::String> funcName = frame->GetFunctionName();
            int id = frame->GetScriptId();
            MetadataObject *metadata = moduleMetadatas[id];
            if (metadata) {
                v8::Local<v8::Value> filename = metadata->Get("url");
                v8::String::Utf8Value filenameStr(isolate, filename);
                fs::path path = fs::path(ToCString(filenameStr));
                if (funcName.IsEmpty()) {
                    printf("    at %s [%d:%d]\n", path.filename().c_str(), frame->GetLineNumber(), frame->GetColumn());
                } else {
                    v8::String::Utf8Value funcNameStr(isolate, funcName);
                    printf("    at %s() [%s:%d:%d]\n", ToCString(funcNameStr), path.filename().c_str(), frame->GetLineNumber(), frame->GetColumn());
                }
            } else {
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
        int width = strlen(buff);

        for (int i = startLine; i < line; i++) {
            printf("%*d |%s\n", width, i + 1, lines[i].c_str());
        }
        printf("%*c |", width, ' ');
        for (int i = 0; i < col; i++) {
            putchar(' ');
        }
        for (int i = col; i < msg->GetEndColumn(); i++) {
            putchar('^');
        }
        putchar('\n');

        printf("=====================================\n");
    }

    void throwAndPrintException(v8::Local<v8::Context> ctx, const char* message, ExceptionType type) {
        v8::Local<v8::Value> exc = throwException(ctx, message, type);
        printException(ctx, exc);
    }
}
