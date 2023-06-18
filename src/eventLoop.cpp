#include "globalThis.hpp"
#include "v8-function.h"
#include "v8-local-handle.h"
#include "v8-object.h"
#include <cstdint>
#include <v8-exception.h>
#include <event.hpp>
#include <sys/time.h>

#include "Senkora.hpp"
#include "eventLoop.hpp"
#include "v8-context.h"

extern const Senkora::SharedGlobals globals;

namespace events {
    EventLoop *Init() {
        auto loop = new EventLoop;
        loop->immediate = new foxevents::FoxEventQueue();
        loop->rest = new foxevents::FoxEventQueue();

        return loop;
    }

    void Run(EventLoop *loop) {
        while (HasEvents(loop)) {
            uint64_t now = getTimeInMs();
            if (!loop->immediate->empty()) {
                loop->immediate->run(now);
            }
            if (!loop->rest->empty()) {
                loop->rest->run(now);
            }
        }
    }

    void Add(EventLoop *loop, foxevents::FoxEvent *event) {
        loop->rest->add(event);
    }

    void AddImmediate(EventLoop *loop, foxevents::FoxEvent *event) {
        loop->immediate->add(event);
    }

    void Remove(EventLoop *loop, int id) {
        loop->rest->remove_id(id);
    }

    void RemoveImmediate(EventLoop *loop, int id) {
        loop->immediate->remove_id(id);
    }

    bool HasEvents(EventLoop *loop) {
        return !loop->immediate->empty() || !loop->rest->empty();
    }

    uint64_t getTimeInMs() {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
    }

    void setTimeout(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (args.Length() < 2) {
            Senkora::throwException(ctx, "setTimeout requires at least 2 arguments");
        }

        if (!args[0]->IsFunction()) {
            Senkora::throwException(ctx, "setTimeout requires a function as the first argument");
        }

        if (!args[1]->IsNumber()) {
            Senkora::throwException(ctx, "setTimeout requires a number as the second argument");
        }

        v8::Handle<v8::Value> callback(args[0]);
        long timeout = args[1]->IntegerValue(isolate->GetCurrentContext()).ToChecked();

        auto funcArgs = new EventLoopData;
        funcArgs->callback.Reset(isolate, callback);
        funcArgs->global.Reset(isolate, args.This());
        funcArgs->isolate = isolate;

        auto event = new foxevents::FoxEvent(timeout, false, executionFunc, funcArgs, globals.restId);
        Add(globals.globalLoop, event);
        globals.restId++;
        args.GetReturnValue().Set(v8::Integer::New(isolate, event->id));
    }

    void setImmediate(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (args.Length() < 1) {
            Senkora::throwException(ctx, "setImmediate requires at least 1 argument");
        }

        if (!args[0]->IsFunction()) {
            Senkora::throwException(ctx, "setImmediate requires a function as the first argument");
        }

        v8::Handle<v8::Value> callback(args[0]);

        auto funcArgs = new EventLoopData;
        funcArgs->callback.Reset(isolate, callback);
        funcArgs->global.Reset(isolate, args.This());
        funcArgs->isolate = isolate;

        auto event = new foxevents::FoxEvent(0, false, executionFunc, funcArgs, globals.restId);
        AddImmediate(globals.globalLoop, event);
        globals.restId++;
        args.GetReturnValue().Set(v8::Integer::New(isolate, event->id));
    }

    void setInterval(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (args.Length() < 2) {
            Senkora::throwException(ctx, "setInterval requires at least 2 arguments");
        }

        if (!args[0]->IsFunction()) {
            Senkora::throwException(ctx, "setInterval requires a function as the first argument");
        }

        if (!args[1]->IsNumber()) {
            Senkora::throwException(ctx, "setInterval requires a number as the second argument");
        }

        v8::Handle<v8::Value> callback(args[0]);
        long timeout = args[1]->IntegerValue(isolate->GetCurrentContext()).ToChecked();

        auto funcArgs = new EventLoopData;
        funcArgs->callback.Reset(isolate, callback);
        funcArgs->global.Reset(isolate, args.This());
        funcArgs->isolate = isolate;

        auto event = new foxevents::FoxEvent(timeout, true, executionFunc, funcArgs, globals.restId);
        Add(globals.globalLoop, event);
        globals.restId++;
        args.GetReturnValue().Set(v8::Integer::New(isolate, event->id));
    }

    void clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (args.Length() < 1) {
            Senkora::throwException(ctx, "clearInterval requires at least 1 argument");
        }

        if (!args[0]->IsNumber()) {
            Senkora::throwException(ctx, "clearInterval requires a number as the first argument");
        }

        long id = args[0]->IntegerValue(isolate->GetCurrentContext()).ToChecked();
        Remove(globals.globalLoop, (int) id);
    }

    void clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (args.Length() < 1) {
            Senkora::throwException(ctx, "clearTimeout requires at least 1 argument");
        }

        if (!args[0]->IsNumber()) {
            Senkora::throwException(ctx, "clearTimeout requires a number as the first argument");
        }

        long id = args[0]->IntegerValue(isolate->GetCurrentContext()).ToChecked();
        Remove(globals.globalLoop, (int) id);
    }

    void clearImmediate(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (args.Length() < 1) {
            Senkora::throwException(ctx, "clearImmediate requires at least 1 argument");
        }

        if (!args[0]->IsNumber()) {
            Senkora::throwException(ctx, "clearImmediate requires a number as the first argument");
        }

        long id = args[0]->IntegerValue(isolate->GetCurrentContext()).ToChecked();
        RemoveImmediate(globals.globalLoop, (int) id);
    }

    void executionFunc(void *args) {
        auto funcArgs = (EventLoopData *) args;
        v8::Isolate *isolation = funcArgs->isolate;
        v8::Isolate::Scope isolateScope(isolation);
        v8::HandleScope scope(isolation);
        v8::Local<v8::Context> ctx = isolation->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        v8::Local<v8::Value> preFunc = funcArgs->callback.Get(isolation);
        v8::Local<v8::Object> global = funcArgs->global.Get(isolation);

        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(preFunc);
        v8::TryCatch tryCatch(isolation);
        v8::MaybeLocal<v8::Value> result = func->Call(ctx, global, 0, nullptr);

        if (tryCatch.HasCaught() || result.IsEmpty()) {
            Senkora::printException(ctx, tryCatch.Exception());
        }
    }
}
