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
        void printHelp();
        std::map<std::string, std::function<void(std::string data, std::any extraData)>> funcs;
        std::map<std::string, std::any> extraDatas;

    public:
        ArgHandler(int argc, char **argv) {
            this->argc = argc;
            this->argv = argv;
        }

        void onArg(std::string arg, std::function<void(std::string data, std::any extraData)> func, std::any data = nullptr);

        void run();
};

#endif
