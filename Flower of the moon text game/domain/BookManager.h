#ifndef DOMAIN_BOOK_MANAGER_H_
#define DOMAIN_BOOK_MANAGER_H_

#include "Book.h"
#include <unordered_map>
#include <string>
#include <vector>

namespace domain {

class BookManager {
public:
    void AddBook(const Book& book);
    const Book* GetBook(const std::string& id) const;
    std::vector<Book> GetAllBooks() const;
private:
    std::unordered_map<std::string, Book> books_;
};

} // namespace domain 

#endif  // DOMAIN_BOOK_MANAGER_H_ 