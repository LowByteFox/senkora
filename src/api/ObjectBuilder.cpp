#include "ObjectBuilder.hpp"
#include "Senkora.hpp"
#include <v8.h>

namespace Senkora::Object {
    ObjectBuilder::ObjectBuilder(v8::Isolate *isolation): obj(v8::ObjectTemplate::New(isolation)), isolate(isolation), keys(v8::Array::New(isolation)) {}

    v8::Local<v8::Object> ObjectBuilder::Assemble(v8::Local<v8::Context> ctx) {
        v8::Context::Scope context_scope(ctx);
        return this->obj->NewInstance(ctx).ToLocalChecked();
    }

    void ObjectBuilder::Dissasemble(v8::Local<v8::Object> object) {
        this->isDissasembled = true;
        this->dissasembly = object;    
    }

    void ObjectBuilder::Set(const char* name, v8::Local<v8::Value> value) {
        v8::Isolate *isolation = this->isolate;
        v8::Isolate::Scope isolate_scope(isolation);
        v8::Local<v8::Context> ctx = isolation->GetCurrentContext();
        v8::Context::Scope context_scope(ctx);

        if (!this->isDissasembled) {
            this->obj->Set(isolation, name, value);
            this->keys->Set(ctx, this->keys->Length(), v8::String::NewFromUtf8(isolation, name).ToLocalChecked()).FromJust();
        }
        else {
            v8::Local<v8::String> key = v8::String::NewFromUtf8(isolation, name).ToLocalChecked();
            this->dissasembly->Set(ctx, key, value).FromJust();
        }
    }

    void ObjectBuilder::Set(const char* name, v8::Local<v8::FunctionTemplate> value) {
        v8::Isolate *isolation = this->isolate;
        v8::Isolate::Scope isolate_scope(isolation);
        v8::Local<v8::Context> ctx = isolation->GetCurrentContext();
        v8::Context::Scope context_scope(ctx);

        if (!this->isDissasembled) {
            this->obj->Set(isolation, name, value);
            this->keys->Set(ctx, this->keys->Length(), v8::String::NewFromUtf8(isolation, name).ToLocalChecked()).FromJust();
        } else {
            Senkora::throwAndPrintException(ctx, "v8::Object doesn't accept v8::FunctionTemplate", Senkora::ExceptionType::TYPE);
            exit(1);
        }
    }

    void ObjectBuilder::Set(const char* name, v8::Local<v8::Object> value) {
        v8::Isolate *isolation = this->isolate;
        v8::Isolate::Scope isolate_scope(isolation);
        v8::Local<v8::Context> ctx = isolation->GetCurrentContext();
        v8::Context::Scope context_scope(ctx);

        if (!this->isDissasembled) {
            this->obj->Set(isolation, name, v8::Local<v8::Value>::Cast(value));
            this->keys->Set(ctx, this->keys->Length(), v8::String::NewFromUtf8(isolation, name).ToLocalChecked()).FromJust();
        } else {
            this->dissasembly->Set(ctx, v8::String::NewFromUtf8(isolation, name).ToLocalChecked(), value).FromJust();
        }
    }

    void ObjectBuilder::Set(const char* name, v8::Local<v8::ObjectTemplate> value) {
        v8::Isolate *isolation = this->isolate;
        v8::Isolate::Scope isolate_scope(isolation);
        v8::Local<v8::Context> ctx = isolation->GetCurrentContext();
        v8::Context::Scope context_scope(ctx);

        if (!this->isDissasembled) {
            this->obj->Set(isolation, name, value);
            this->keys->Set(ctx, this->keys->Length(), v8::String::NewFromUtf8(isolation, name).ToLocalChecked()).FromJust();
        } else {
            this->dissasembly->Set(ctx, v8::String::NewFromUtf8(isolation, name).ToLocalChecked(), value->NewInstance(ctx).ToLocalChecked()).FromJust();
        }
    }

    v8::Local<v8::Value> ObjectBuilder::Get(const char* name) {
        v8::Isolate *isolation = this->isolate;
        v8::Isolate::Scope isolate_scope(isolation);
        v8::Local<v8::Context> ctx = isolation->GetCurrentContext();
        v8::Context::Scope context_scope(ctx);

        if (!this->isDissasembled) {
            return v8::Undefined(isolation);
        }
        else return this->dissasembly->Get(ctx, v8::String::NewFromUtf8(isolation, name).ToLocalChecked()).ToLocalChecked();
    }

    v8::Local<v8::Array> ObjectBuilder::GetKeys(v8::Local<v8::Context> ctx) {
        v8::Isolate *isolation = this->isolate;
        v8::Isolate::Scope isolate_scope(isolation);
        v8::Context::Scope context_scope(ctx);

        if (!this->isDissasembled) {
            return this->keys;
        }
        else return this->dissasembly->GetPropertyNames(ctx).ToLocalChecked();
    }
}