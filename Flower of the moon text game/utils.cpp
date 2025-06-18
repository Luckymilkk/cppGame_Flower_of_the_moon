#include "utils.h"

std::vector<std::string> splitString(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trimString(token)); // Сразу убираем пробелы у токенов
    }
    return tokens;
}

std::string trimString(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (start == std::string::npos)
        return ""; // Строка состоит только из пробелов
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

int getIntInput(const std::string& prompt, int minVal, int maxVal) {
    int choice;
    while (true) {
        std::cout << prompt;
        std::cin >> choice;
        if (std::cin.fail() || choice < minVal || choice > maxVal) {
            std::cout << "Неверный ввод. Пожалуйста, введите число"
                << (minVal != std::numeric_limits<int>::min() ? " от " + std::to_string(minVal) : "")
                << (maxVal != std::numeric_limits<int>::max() ? " до " + std::to_string(maxVal) : "")
                << "." << std::endl;
            std::cin.clear(); // Сброс флагов ошибок ввода
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка буфера ввода
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка буфера, если введено что-то лишнее после числа
            return choice;
        }
    }
}