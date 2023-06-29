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
        std::map<std::string, std::function<void(std::string data, std::any extraData)>> funcs;
        std::map<std::string, std::any> extraDatas;
        
    public:
        ArgHandler(int argc, char **argv): argc(argc), argv(argv) {}

        void onArg(const std::string& arg, const std::function<void(std::string data, std::any extraData)>& func, std::any data = nullptr);

        void run();
};

#endif
