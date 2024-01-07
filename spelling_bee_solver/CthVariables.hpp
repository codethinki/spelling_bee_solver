#pragma once

#include <string>

#define PREPARE_WORD_LIST


namespace cth {

using std::string_view;

//can be any path
constexpr string_view PREPARED_WORDLIST_PATH = "res/words_easy_prepared.txt";

constexpr uint32_t SOLUTIONS = 30;

constexpr char WORDLIST_DELIMITER = ',';


#ifdef PREPARE_WORD_LIST
constexpr uint32_t MAX_WORD_SIZE = 20;
constexpr uint32_t MIN_WORD_SIZE = 4;

constexpr char CUSTOM_WORDLIST_DELIMITER = '\n';
//cant be TEMP_WORDLIST_PATH
constexpr string_view WORDLIST_PATH = "res/words_easy.txt";

//cant be WORDLIST_PATH
constexpr string_view TEMP_WORDLIST_PATH = "temp.txt";
#endif

}
