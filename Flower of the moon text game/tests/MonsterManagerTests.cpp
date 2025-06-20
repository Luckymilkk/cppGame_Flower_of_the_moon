#include "gtest/gtest.h"
#include "../domain/MonsterManager.h"

TEST(MonsterManagerTest, AddAndGetMonster) {
    domain::MonsterManager manager;
    domain::Monster monster;
    monster.name = "Волк";
    manager.addMonster(monster);
    const domain::Monster* found = manager.getMonster("Волк");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->name, "Волк");
    EXPECT_EQ(manager.getMonster("notfound"), nullptr);
}

TEST(MonsterManagerTest, GetAllMonsters) {
    domain::MonsterManager manager;
    domain::Monster m1; m1.name = "A";
    domain::Monster m2; m2.name = "B";
    manager.addMonster(m1);
    manager.addMonster(m2);
    auto all = manager.getAllMonsters();
    EXPECT_EQ(all.size(), 2);
} 