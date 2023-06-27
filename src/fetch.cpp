#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include "fetch.hpp"

#include "Senkora.hpp"
#include <v8.h>

namespace httpF {
    size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output) {
        size_t totalSize = size * nmemb;
        output->append(static_cast<char *>(contents), totalSize);
        return totalSize;
    }

    struct FetchOptions
    {
        std::string url;
        std::string method;
        std::string body;
        std::string response;
        long status;
    };

    size_t CurlWriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        FetchOptions *options = static_cast<FetchOptions *>(userp);
        return WriteCallback(contents, size, nmemb, &(options->response));
    }

    bool PerformHTTPRequest(const std::string &url, const std::string &method, const std::string &body, std::string &response, long &status)
    {
        CURL *curl = curl_easy_init();
        if (!curl)
        {
            return false;
        }

        FetchOptions options;
        options.url = url;
        options.method = method;
        options.body = body;

        curl_easy_setopt(curl, CURLOPT_URL, options.url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, options.method.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &options);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, options.body.c_str());

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &options.status);

        curl_easy_cleanup(curl);

        response = options.response;
        status = options.status;

        return true;
    }
}

namespace fetch {
    // TODO: Not use curl
    // TODO(?): Make it async
    void fetch(const v8::FunctionCallbackInfo<v8::Value> &args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        if (args.Length() < 1) {
            Senkora::throwException(ctx, "fetch requires at least 1 argument");
            return;
        }

        if (!args[0]->IsString()) {
            Senkora::throwException(ctx, "fetch requires a string URL as the first argument");
            return;
        }

        std::string url = *v8::String::Utf8Value(isolate, args[0]->ToString(ctx).ToLocalChecked());

        std::string method = "GET";
        if (args.Length() > 1 && args[1]->IsString())
        {
            method = *v8::String::Utf8Value(isolate, args[1]->ToString(ctx).ToLocalChecked());
        }

        std::string body;
        if (args.Length() > 2 && args[2]->IsString())
        {
            body = *v8::String::Utf8Value(isolate, args[2]->ToString(ctx).ToLocalChecked());
        }

        std::string response;
        long status = 0;
        bool success = httpF::PerformHTTPRequest(url, method, body, response, status);

        v8::Local<v8::Object> result = v8::Object::New(isolate);
        result->Set(ctx, v8::String::NewFromUtf8(isolate, "status").ToLocalChecked(), v8::Integer::New(isolate, status));

        if (success)
        {
            result->Set(ctx, v8::String::NewFromUtf8(isolate, "ok").ToLocalChecked(), v8::Boolean::New(isolate, true));
            result->Set(ctx, v8::String::NewFromUtf8(isolate, "text").ToLocalChecked(), v8::String::NewFromUtf8(isolate, response.c_str()).ToLocalChecked());
        }
        else
        {
            result->Set(ctx, v8::String::NewFromUtf8(isolate, "ok").ToLocalChecked(), v8::Boolean::New(isolate, false));
            result->Set(ctx, v8::String::NewFromUtf8(isolate, "error").ToLocalChecked(), v8::String::NewFromUtf8(isolate, "Failed to perform the HTTP request").ToLocalChecked());
        }

        args.GetReturnValue().Set(result);
    }
}