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
#ifndef SENKORA_TOML_API
#define SENKORA_TOML_API

#include "Senkora.hpp"
#include <toml.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace Senkora::TOML {
    enum class TomlTypes {
        TOML_STRING,
        TOML_INT,
        TOML_FLOAT,
        TOML_BOOL,
        TOML_ARRAY,
        TOML_TABLE,
        TOML_DATETIME,
        TOML_NONE
    };

    class TomlNode {
        public:
            TomlTypes type;
            std::string key;
            // C++ doesn't have unions with non-trivial members, so we have to use a struct
            struct {
                std::string s;
                int64_t i;
                double f;
                bool b;
                std::vector<std::unique_ptr<TomlNode>> a;
                std::map<std::string_view, std::unique_ptr<TomlNode>> t;
                toml_timestamp_t d;
            } value;
    };

    // don't use these
    std::unique_ptr<TomlNode> handleTable(toml_table_t *table);
    std::unique_ptr<TomlNode> handleArray(toml_array_t *arr);
    std::unique_ptr<TomlNode> handleRaw(char *raw);

    // only these
    std::unique_ptr<TomlNode> parse(char *toml);
}

#endif
