#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

int main() {
#if defined(_WIN32) || defined(_WIN64)
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    std::locale::global(std::locale("ru_RU.UTF-8"));

    std::ifstream file("test.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
    return 0;
} 