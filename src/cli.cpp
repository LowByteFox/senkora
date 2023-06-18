#include "cli.hpp"

void ArgHandler::onArg(std::string arg, const std::function<void(std::string data, std::any extraData)>& func, std::any data) {
    this->funcs[arg] = func;
    this->extraDatas[arg] = data;
}

void ArgHandler::run() {
    if (this->argc == 1) {
        this->printHelp();
        return;
    }

    for (int i = 1; i < this->argc; i++) {
        if (this->funcs.find(this->argv[i]) != this->funcs.end()) {
            if (i + 1 != this->argc) {
                this->funcs[this->argv[i]](this->argv[i + 1], this->extraDatas[this->argv[i]]);
            } else this->funcs[this->argv[i]]("", this->extraDatas[this->argv[i]]);
        }
    }
}
