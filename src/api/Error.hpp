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
#ifndef SENKORA_ERROR
#define SENKORA_ERROR

#include <v8.h>
#include "v8-container.h"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-script.h"
#include "v8-template.h"
#include "v8-value.h"

namespace Senkora::Error {
    v8::Local<v8::Array> returnError(v8::Isolate *isolate, v8::Local<v8::Value> val, std::string msg);
    v8::Local<v8::FunctionTemplate> makeError(v8::Isolate *isolate);

}

#endif
