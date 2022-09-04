#include <cctype>
#include <string>
#include <vector>
#include <algorithm>
#include "string.hpp"

std::string leapus::string::toupper(const std::string &v){
    std::vector<char> tmp;
    tmp.resize(v.size());
    std::transform(v.begin(), v.end(),tmp.begin(), ::toupper);
    return std::string{&*tmp.begin(), v.size()};
}

