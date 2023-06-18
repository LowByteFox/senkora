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
#include <event.hpp>
#include <vector>
#include <memory>

namespace events {

    typedef struct {
        v8::Persistent<v8::Value> callback;
        v8::Persistent<v8::Object> global;
        v8::Isolate *isolate;
        int id;
    } EventLoopData;

    typedef struct {
        std::unique_ptr<foxevents::FoxEventQueue> immediate;
        std::unique_ptr<foxevents::FoxEventQueue> rest;
        std::vector<std::unique_ptr<foxevents::FoxEvent>> immediateCache;
        std::vector<std::unique_ptr<foxevents::FoxEvent>> restCache;
        std::vector<std::unique_ptr<EventLoopData>> data;
    } EventLoop;

    std::unique_ptr<EventLoop> Init();

    void Run(std::unique_ptr<EventLoop> loop);
    void Add(EventLoop* const& loop, std::unique_ptr<foxevents::FoxEvent> event);
    void AddImmediate(EventLoop* const& loop, std::unique_ptr<foxevents::FoxEvent> event);
    void Remove(EventLoop* const& loop, int id);
    void RemoveImmediate(EventLoop* const& loop, int id);
    bool HasEvents(EventLoop* const& loop);

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
