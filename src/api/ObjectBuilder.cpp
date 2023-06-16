#include "ObjectBuilder.hpp"
#include "Senkora.hpp"
#include <v8.h>

namespace Senkora::Object {
    ObjectBuilder::ObjectBuilder(v8::Isolate *isolate) {
        this->obj = v8::ObjectTemplate::New(isolate);
        this->isolate = isolate;
        this->keys = v8::Array::New(isolate);
    }

    v8::Local<v8::Object> ObjectBuilder::Assemble(v8::Local<v8::Context> ctx) {
        v8::Context::Scope context_scope(ctx);
        return this->obj->NewInstance(ctx).ToLocalChecked();
    }

    void ObjectBuilder::Dissasemble(v8::Local<v8::Object> obj) {
        this->isDissasembled = true;
        this->dissasembly = obj;
    }

    void ObjectBuilder::Set(const char* name, v8::Local<v8::Value> value) {
        v8::Isolate *isolate = this->isolate;
        v8::Isolate::Scope isolate_scope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope context_scope(ctx);

        if (!this->isDissasembled) {
            this->obj->Set(isolate, name, value);
            this->keys->Set(ctx, this->keys->Length(), v8::String::NewFromUtf8(isolate, name).ToLocalChecked()).FromJust();
        }
        else {
            v8::Local<v8::String> key = v8::String::NewFromUtf8(isolate, name).ToLocalChecked();
            this->dissasembly->Set(ctx, key, value).FromJust();
        }
    }

    void ObjectBuilder::Set(const char* name, v8::Local<v8::FunctionTemplate> value) {
        v8::Isolate *isolate = this->isolate;
        v8::Isolate::Scope isolate_scope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope context_scope(ctx);

        if (!this->isDissasembled) {
            this->obj->Set(isolate, name, value);
            this->keys->Set(ctx, this->keys->Length(), v8::String::NewFromUtf8(isolate, name).ToLocalChecked()).FromJust();
        }
    }

    void ObjectBuilder::Set(const char* name, v8::Local<v8::Object> value) {
        v8::Isolate *isolate = this->isolate;
        v8::Isolate::Scope isolate_scope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope context_scope(ctx);

        if (!this->isDissasembled) {
            this->obj->Set(isolate, name, value);
            this->keys->Set(ctx, this->keys->Length(), v8::String::NewFromUtf8(isolate, name).ToLocalChecked()).FromJust();
        } else {
            this->dissasembly->Set(ctx, v8::String::NewFromUtf8(isolate, name).ToLocalChecked(), value).FromJust();
        }
    }

    v8::Local<v8::Array> ObjectBuilder::GetKeys(v8::Local<v8::Context> ctx) {
        v8::Isolate *isolate = this->isolate;
        v8::Isolate::Scope isolate_scope(isolate);
        v8::Context::Scope context_scope(ctx);

        if (!this->isDissasembled) {
            return this->keys;
        }
        else return this->dissasembly->GetPropertyNames(ctx).ToLocalChecked();
    }
}