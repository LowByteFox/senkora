#ifndef CLI
#define CLI

#include <any>
#include <functional>
#include <map>
#include <string>

class ArgHandler {
    private:
        int argc;
        char **argv;
        void printHelp() const;
        std::map<std::string_view, std::function<void(std::string data, std::any extraData)>> funcs;
        std::map<std::string_view, std::any> extraDatas;

    public:
        ArgHandler(int argc, char **argv): argc(argc), argv(argv) {}

        void onArg(const std::string& arg, const std::function<void(std::string data, std::any extraData)>& func, std::any data = nullptr);

        void run();
};

#endif
