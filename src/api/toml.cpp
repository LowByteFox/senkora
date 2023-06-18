#include "Senkora.hpp"
#include "toml.hpp"
#include <toml.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>

namespace Senkora::TOML {
    std::unique_ptr<TomlNode> handleRaw(const char *raw) {
        auto node = std::make_unique<TomlNode>();
        char *sval;
        int64_t ival;
        int bval;
        double dval;
        toml_timestamp_t ts;
        char dbuf[100];

        if (!toml_rtos(raw, &sval)) {
            node->type = TomlTypes::TOML_STRING;
            node->value.s = std::string(sval);
        } else if (!toml_rtoi(raw, &ival)) {
            node->type = TomlTypes::TOML_INT;
            node->value.i = ival;
        } else if (!toml_rtob(raw, &bval)) {
            node->type = TomlTypes::TOML_BOOL;
            node->value.b = bval;
        } else if (!toml_rtod_ex(raw, &dval, dbuf, sizeof(dbuf))) {
            node->type = TomlTypes::TOML_FLOAT;
            node->value.f = dval;
        } else if (!toml_rtots(raw, &ts)) {
            node->type = TomlTypes::TOML_DATETIME;
            node->value.d = ts;
        } else {
            node->type = TomlTypes::TOML_NONE;
        }

        return node;
    }
    
    std::unique_ptr<TomlNode> handleArray(const toml_array_t *arr) {
        auto node = std::make_unique<TomlNode>();
        const char *raw;
        const toml_array_t *arr2;
        const toml_table_t *tab;

        if (toml_array_kind(arr) == 't') {
            node->type = TomlTypes::TOML_ARRAY;
            for (int i = 0; (tab = toml_table_at(arr, i)); i++) {
                auto tmp = handleTable(tab);
                node->value.a.push_back(std::move(tmp));
            }
            return node;
        }

        switch (toml_array_kind(arr)) {
            case 'v':
                for (int i = 0; (raw = toml_raw_at(arr, i)); i++) {
                    node->value.a.push_back(handleRaw(raw));
                }
                break;
            case 'a':
                for (int i = 0; (arr2 = toml_array_at(arr, i)); i++) {
                    node->value.a.push_back(handleArray(arr2));
                }
                break;
            default:
                break;
        }

        return node;
    }

    std::unique_ptr<TomlNode> handleTable(const toml_table_t *table) {
        auto node = std::make_unique<TomlNode>();
        node->type = TomlTypes::TOML_TABLE;

        const char *key;
        const char *raw;
        const toml_array_t *arr;
        const toml_table_t *tab;

        for (int i = 0; (key = toml_key_in(table, i)); i++) {
            if ((raw = toml_raw_in(table, key))) {
                auto tmp = handleRaw(raw);
                tmp->key = std::string_view(key);
                node->value.t.try_emplace(key, std::move(tmp));
            } else if ((arr = toml_array_in(table, key))) {
                auto tmp = handleArray(arr);
                tmp->key = std::string_view(key);
                node->value.a.push_back(std::move(tmp));
            } else if ((tab = toml_table_in(table, key))) {
                auto tmp = handleTable(tab);
                tmp->key = std::string(key);
                node->value.t.try_emplace(key, std::move(tmp));
            }
        }

        return node;
    }

    std::unique_ptr<TomlNode> parse(char* toml) {
        const toml_table_t *tbl = toml_parse(toml, nullptr, 0);
        
        return handleTable(tbl);
    }

    void printTomlNode(TomlNode const &node, int indent) {
        switch (node.type) {
            case TomlTypes::TOML_STRING:
                printf("%s = \"%s\"\n", node.key.c_str(), node.value.s.c_str());
                break;
            case TomlTypes::TOML_INT:
                std::cout << node.value.i;
                break;
            case TomlTypes::TOML_FLOAT:
                std::cout << node.value.f;
                break;
            case TomlTypes::TOML_BOOL:
                std::cout << (node.value.b ? "true" : "false");
                break;
            case TomlTypes::TOML_TABLE:
                for (auto start = node.value.t.begin(); start != node.value.t.end(); start++) {
                    printTomlNode(*start->second.get(), indent + 2);
                }
                break;
            case TomlTypes::TOML_ARRAY:
                for (auto start = node.value.a.begin(); start != node.value.a.end(); start++) {
                    printTomlNode(*(*start).get(), indent + 2);
                }
                break;
            case TomlTypes::TOML_DATETIME:
                break;
            case TomlTypes::TOML_NONE:
                break;
        }
    }
}