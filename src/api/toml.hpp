#ifndef SENKORA_TOML_AAPI
#define SENKORA_TOML_API

#include "Senkora.hpp"
#include <toml.h>

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
}

#endif