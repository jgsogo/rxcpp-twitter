
#include <vector>
#include <string>

namespace rx::utils {
    enum class Split {
        KeepDelimiter,
        RemoveDelimiter,
        OnlyDelimiter
    };

    std::vector<std::string> split(std::string s, std::string d, Split m = Split::KeepDelimiter);
}