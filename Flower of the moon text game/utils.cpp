#include "utils.h"

std::vector<std::string> SplitString(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(s);
    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(TrimString(token));
    }
    return tokens;
}

std::string TrimString(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

int GetIntInput(const std::string& prompt, int min_val, int max_val) {
    int choice;
    while (true) {
        std::cout << prompt;
        std::cin >> choice;
        if (std::cin.fail() || choice < min_val || choice > max_val) {
            std::cout << "Неверный ввод. Пожалуйста, введите число"
                << (min_val != std::numeric_limits<int>::min() ? " от " + std::to_string(min_val) : "")
                << (max_val != std::numeric_limits<int>::max() ? " до " + std::to_string(max_val) : "")
                << "." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
    }
}