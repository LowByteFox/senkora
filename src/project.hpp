#ifndef SENKORA_PROJECT
#define SENKORA_PROJECT

#include <Senkora.hpp>
#include <toml.hpp>
#include <memory>

namespace project {
    std::unique_ptr<Senkora::TOML::TomlNode> parseProjectConfig(const char* path);
}

#endif