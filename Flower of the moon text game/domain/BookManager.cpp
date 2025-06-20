#include "BookManager.h"

namespace domain {

void BookManager::addBook(const Book& book) {
    books[book.id] = book;
}

const Book* BookManager::getBook(const std::string& id) const {
    auto it = books.find(id);
    if (it != books.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Book> BookManager::getAllBooks() const {
    std::vector<Book> result;
    for (const auto& pair : books) {
        result.push_back(pair.second);
    }
    return result;
}

} // namespace domain 