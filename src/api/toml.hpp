#ifndef SENKORA_TOML_AAPI
#define SENKORA_TOML_API

#include "Senkora.hpp"
#include <toml.h>
#include <string>
#include <vector>
#include <map>

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
                int i;
                float f;
                bool b;
                std::vector<TomlNode*> a;
                std::map<std::string, TomlNode*> t;
                toml_timestamp_t d;
            } value;
    };

    // don't use these
    TomlNode *handleTable(toml_table_t *table);
    TomlNode *handleArray(toml_array_t *arr);
    TomlNode *handleRaw(const char *raw);

    // only these
    TomlNode *parse(std::string toml);
    void printTomlNode(TomlNode *node, int indent = 0);
}

#endif