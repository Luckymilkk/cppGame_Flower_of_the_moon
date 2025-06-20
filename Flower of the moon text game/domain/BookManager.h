#pragma once
#include "Book.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace domain {

class BookManager {
public:
    void addBook(const Book& book);
    const Book* getBook(const std::string& id) const;
    std::vector<Book> getAllBooks() const;
private:
    std::unordered_map<std::string, Book> books;
};

} // namespace domain 