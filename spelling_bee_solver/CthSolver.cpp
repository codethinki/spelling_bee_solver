#include "CthSolver.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <omp.h>
#include <span>
#include <unordered_map>
#include <vector>


namespace cth {
using namespace std;


inline vector<string> loadWordlistBlock(const uint32_t block_size, ifstream& file, const char wordlist_delimiter) {
    vector<string> words{};
    words.reserve(block_size);

    string line;
    for(uint32_t i = 0; i < block_size && getline(file, line, wordlist_delimiter); i++) words.push_back(line);

    return words;
}

vector<string> solve(const string_view chars) {
    uint32_t solutionCount = 0;
    vector<string> solutions{};

    array<bool, 26> map{};
    for(auto& c : chars) map[c - 'a'] = true;
#pragma omp parallel shared(solutionCount, map)
    {
        vector<uint32_t> solutionsPart{};
        vector<string> words; //TEMP load part of the wordlist


        for(uint32_t i = 0; i < words.size(); i++) {
            for(const char c : words[i])
                if(!map[c - 'a']) goto skip;

            solutionsPart.push_back(i);
        skip:;
        }
#pragma omp atomic write
        solutionCount += solutionsPart.size();

#pragma omp single
        solutions.reserve(solutionCount);

#pragma omp ordered threads
        for(const uint32_t index : solutionsPart) solutions.push_back(words[index]);
    }
    return solutions;
}

#ifdef PREPARE_WORD_LIST
inline uint32_t checkWordBlockChunk(const uint32_t first_block_index, span<string> block_chunk, span<uint32_t> result_span) {
    int results = 0;
    for(uint32_t i = 0; i < block_chunk.size(); i++) {
        string& word = block_chunk[i];

        unordered_map<char, uint32_t> charCount{};
        int upper = 0;

        if(word.size() < MIN_WORD_SIZE || word.size() > MAX_WORD_SIZE) continue;

        for(char& c : word) {
            if(c > 'z' || (c < 'a' && (c < 'A' || c > 'Z'))) goto skip;
            if(c >= 'A' && c <= 'Z') {
                if(++upper > 1) goto skip;
                c += 'a' - 'A';
            }

            if(++charCount[c] > 4) goto skip;

            if(charCount.size() > 7) goto skip;
        }

        result_span[results++] = first_block_index + i;
    skip:;
    }
    return results;
}

inline vector<string> loadPreparedWords() {
    static constexpr uint32_t CHUNK_SIZE = 30000;

    ifstream file(WORDLIST_PATH.data());

    if(!file.is_open()) {
        cerr << "ERROR: couldn't open a file\n";
        cerr << " files:\n";
        auto printFile = [](const string_view path) { cerr << "  - \"" << path << "\"\n"; };

        printFile(WORDLIST_PATH);

        exit(EXIT_FAILURE);
    }

    vector<string> validWords{};

    while(!file.eof()) {
        vector<string> wordBlock;
        vector<uint32_t> results;
        vector<uint32_t> validIndices;


#pragma omp parallel shared(validIndices, results, wordBlock)
        {
            static const uint32_t CHUNKS = omp_get_num_threads();
            static const uint32_t BLOCK_SIZE = CHUNKS * CHUNK_SIZE;
#pragma omp single
            {
                results.resize(CHUNKS);
                validIndices.resize(BLOCK_SIZE);
                wordBlock = loadWordlistBlock(BLOCK_SIZE, file, CUSTOM_WORDLIST_DELIMITER);
            }

#pragma omp for
            for(uint32_t i = 0; i < CHUNKS; i++) {
                const uint32_t index = CHUNK_SIZE * i;
                if(index >= wordBlock.size()) continue;

                const uint32_t chunkSize = index + CHUNK_SIZE > wordBlock.size() ? wordBlock.size() - index : CHUNK_SIZE;
                const span<string> blockChunk{&wordBlock[index], chunkSize};
                const span<uint32_t> validIndicesChunk{&validIndices[index], chunkSize};

                results[i] = checkWordBlockChunk(index, blockChunk, validIndicesChunk);
            }
            size_t first;

#pragma omp single
            {
                first = validWords.size();
                validWords.resize(validWords.size() + std::accumulate(results.begin(), results.end(), 0u));
            }
#pragma omp for
            for(uint32_t i = 0; i < CHUNKS; i++) {
                const uint32_t batchFirst = first + accumulate(results.begin(), results.begin() + i, 0u);
                for(uint32_t k = 0; k < results[i]; k++)
                    validWords[batchFirst + k] = wordBlock[validIndices[CHUNK_SIZE * i + k]];
            }
        }
    }
    file.close();

    return validWords;
}

void prepareWordList() {
    const vector<string> words = loadPreparedWords();

    array<vector<uint32_t>, MAX_WORD_SIZE - MIN_WORD_SIZE> wordLengths{};
    for(uint32_t i = 0; i < words.size(); i++) { wordLengths[words[i].size() - MIN_WORD_SIZE].push_back(i); }
#pragma omp parallel for shared(wordLengths, words)
    for(auto& wordLength : wordLengths)
        ranges::sort(wordLength, [&words](const uint32_t a, const uint32_t b) { return words[a] < words[b]; });

    ofstream file(PREPARED_WORDLIST_PATH.data(), ios::ate);
    for(int i = wordLengths.size() - 1; i >= 0; --i)
        for(const uint32_t index : wordLengths[i]) file << words[index] << WORDLIST_DELIMITER;

    filesystem::copy(WORDLIST_PATH, PREPARED_WORDLIST_PATH, filesystem::copy_options::update_existing);
}
#endif

}
