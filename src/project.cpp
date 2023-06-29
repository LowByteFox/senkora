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
#include <Senkora.hpp>
#include <toml.hpp>
#include <memory>

#include "project.hpp"

namespace project {
    std::unique_ptr<Senkora::TOML::TomlNode> parseProjectConfig(const char* path) {
        std::string file = Senkora::readFile(path);
        if (!file.length()) {
            auto node = std::make_unique<Senkora::TOML::TomlNode>();
            node->type = Senkora::TOML::TomlTypes::TOML_NONE;

            return node;
        }

        return Senkora::TOML::parse(file.data());
    }
}
