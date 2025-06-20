#pragma once
#include <string>

namespace domain {

struct Book {
    std::string id;
    std::string title;
    std::string content;
    // Методы для взаимодействия с книгой
    void read();
};

} // namespace domain 