#include "gtest/gtest.h"
#include "../domain/Monster.h"

TEST(MonsterTest, TakeDamageAndIsAlive) {
    domain::Monster monster;
    monster.health = 5;
    monster.TakeDamage(3);
    EXPECT_EQ(monster.health, 2);
    EXPECT_TRUE(monster.IsAlive());
    monster.TakeDamage(2);
    EXPECT_EQ(monster.health, 0);
    EXPECT_FALSE(monster.IsAlive());
    monster.TakeDamage(1);
    EXPECT_EQ(monster.health, 0);
    EXPECT_FALSE(monster.IsAlive());
} 