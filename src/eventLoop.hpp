#ifndef EVENTLOOP
#define EVENTLOOP

#include "v8-isolate.h"
#include "v8-object.h"
#include "v8-persistent-handle.h"
#include "v8-value.h"
#include <any>
#include <cstdint>
#include <v8-local-handle.h>
#include <v8.h>
#include <event.h>
#include <vector>

namespace events {
    typedef struct {
        FoxEventQueue *immediate;
        FoxEventQueue *rest;
    } EventLoop;

    typedef struct {
        v8::Persistent<v8::Value> callback;
        v8::Persistent<v8::Object> global;
        v8::Handle<v8::Function> func;
        v8::Isolate *isolate;
    } EventLoopData;

    EventLoop *Init();

    void Run(EventLoop *loop);
    void Free(EventLoop *loop);
    void Add(EventLoop *loop, FoxEvent *event);
    void AddImmediate(EventLoop *loop, FoxEvent *event);
    void Remove(EventLoop *loop, FoxEvent *event);
    void RemoveImmediate(EventLoop *loop, FoxEvent *event);
    bool HasEvents(EventLoop *loop);

    void setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    void setInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    void setImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
    void clearImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
    void clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    void clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args);

    uint64_t getTimeInMs();

    void executionFunc(void *args);
}

#endif
