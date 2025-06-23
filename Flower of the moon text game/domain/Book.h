#pragma once
#include <string>

namespace domain {

struct Book {
    std::string id;
    std::string title;
    std::string content;
    
    void read();
};

} // namespace domain 