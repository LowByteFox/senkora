#include <jsapi.h>

// See 'boilerplate.cpp' for documentation.

namespace boilerplate {

    extern const JSClassOps DefaultGlobalClassOps;

    JSObject* CreateGlobal(JSContext* cx);

    void ReportAndClearException(JSContext* cx);

    bool Run(bool (*task)(JSContext* ctx, int argc, const char **argv), bool initSelfHosting, int argc, const char **argv);
}  // namespace boilerplate
