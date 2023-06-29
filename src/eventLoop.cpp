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
#include "globalThis.hpp"
#include "v8-function.h"
#include "v8-local-handle.h"
#include "v8-object.h"
#include <cstdint>
#include <v8-exception.h>
#include <event.hpp>
#include <sys/time.h>
#include <memory>

#include "Senkora.hpp"
#include "eventLoop.hpp"
#include "v8-context.h"

extern const Senkora::SharedGlobals globals;

namespace events {
    std::unique_ptr<EventLoop> Init() {
        auto loop = std::make_unique<EventLoop>();
        loop->immediate = std::make_unique<foxevents::FoxEventQueue>();
        loop->rest = std::make_unique<foxevents::FoxEventQueue>();
        loop->data = std::vector<std::unique_ptr<EventLoopData>>();
        loop->restCache = std::vector<std::unique_ptr<foxevents::FoxEvent>>();
        loop->immediateCache = std::vector<std::unique_ptr<foxevents::FoxEvent>>();

        return loop;
    }

    void Run(EventLoop* const& loop) {
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

    void Add(EventLoop* const& loop, foxevents::FoxEvent* const& event) {
        loop->rest->add(event);
    }

    void AddImmediate(EventLoop* const& loop, foxevents::FoxEvent* const& event) {
        loop->immediate->add(event);
    }

    void Remove(EventLoop* const& loop, int id) {
        loop->rest->remove_id(id);
        size_t length = globals.globalLoop->restCache.size();
        for (int i = 0; i < length; i++) {
            if (globals.globalLoop->restCache[i]->id == id) {
                globals.globalLoop->restCache.erase(globals.globalLoop->restCache.begin() + i);
                break;
            }
        }
        size_t length2 = globals.globalLoop->data.size();
        for (int i = 0; i < length2; i++) {
            if (globals.globalLoop->data[i]->id == id) {
                globals.globalLoop->data.erase(globals.globalLoop->data.begin() + i);
                break;
            }
        }
    }

    void RemoveImmediate(EventLoop* const& loop, int id) {
        loop->immediate->remove_id(id);
        
        size_t length = globals.globalLoop->immediateCache.size();
        for (int i = 0; i < length; i++) {
            if (globals.globalLoop->immediateCache[i]->id == id) {
                globals.globalLoop->immediateCache.erase(globals.globalLoop->immediateCache.begin() + i);
                break;
            }
        }
        size_t length2 = globals.globalLoop->data.size();
        for (int i = 0; i < length2; i++) {
            if (globals.globalLoop->data[i]->id == id) {
                globals.globalLoop->data.erase(globals.globalLoop->data.begin() + i);
                break;
            }
        }
    }

    bool HasEvents(EventLoop* const& loop) {
        return !loop->immediate->empty() || !loop->rest->empty();
    }

    uint64_t getTimeInMs() {
        struct timeval tv;
        gettimeofday(&tv, nullptr);

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

        auto funcArgs = std::make_unique<EventLoopData>();
        funcArgs->callback.Reset(isolate, callback);
        funcArgs->global.Reset(isolate, args.This());
        funcArgs->isolate = isolate;
        funcArgs->id = globals.restId;

        auto event = std::make_unique<foxevents::FoxEvent>(timeout, false, executionFunc, funcArgs.get(), globals.restId);
        args.GetReturnValue().Set(v8::Integer::New(isolate, event->id));
        Add(globals.globalLoop.get(), event.get());
        globals.globalLoop->data.push_back(std::move(funcArgs));
        globals.globalLoop->restCache.push_back(std::move(event));
        globals.restId++;
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

        auto funcArgs = std::make_unique<EventLoopData>();
        funcArgs->callback.Reset(isolate, callback);
        funcArgs->global.Reset(isolate, args.This());
        funcArgs->isolate = isolate;
        funcArgs->id = globals.restId;

        auto event = std::make_unique<foxevents::FoxEvent>(0, false, executionFunc, funcArgs.get(), globals.restId);
        args.GetReturnValue().Set(v8::Integer::New(isolate, event->id));
        AddImmediate(globals.globalLoop.get(), event.get());
        globals.globalLoop->data.push_back(std::move(funcArgs));
        globals.globalLoop->immediateCache.push_back(std::move(event));

        globals.restId++;
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

        auto funcArgs = std::make_unique<EventLoopData>();
        funcArgs->callback.Reset(isolate, callback);
        funcArgs->global.Reset(isolate, args.This());
        funcArgs->isolate = isolate;
        funcArgs->id = globals.restId;

        auto event = std::make_unique<foxevents::FoxEvent>(timeout, true, executionFunc, funcArgs.get(), globals.restId);
        args.GetReturnValue().Set(v8::Integer::New(isolate, event->id));
        Add(globals.globalLoop.get(), event.get());
        globals.globalLoop->restCache.push_back(std::move(event));
        globals.globalLoop->data.push_back(std::move(funcArgs));
        globals.restId++;
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
        Remove(globals.globalLoop.get(), (int) id);
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
        Remove(globals.globalLoop.get(), (int) id);
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
        RemoveImmediate(globals.globalLoop.get(), (int) id);
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
