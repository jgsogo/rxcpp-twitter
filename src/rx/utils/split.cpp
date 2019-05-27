
#include "rx/utils/split.h"

#include <regex>

namespace rx::utils {

    std::vector<std::string> split(std::string s, std::string d, Split m) {
        std::regex delim(d);
        std::cregex_token_iterator cursor(&s[0], &s[0] + s.size(), delim, m == Split::KeepDelimiter ? std::initializer_list<int>({-1, 0}) : (m == Split::RemoveDelimiter ? std::initializer_list<int>({-1}) : std::initializer_list<int>({0})));
        std::cregex_token_iterator end;
        std::vector<std::string> splits(cursor, end);
        return splits;
    }

}