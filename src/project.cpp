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