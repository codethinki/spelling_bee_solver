#pragma once

#include <string_view>

namespace cth {

using std::string_view;

//can be any path
constexpr string_view PREPARED_WORDLIST_PATH = "res/wordlist_prepared.txt";
constexpr string_view WORDLIST_PATH = "res/wordlist.txt";

constexpr uint32_t SOLUTIONS = 500;

constexpr char WORDLIST_DELIMITER = ',';


constexpr uint32_t MAX_WORD_SIZE = 100;
constexpr uint32_t MIN_WORD_SIZE = 4;



}
