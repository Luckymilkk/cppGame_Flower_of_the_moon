#include "gtest/gtest.h"
#include "../domain/MonsterManager.h"

TEST(MonsterManagerTest, AddAndGetMonster) {
    domain::MonsterManager manager;
    domain::Monster monster;
    monster.name = "Волк";
    manager.AddMonster(monster);
    const domain::Monster* found = manager.GetMonster("Волк");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Волк");
    EXPECT_EQ(manager.GetMonster("notfound"), nullptr);
}

TEST(MonsterManagerTest, GetAllMonsters) {
    domain::MonsterManager manager;
    domain::Monster m1; m1.name = "A";
    domain::Monster m2; m2.name = "B";
    manager.AddMonster(m1);
    manager.AddMonster(m2);
    auto all = manager.GetAllMonsters();
    EXPECT_EQ(all.size(), 2);
} 