#include "cth_solver.hpp"

#include "cth_variables.hpp"

#include "cth/cth_log.hpp"

#include <algorithm>
#include <array>
#include <execution>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <string>
#include <vector>
#include <print>

#include <omp.h>


namespace cth {
using namespace std;

vector<pair<string, char>> solve(const string_view valid_chars) {
    ifstream file(PREPARED_WORDLIST_PATH.data());
    CTH_STABLE_ASSERT(file.is_open() && "solve: unable to open file");

    vector<pair<string, char>> solutions{};


    array<bool, 26> validArr{};
    ranges::for_each(valid_chars, [&validArr](const char c) { validArr[c - 'a'] = true; });

    string word;
    char score;
    while(solutions.size() < SOLUTIONS && getline(file, word, WORDLIST_DELIMITER) && file.get(score)) {
        if(!word.contains(valid_chars[0]) ||
            !ranges::all_of(word, [validArr](const char c) { return validArr[c - 'a']; }))
            continue;

        solutions.emplace_back(word, score);
    }


    return solutions;
}

[[nodiscard]] inline char calcWordScore(const char word_size, const char unique_chars) {
    if(word_size == 4) return 1;
    return word_size + (unique_chars == 7 ? 7 : 0);
}

[[nodiscard]] vector<pair<string, char>> loadWordlistChunk(const int64_t byte_size, const int64_t offset, const char delimiter) {
    ifstream file(WORDLIST_PATH.data());
    CTH_STABLE_ASSERT(file.is_open() && "loadWordlistChunk: failed to open file");
    const streampos end = offset + byte_size;

    if(offset != 0) {
        file.seekg(offset - 1, ios::beg);
        file.ignore(numeric_limits<streamsize>::max(), delimiter);
        if(file.eof()) return {};
    }

    vector<pair<string, char>> chunk{};
    string word;
    while(getline(file, word, delimiter) && file.tellg() < end) {
        if(word.size() < MIN_WORD_SIZE || word.size() > MAX_WORD_SIZE) continue;


        array<char, 26> charCount{};
        auto maxCharCount = static_cast<char>(static_cast<float>(word.size()) * 0.8f); //filter weird words like "aaaa" or "aaab"

        if(!ranges::all_of(word, [&charCount, maxCharCount](char& c) {
            if(c > 'z' || (c < 'a' && (c < 'A' || c > 'Z'))) return false;
            if(c >= 'A' && c <= 'Z') c += 'a' - 'A';


            if(++charCount[c - 'a'] > maxCharCount) return false;

            return true;
        }))
            continue;

        const size_t uniqueChars = ranges::count_if(charCount, [](const char count) { return count > 0; });
        if(7 < uniqueChars) continue;


        chunk.emplace_back(word, calcWordScore(static_cast<char>(word.size()), uniqueChars));
    }

    file.close();

    return chunk;
}

void prepareWordList(const char delimiter) {
    const size_t size = filesystem::file_size(WORDLIST_PATH.data());
    const size_t chunks = omp_get_max_threads();
    const size_t chunkSize = size / chunks;

    vector<pair<string, char>> tempWordList{};

#pragma omp parallel for shared(tempWordList)
    for(uint32_t i = 0; i < chunks; i++) {
        auto chunk = loadWordlistChunk(chunkSize, i * chunkSize, delimiter);
#pragma omp critical
        tempWordList.insert(tempWordList.end(), chunk.begin(), chunk.end());
    }

    sort(execution::par, tempWordList.begin(), tempWordList.end(), [](const auto& a, const auto& b) {
        return a.second > b.second || (a.second == b.second && a.first < b.first);
    });

    log::msgln("found " + to_string(tempWordList.size()) + " valid words", log::LOG_COLOR_HINT);

    ofstream file(PREPARED_WORDLIST_PATH.data(), ios::ate);
    CTH_STABLE_ASSERT(file.is_open() && "prepareWordList: failed to open file");

    ranges::for_each(tempWordList, [&file](const auto& pair) { file << pair.first << WORDLIST_DELIMITER << pair.second; });



    file.close();
}


}
