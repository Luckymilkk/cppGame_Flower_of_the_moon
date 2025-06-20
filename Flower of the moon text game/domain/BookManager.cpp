#include "BookManager.h"

namespace domain {

void BookManager::AddBook(const Book& book) {
    books_[book.id] = book;
}

const Book* BookManager::GetBook(const std::string& id) const {
    auto it = books_.find(id);
    if (it != books_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Book> BookManager::GetAllBooks() const {
    std::vector<Book> result;
    for (const auto& pair : books_) {
        result.push_back(pair.second);
    }
    return result;
}

} // namespace domain 