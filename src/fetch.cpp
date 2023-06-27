#include "fetch.hpp"

#include "Senkora.hpp"
#include <v8.h>

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <iomanip>
#include <iterator>
#include <memory>
#include <type_traits>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace httpF {
    // Helper function to write data received from HTTP response
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }

    // Custom struct to store fetch options
    struct FetchOptions {
        std::string url;
        std::string method;
        std::string body;
        std::string response;
        long status;
    };

    // Perform the HTTP request natively
    bool PerformHTTPRequest(const std::string& url, const std::string& method, const std::string& body, std::string& response, long& status) {
        // Parse the URL to extract the hostname and path
        std::string hostname, path;
        size_t protocolPos = url.find("://");
        if (protocolPos != std::string::npos) {
            protocolPos += 3;
            size_t pathPos = url.find('/', protocolPos);
            if (pathPos == std::string::npos) {
                hostname = url.substr(protocolPos);
                path = "/";
            } else {
                hostname = url.substr(protocolPos, pathPos - protocolPos);
                path = url.substr(pathPos);
            }
        } else {
            return false;  // Invalid URL
        }

        // Resolve the hostname to an IP address
        struct addrinfo hints, *res = nullptr;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(hostname.c_str(), "http", &hints, &res) != 0) {
            return false;  // Failed to resolve hostname
        }

        // Create a socket
        int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd == -1) {
            freeaddrinfo(res);
            return false;  // Failed to create socket
        }

        // Connect to the server
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
            freeaddrinfo(res);
            close(sockfd);
            return false;  // Failed to connect to server
        }

        freeaddrinfo(res);

        // Construct the HTTP request
        std::stringstream requestStream;
        requestStream << method << " " << path << " HTTP/1.1\r\n";
        requestStream << "Host: " << hostname << "\r\n";
        requestStream << "Content-Length: " << body.length() << "\r\n";
        requestStream << "Connection: close\r\n";
        requestStream << "Content-Type: application/json\r\n";
        requestStream << "\r\n";
        requestStream << body;

        std::string request = requestStream.str();

        // Send the request
        if (send(sockfd, request.c_str(), request.length(), 0) == -1) {
            close(sockfd);
            return false;  // Failed to send request
        }

        // Receive the response
        constexpr size_t bufferSize = 4096;
        char buffer[bufferSize];
        std::stringstream responseStream;
        ssize_t bytesRead;

        while ((bytesRead = recv(sockfd, buffer, bufferSize - 1, 0)) > 0) {
            buffer[bytesRead] = '\0';
            responseStream << buffer;
        }

        // Close the socket
        close(sockfd);

        response = responseStream.str();

        // Parse the response status
        std::istringstream responseLineStream(response);
        std::string httpVersion;
        responseLineStream >> httpVersion;
        responseLineStream >> status;

        // Check if the response was successful
        return (status >= 200 && status < 300);
    }
}

namespace fetch {
    // TODO(?): Make it async
    void fetch(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate *isolate = args.GetIsolate();
        v8::Isolate::Scope isolateScope(isolate);
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
        v8::Context::Scope contextScope(ctx);

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
        if (args.Length() > 1 && args[1]->IsString()) {
            method = *v8::String::Utf8Value(isolate, args[1]->ToString(ctx).ToLocalChecked());
        }

        std::string body;
        if (args.Length() > 2 && args[2]->IsString()) {
            body = *v8::String::Utf8Value(isolate, args[2]->ToString(ctx).ToLocalChecked());
        }

        std::string response;
        long status = 0;
        bool success = httpF::PerformHTTPRequest(url, method, body, response, status);

        v8::Local<v8::Object> result = v8::Object::New(isolate);
        v8::Maybe<bool> _ = result->Set(ctx, v8::String::NewFromUtf8(isolate, "status").ToLocalChecked(), v8::Integer::New(isolate, status));

        if (success) {
            _ = result->Set(ctx, v8::String::NewFromUtf8(isolate, "ok").ToLocalChecked(), v8::Boolean::New(isolate, true));
            std::string responseBody = response.substr(response.find("\r\n\r\n") + 4);
            _ = result->Set(ctx, v8::String::NewFromUtf8(isolate, "text").ToLocalChecked(), v8::String::NewFromUtf8(isolate, responseBody.c_str()).ToLocalChecked());
        } else {
            _ = result->Set(ctx, v8::String::NewFromUtf8(isolate, "ok").ToLocalChecked(), v8::Boolean::New(isolate, false));
            _ = result->Set(ctx, v8::String::NewFromUtf8(isolate, "error").ToLocalChecked(), v8::String::NewFromUtf8(isolate, "Failed to perform the HTTP request").ToLocalChecked());
        }

        args.GetReturnValue().Set(result);
    }
}
