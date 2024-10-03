#include <algorithm>
#include <chrono>
#include <iostream>
#include <print>

#include "cth_solver.hpp"
#include "cth_variables.hpp"


#include <filesystem>


namespace cth {
using namespace std;

char getWordlistDelimiter() {
    std::println("please input the delimiter of {} [\"none\" -> \'\\n\']", WORDLIST_PATH);


    string input;
    cin >> input;
    while(input.size() > 1 && input != "none") {
        std::println("invalid, only single char delimiters allowed\ntry again: ");
        cin >> input;
    }

    if(input == "none") return '\n';
    return input[0];
}



void prepare() {

    std::println("no prepared-wordlist detected");

    if(!filesystem::exists(WORDLIST_PATH.data())) {
        std::println("no wordlist detected, please place a wordlist at {}", WORDLIST_PATH);
        system("pause");
        exit(EXIT_FAILURE);
    }
    const char delimiter = getWordlistDelimiter();


    std::println("\n\npreparing...");
    const auto start = std::chrono::high_resolution_clock::now();
    prepareWordList(delimiter);
    const auto end = std::chrono::high_resolution_clock::now();
    std::println("prepared in {}\n\n", std::chrono::duration<float>(end - start).count());
}


char getChar(const string_view excluded) {
    char c = 0;

    while(c < 'a' || c > 'z') {
        if(c != 0) std::println("invalid, try again: ");

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
    std::println("input the gold char: ");
    validChars += getChar("");

    cout << "\n\n";

    for(int i = 0; i < 6; i++) {
        std::println("[current: \" {} \"] input a character: ", validChars);
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

    std::println("solving...");
    auto solutions = cth::solve(validChars);

    ranges::for_each(solutions, [](const auto& solution) { std::println("{0}, [score: {1}]", solution.first, (int) solution.second); });

    std::println("\n\nfound {} solutions", solutions.size());

    std::println("\n\n[input any value to leave]");
    string x;
    cin >> x;


    return EXIT_SUCCESS;
}
