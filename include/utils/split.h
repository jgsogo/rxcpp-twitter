
#pragma once

#include "utils/utils_export.h"

#include <vector>
#include <string>

namespace utils {
    enum class Split {
        KeepDelimiter,
        RemoveDelimiter,
        OnlyDelimiter
    };

    UTILS_EXPORT std::vector<std::string> split(std::string s, std::string d, Split m = Split::KeepDelimiter);
}