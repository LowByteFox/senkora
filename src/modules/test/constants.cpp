#include <map>
#include <string>
#include "constants.hpp"

namespace testConst
{
    std::map<std::string, std::string> colors = {
        {"reset", "\033[0m"},
        {"red", "\033[31m"},
        {"green", "\033[32m"},
        {"yellow", "\033[33m"},
        {"blue", "\033[34m"},
        {"magenta", "\033[35m"},
        {"cyan", "\033[36m"},
        {"white", "\033[37m"},
        {"bright", "\033[1m"},
        {"dim", "\033[2m"},
        {"underscore", "\033[4m"},
        {"blink", "\033[5m"},
        {"reverse", "\033[7m"},
        {"hidden", "\033[8m"},
        {"gray", "\033[90m"}
    };
    
    std::map<std::string, int> testEmbedderNums = {
        {"describe", 158},
        {"error", 159},
        {"errorStr", 160}
    };

    std::string getColor(std::string code)
    {
        return colors[code];
    }

    int getTestEmbedderNum(std::string code)
    {
        return testEmbedderNums[code];
    }
}