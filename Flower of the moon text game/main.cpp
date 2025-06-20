#define NOMINMAX // Для Windows, чтобы избежать конфликтов min/max макросов
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <locale>
#include <stdexcept> // Для std::exception

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h> // Для установки кодировки консоли Windows
#endif

#include "engine/GameEngine.h"

int main() {
    #if defined(_WIN32) || defined(_WIN64)
    // Устанавливаем кодовые страницы для корректного отображения UTF-8 в консоли Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif

    try {
        std::cout << "--- Добро пожаловать в игру 'Цветок Луны' ---\n" << std::endl;
        
        engine::GameEngine game;
        game.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "\nКритическая ошибка во время выполнения: " << e.what() << std::endl;
        std::cerr << "Программа будет закрыта." << std::endl;
    }

    std::cout << "\nНажмите Enter для выхода...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();

    return 0;
}



