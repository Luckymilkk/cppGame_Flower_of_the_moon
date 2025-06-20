#include "gtest/gtest.h"
#include "../domain/Player.h"
#include "../domain/ItemManager.h"
#include <sstream>
#include <iostream>
#include <cassert>

TEST(PlayerTest, AddAndRemoveItem) {
    domain::Player player;
    player.AddItem("sword");
    EXPECT_TRUE(player.HasItem("sword"));
    player.RemoveItem("sword");
    EXPECT_FALSE(player.HasItem("sword"));
}

TEST(PlayerTest, TakeAndHealDamage) {
    domain::Player player;
    player.health = 10;
    player.TakeDamage(3);
    EXPECT_EQ(player.health, 7);
    player.Heal(5);
    EXPECT_EQ(player.health, 12);
}

TEST(PlayerTest, KnowsSpell) {
    domain::Player player;
    player.known_spells.push_back("fireball");
    EXPECT_TRUE(player.KnowsSpell("fireball"));
    EXPECT_FALSE(player.KnowsSpell("icebolt"));
}

TEST(PlayerTest, ResetForNewAttempt) {
    domain::Player initial;
    initial.health = 20;
    initial.mana = 5;
    domain::Player player;
    player.health = 1;
    player.mana = 0;
    player.ResetForNewAttempt(initial);
    EXPECT_EQ(player.health, 20);
    EXPECT_EQ(player.mana, 5);
}

TEST(PlayerTest, DisplayStats) {
    domain::Player player;
    player.health = 10;
    player.inventory.push_back("sword");
    player.known_spells.push_back("fireball");
    player.books_actually_read["book1"] = true;
    domain::ItemManager item_manager;
    testing::internal::CaptureStdout();
    player.DisplayStats(item_manager);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_NE(output.find("Здоровье: 10"), std::string::npos);
    EXPECT_NE(output.find("sword"), std::string::npos);
    EXPECT_NE(output.find("fireball"), std::string::npos);
    EXPECT_NE(output.find("book1"), std::string::npos);
}

void TestPlayerBasics() {
    domain::Player player;
    domain::ItemManager item_manager;
    assert(player.health == 100);
    assert(player.max_health == 100);
    assert(player.inventory.empty());
    player.AddItem("test_item");
    assert(player.inventory.size() == 1);
    assert(player.HasItem("test_item"));
    player.RemoveItem("test_item");
    assert(player.inventory.empty());
    assert(!player.HasItem("test_item"));
    player.TakeDamage(30);
    assert(player.health == 70);
    player.Heal(20);
    assert(player.health == 90);
    domain::Player initial_config;
    initial_config.health = 100;
    initial_config.mana = 50;
    player.ResetForNewAttempt(initial_config);
    assert(player.health == 100);
    assert(player.mana == 50);
    player.DisplayStats(item_manager);
    std::cout << "Player tests passed!" << std::endl;
}

int main() {
    TestPlayerBasics();
    return 0;
} 