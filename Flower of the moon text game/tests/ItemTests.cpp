#include "gtest/gtest.h"
#include "../domain/Item.h"

TEST(ItemTest, FieldsAndUse) {
    domain::Item item;
    item.id = "item1";
    item.name = "Меч";
    item.description = "Острый меч";
    item.effects["damage"] = 10;
    EXPECT_EQ(item.id, "item1");
    EXPECT_EQ(item.name, "Меч");
    EXPECT_EQ(item.description, "Острый меч");
    EXPECT_EQ(item.effects["damage"], 10);
    // Use() пока не реализует логику, просто вызывается
    item.Use();
} 