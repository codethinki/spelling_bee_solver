#include <algorithm>
#include <chrono>
#include <iostream>
#include <print>

#include "cth_solver.hpp"
#include "cth_variables.hpp"

#include "cth/cth_windows.hpp"

#include "cth/cth_log.hpp"

namespace cth {
using namespace std;

char getWordlistDelimiter() {
    msgln("please input the delimiter of " + string(WORDLIST_PATH) + " [\"none\" -> \'\\n\']", log::LOG_COLOR_HINT);


    string input;
    cin >> input;
    while(input.size() > 1 && input != "none") {
        log::msg("invalid, only single char delimiters allowed\ntry again: ", log::LOG_COLOR_ERROR);
        cin >> input;
    }

    if(input == "none") return '\n';
    return input[0];
}



void prepare() {

    msgln("no prepared-wordlist detected", log::LOG_COLOR_WARN);

    if(!filesystem::exists(WORDLIST_PATH.data())) {
        msgln("no wordlist detected, please place a wordlist at " + string(WORDLIST_PATH), log::LOG_COLOR_ERROR);
        system("pause");
        exit(EXIT_FAILURE);
    }
    const char delimiter = getWordlistDelimiter();


    msgln("\n\npreparing...", log::LOG_COLOR_PASSED);
    const auto start = std::chrono::high_resolution_clock::now();
    prepareWordList(delimiter);
    const auto end = std::chrono::high_resolution_clock::now();
    msgln("prepared in " + to_string(std::chrono::duration<float>(end - start).count()) + "\n\n", log::LOG_COLOR_PASSED);
}


char getChar(const string_view excluded) {
    char c = 0;

    while(c < 'a' || c > 'z') {
        if(c != 0) log::msgln("invalid, try again: ", log::LOG_COLOR_ERROR);

        string input;
        cin >> input;

        if(input.size() != 1) continue;
        if(excluded.contains(c)) continue;
        c = input[0];
    }

    return c;
}

string getValidChars() {
    string validChars{};
    log::msg("input the gold char: ", log::LOG_COLOR_HINT);
    validChars += getChar("");

    cout << "\n\n";

    for(int i = 0; i < 6; i++) {
        log::msg("[current: \"" + validChars + "\"] input a character: ", log::LOG_COLOR_HINT);
        validChars += getChar(validChars);
    }

    return validChars;
}

}

int main() {

    using namespace std;


    if(!filesystem::exists(cth::PREPARED_WORDLIST_PATH.data())) cth::prepare();

    const string validChars = cth::getValidChars();

    cout << "\n\n";

    msgln("solving...", cth::log::LOG_COLOR_PASSED);
    auto solutions = cth::solve(validChars);

    ranges::for_each(solutions, [](const auto& solution) { std::println("{0}, [score: {1}]", solution.first, (int) solution.second); });

    msgln("\n\nfound " + to_string(solutions.size()) + " solutions", cth::log::LOG_COLOR_PASSED);

    msgln("\n\n[input any value to leave]", cth::log::LOG_COLOR_HINT);
    string x;
    cin >> x;


    return EXIT_SUCCESS;
}
