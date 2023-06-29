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
#include "peekaboo.hpp"
#include "Senkora.hpp"
#include "v8-isolate.h"
#include "v8-local-handle.h"
#include "v8-primitive.h"
#include "v8-promise.h"
#include "v8-value.h"

#include <v8.h>

void peekaboo(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate *isolate = args.GetIsolate();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
    v8::Context::Scope ctx_scope(ctx);

    if (args.Length() != 1) {
        Senkora::throwException(ctx, "Expected only 1 argument");
        return;
    }

    if (!args[0]->IsPromise()) {
        Senkora::throwException(ctx, "Expected argument 1 to be Promise");
        return;
    }

    v8::Local<v8::Promise> prom = v8::Local<v8::Promise>::Cast(args[0]);

    if (prom->State() != v8::Promise::PromiseState::kFulfilled) {
        Senkora::throwException(ctx, "Promise is not Fulfilled");
        return;
    } else {
        args.GetReturnValue().Set(prom->Result());
    }
}
