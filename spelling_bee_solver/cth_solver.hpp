#pragma once
#include <string_view>
#include<vector>

namespace cth {
using namespace std;

vector<pair<string, char>> solve(string_view valid_chars);

void prepareWordList(char delimiter);

}