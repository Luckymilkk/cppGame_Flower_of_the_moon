#include "gtest/gtest.h"
#include "../domain/BookManager.h"

TEST(BookManagerTest, AddAndGetBook) {
    domain::BookManager manager;
    domain::Book book;
    book.id = "b1";
    book.title = "Тайна";
    manager.AddBook(book);
    const domain::Book* found = manager.GetBook("b1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->title, "Тайна");
    EXPECT_EQ(manager.GetBook("notfound"), nullptr);
}

TEST(BookManagerTest, GetAllBooks) {
    domain::BookManager manager;
    domain::Book b1; b1.id = "a"; b1.title = "A";
    domain::Book b2; b2.id = "b"; b2.title = "B";
    manager.AddBook(b1);
    manager.AddBook(b2);
    auto all = manager.GetAllBooks();
    EXPECT_EQ(all.size(), 2);
} 