#pragma once
#include <string>
#include <string_view>
#include<vector>

namespace cth {

std::vector<std::pair<std::string, char>> solve(std::string_view valid_chars);

void prepareWordList(char delimiter);

}