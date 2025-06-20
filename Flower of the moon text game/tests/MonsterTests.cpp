#include "gtest/gtest.h"
#include "../domain/Monster.h"

TEST(MonsterTest, TakeDamageAndIsAlive) {
    domain::Monster monster;
    monster.health = 5;
    monster.takeDamage(3);
    EXPECT_EQ(monster.health, 2);
    EXPECT_TRUE(monster.isAlive());
    monster.takeDamage(2);
    EXPECT_EQ(monster.health, 0);
    EXPECT_FALSE(monster.isAlive());
    monster.takeDamage(1);
    EXPECT_EQ(monster.health, 0);
    EXPECT_FALSE(monster.isAlive());
} 