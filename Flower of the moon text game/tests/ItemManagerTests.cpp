#include "gtest/gtest.h"
#include "../domain/ItemManager.h"

TEST(ItemManagerTest, AddAndGetItem) {
    domain::ItemManager manager;
    domain::Item item;
    item.id = "item1";
    item.name = "Меч";
    manager.AddItem(item);
    const domain::Item* found = manager.GetItem("item1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Меч");
    EXPECT_EQ(manager.GetItem("notfound"), nullptr);
}

TEST(ItemManagerTest, GetAllItems) {
    domain::ItemManager manager;
    domain::Item item1; item1.id = "i1"; item1.name = "A";
    domain::Item item2; item2.id = "i2"; item2.name = "B";
    manager.AddItem(item1);
    manager.AddItem(item2);
    auto all = manager.GetAllItems();
    EXPECT_EQ(all.size(), 2);
} 