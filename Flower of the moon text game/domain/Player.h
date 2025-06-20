#ifndef DOMAIN_PLAYER_H_
#define DOMAIN_PLAYER_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include "ItemManager.h"

namespace domain {

class Player {
public:
    Player();

    std::string name;
    int health;
    int max_health;
    int mana;
    int magic_reserve;
    int max_magic_reserve;
    int experience;
    int level;
    int steps;
    int inventory_slots;
    int attempts;
    bool magic_access;

    int agility;
    int accuracy;
    int stamina;
    int intelligence;
    int combat_experience;
    int magic_control;
    int magic_experience;
    int psychic_stability;

    std::unordered_map<std::string, int> stats;
    std::vector<std::string> inventory;
    std::vector<std::string> known_spells;
    std::map<std::string, bool> books_actually_read;
    int rifle_uses_per_battle;

    void AddItem(const std::string& item_id);
    void RemoveItem(const std::string& item_id);
    void TakeDamage(int amount);
    void Heal(int amount);
    void DisplayStats(const ItemManager& item_manager) const;
    void ResetForNewAttempt(const Player& initial_config);
    bool HasItem(const std::string& item_id) const;
    bool KnowsSpell(const std::string& spell_name) const;
    int GetStatValue(const std::string& stat_name) const;
    void ModifyStat(const std::string& stat_name, int value, int max_value);
    int GetUsedInventorySlots(const ItemManager& item_manager) const;
};

}  // namespace domain

#endif  // DOMAIN_PLAYER_H_ 