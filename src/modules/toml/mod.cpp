#include "mod.hpp"

#include <v8.h>
#include <Senkora.hpp>
#include "../modules.hpp"
#include <memory>
#include <ObjectBuilder.hpp>
#include <toml.hpp>

using Senkora::Object::ObjectBuilder;
using Senkora::TOML::TomlTypes;
namespace tomlMod {
    v8::Local<v8::Value> toJsVal(const Senkora::TOML::TomlNode* & node) {
        switch (node->type) {
            using enum Senkora::TOML::TomlTypes;
            case TOML_STRING:
                return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), node->value.s.c_str()).ToLocalChecked();
            case TOML_INT:
                return v8::Integer::New(v8::Isolate::GetCurrent(), (int32_t) node->value.i);
            case TOML_FLOAT:
                return v8::Number::New(v8::Isolate::GetCurrent(), node->value.f);
            case TOML_BOOL:
                return v8::Boolean::New(v8::Isolate::GetCurrent(), node->value.b);
            default: 
                break;
        }

        return v8::Undefined(v8::Isolate::GetCurrent());
    }

    v8::Local<v8::Value> toJsArray(v8::Isolate *isolate, const Senkora::TOML::TomlNode* & node) {
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Local<v8::Array> arr = v8::Array::New(isolate);

        for (int i = 0; i < node->value.a.size(); i++) {
            const Senkora::TOML::TomlNode *tmpNode = node->value.a[i].get();
            switch (tmpNode->type) {
                using enum Senkora::TOML::TomlTypes;
                case TOML_ARRAY: {
                    v8::Maybe<bool> res1 = arr->Set(ctx, i, toJsArray(isolate, tmpNode));
                    break;
                }
                case TOML_TABLE: {
                    v8::Maybe<bool> res2 = arr->Set(ctx, i, toJsObject(isolate, tmpNode));
                    break;
                }
                case TOML_DATETIME:
                    break;
                default:
                    v8::Maybe<bool> res3 = arr->Set(ctx, i, toJsVal(tmpNode));
            }
        }

        return v8::Local<v8::Value>::Cast(arr);
    }

    v8::Local<v8::Object> toJsObject(v8::Isolate *isolate, const Senkora::TOML::TomlNode* & node) {
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Local<v8::Object> obj = v8::Object::New(isolate);

        for (const auto& [key, val] : node->value.t) {
            const Senkora::TOML::TomlNode *tmpNode = val.get();
            switch (tmpNode->type) {
                using enum Senkora::TOML::TomlTypes;
                case TOML_ARRAY: {
                    v8::Maybe<bool> res = obj->Set(ctx, v8::String::NewFromUtf8(isolate, key.data()).ToLocalChecked(), toJsArray(isolate, tmpNode));
                    break;
                } case TOML_TABLE: {
                    v8::Maybe<bool> res = obj->Set(ctx, v8::String::NewFromUtf8(isolate, key.data()).ToLocalChecked(), toJsObject(isolate, tmpNode));
                    break;
                }
                case TOML_DATETIME:
                    break;
                default:
                    if (tmpNode->type != TOML_NONE) {
                        v8::Maybe<bool> res = obj->Set(ctx, v8::String::NewFromUtf8(isolate, key.data()).ToLocalChecked(), toJsVal(tmpNode));
                    }
                    break;
            }
        }
        return obj;
    }

    v8::Local<v8::Object> toJsObject(v8::Isolate *isolate, Senkora::TOML::TomlNode* const& node) {
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);
        v8::Local<v8::Object> obj = v8::Object::New(isolate);
        
        for (const auto& [key, val] : node->value.t) {
            const Senkora::TOML::TomlNode *tmpNode = val.get();
            switch (tmpNode->type) {
                using enum Senkora::TOML::TomlTypes;
                case TOML_ARRAY: {
                    v8::Maybe<bool> res = obj->Set(ctx, v8::String::NewFromUtf8(isolate, key.data()).ToLocalChecked(), toJsArray(isolate, tmpNode));
                    break;
                }
                case TOML_TABLE: {
                    v8::Maybe<bool> res = obj->Set(ctx, v8::String::NewFromUtf8(isolate, key.data()).ToLocalChecked(), toJsObject(isolate, tmpNode));
                    break;
                }
                case TOML_DATETIME:
                    break;
                default:
                    if (tmpNode->type != TOML_NONE) {
                        v8::Maybe<bool> res = obj->Set(ctx, v8::String::NewFromUtf8(isolate, key.data()).ToLocalChecked(), toJsVal(tmpNode));
                    }
                    break;
            }
        }

        return obj;
    }

    void parseTOML(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

        if (args.Length() < 1) {
            Senkora::throwException(ctx, "Expected 1 argument");
            return;
        }

        if (!args[0]->IsString()) {
            Senkora::throwException(ctx, "Expected argument 1 to be a string");
            return;
        }

        v8::Local<v8::String> path = args[0]->ToString(ctx).ToLocalChecked();
        v8::String::Utf8Value pathUtf8(isolate, path);

        auto node = Senkora::TOML::parse(*pathUtf8);
        if (!node) {
            Senkora::throwException(ctx, "Failed to parse TOML file");
            return;
        }

        args.GetReturnValue().Set(toJsObject(isolate, node.get()));
    }

    std::vector<v8::Local<v8::String>> getExports(v8::Isolate *isolate) {
        std::vector<v8::Local<v8::String>> exports;

        exports.push_back(v8::String::NewFromUtf8(isolate, "parse").ToLocalChecked());
        exports.push_back(v8::String::NewFromUtf8(isolate, "default").ToLocalChecked());
        return exports;
    }

    v8::MaybeLocal<v8::Value> init(v8::Local<v8::Context> ctx, v8::Local<v8::Module> mod) {
        v8::Isolate *isolate = ctx->GetIsolate();

        v8::Local<v8::Object> default_exports = v8::Object::New(isolate);

        v8::Local<v8::String> name = v8::String::NewFromUtf8(isolate, "parse").ToLocalChecked();
        v8::Local<v8::Value> val = v8::FunctionTemplate::New(isolate, parseTOML)->GetFunction(ctx).ToLocalChecked();
        Senkora::Modules::setModuleExport(mod, ctx, default_exports, isolate, name, val);

        Senkora::Modules::setModuleExport(mod, ctx, isolate, v8::String::NewFromUtf8(isolate, "default").ToLocalChecked(), default_exports);

        return v8::Boolean::New(isolate, true);
    }
}