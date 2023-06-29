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

    void Run(EventLoop* const& loop);
    void Add(EventLoop* const& loop, foxevents::FoxEvent* const& event);
    void AddImmediate(EventLoop* const& loop, foxevents::FoxEvent* const& eventt);
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
