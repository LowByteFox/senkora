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
#include "cli.hpp"

void ArgHandler::onArg(const std::string& arg, const std::function<void(std::string data, std::any extraData)>& func, std::any data) {
    this->funcs[arg] = func;
    this->extraDatas[arg] = data;
}

void ArgHandler::run() {
    if (this->argc == 1) {
        this->printHelp();
        return;
    }

    for (int i = 1; i < this->argc; i++) {
        if (this->funcs.contains(this->argv[i])) {
            if (i + 1 != this->argc) {
                this->funcs[this->argv[i]](this->argv[i + 1], this->extraDatas[this->argv[i]]);
            } else this->funcs[this->argv[i]]("", this->extraDatas[this->argv[i]]);
        }
    }
}
