#include "gtest/gtest.h"
#include "../domain/Player.h"
#include "../domain/ItemManager.h"
#include <sstream>
#include <iostream>
#include <cassert>

TEST(PlayerTest, AddAndRemoveItem) {
    domain::Player player;
    player.addItem("sword");
    EXPECT_TRUE(player.hasItem("sword"));
    player.removeItem("sword");
    EXPECT_FALSE(player.hasItem("sword"));
}

TEST(PlayerTest, TakeAndHealDamage) {
    domain::Player player;
    player.health = 10;
    player.takeDamage(3);
    EXPECT_EQ(player.health, 7);
    player.heal(5);
    EXPECT_EQ(player.health, 12);
}

TEST(PlayerTest, KnowsSpell) {
    domain::Player player;
    player.knownSpells.push_back("fireball");
    EXPECT_TRUE(player.knowsSpell("fireball"));
    EXPECT_FALSE(player.knowsSpell("icebolt"));
}

TEST(PlayerTest, ResetForNewAttempt) {
    domain::Player initial;
    initial.health = 20;
    initial.mana = 5;
    domain::Player player;
    player.health = 1;
    player.mana = 0;
    player.resetForNewAttempt(initial);
    EXPECT_EQ(player.health, 20);
    EXPECT_EQ(player.mana, 5);
}

TEST(PlayerTest, DisplayStats) {
    domain::Player player;
    player.health = 10;
    player.inventory.push_back("sword");
    player.knownSpells.push_back("fireball");
    player.booksActuallyRead["book1"] = true;
    testing::internal::CaptureStdout();
    player.displayStats();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Здоровье: 10"), std::string::npos);
    EXPECT_NE(output.find("sword"), std::string::npos);
    EXPECT_NE(output.find("fireball"), std::string::npos);
    EXPECT_NE(output.find("book1"), std::string::npos);
}

void testPlayerBasics() {
    domain::Player player;
    domain::ItemManager itemManager;
    
    // Тест начальных значений
    assert(player.health == 100);
    assert(player.maxHealth == 100);
    assert(player.inventory.empty());
    
    // Тест добавления предмета
    player.addItem("test_item");
    assert(player.inventory.size() == 1);
    assert(player.hasItem("test_item"));
    
    // Тест удаления предмета
    player.removeItem("test_item");
    assert(player.inventory.empty());
    assert(!player.hasItem("test_item"));
    
    // Тест получения урона
    player.takeDamage(30);
    assert(player.health == 70);
    
    // Тест лечения
    player.heal(20);
    assert(player.health == 90);
    
    // Тест сброса для новой попытки
    domain::Player initialConfig;
    initialConfig.health = 100;
    initialConfig.mana = 50;
    player.resetForNewAttempt(initialConfig);
    assert(player.health == 100);
    assert(player.mana == 50);
    
    // Тест отображения статистики (просто проверяем, что не крашится)
    player.displayStats(itemManager);
    
    std::cout << "Player tests passed!" << std::endl;
}

int main() {
    testPlayerBasics();
    return 0;
} 