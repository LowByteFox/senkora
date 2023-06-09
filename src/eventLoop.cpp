#include "globalThis.hpp"
#include "v8-function.h"
#include "v8-local-handle.h"
#include "v8-object.h"
#include <cstdint>
extern "C" {
    #include <event.h>
}
#include <sys/time.h>

#include "Senkora.hpp"
#include "eventLoop.hpp"
#include "v8-context.h"

events::EventLoop *globalLoop;

namespace events {
    EventLoop *Init() {
        EventLoop *loop = new EventLoop;
        loop->immediate = fox_event_queue_create();
        loop->rest = fox_event_queue_create();

        return loop;
    }

    void Run(EventLoop *loop) {
        while (HasEvents(loop)) {
            uint64_t now = getTimeInMs();
            loop->immediate->current_time = now;
            loop->rest->current_time = now;

            if (!fox_event_queue_empty(loop->immediate)) {
                fox_event_queue_run(loop->immediate);
            }
            if (!fox_event_queue_empty(loop->rest)) {
                fox_event_queue_run(loop->rest);
            }
        }
    }

    void Free(EventLoop *loop) {
        fox_event_queue_destroy(loop->immediate);
        fox_event_queue_destroy(loop->rest);
        delete loop;
    }

    void Add(EventLoop *loop, FoxEvent *event) {
        fox_event_queue_add(loop->rest, event);
    }

    void AddImmediate(EventLoop *loop, FoxEvent *event) {
        fox_event_queue_add(loop->immediate, event);
    }

    void Remove(EventLoop *loop, FoxEvent *event) {
        fox_event_queue_remove(loop->rest, event);
    }

    void RemoveImmediate(EventLoop *loop, FoxEvent *event) {
        fox_event_queue_remove(loop->immediate, event);
    }

    bool HasEvents(EventLoop *loop) {
        return !fox_event_queue_empty(loop->immediate) || !fox_event_queue_empty(loop->rest);
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
        int timeout = args[1]->IntegerValue(isolate->GetCurrentContext()).ToChecked();

        EventLoopData *funcArgs = new EventLoopData;
        funcArgs->callback.Reset(isolate, callback);
        funcArgs->global.Reset(isolate, args.This());
        funcArgs->isolate = isolate;

        FoxEvent *event = fox_event_create(getTimeInMs() + timeout, false, executionFunc, funcArgs);
        Add(globalLoop, event);
    }

    void executionFunc(void *args) {
        EventLoopData *funcArgs = (EventLoopData *) args;
        v8::Isolate *isolate = funcArgs->isolate;
        v8::Local<v8::Value> preFunc = funcArgs->callback.Get(isolate);
        v8::Local<v8::Object> global = funcArgs->global.Get(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(preFunc);
        v8::Local<v8::Value> result = func->Call(ctx, global, 0, nullptr).ToLocalChecked();
    }
}
