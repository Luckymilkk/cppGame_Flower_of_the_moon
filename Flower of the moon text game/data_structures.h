#ifndef DATA_STRUCTURES_H_
#define DATA_STRUCTURES_H_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "domain/ItemManager.h"
#include "domain/BookManager.h"
#include "domain/Player.h"
#include "domain/Action.h"


struct PlayerStats {
    int hp = 5;
    int max_hp = 5;
    int steps = 15;
    int inventory_slots = 5;
    int attempts = 0;

    int agility = 0;
    int accuracy = 0;
    int stamina = 0;
    int intelligence = 0;
    int combat_experience = 0;
    int magic_control = 0;
    int magic_experience = 0;
    int psychic_stability = 0;

    bool magic_access = false;
    int magic_reserve = 2;
    int max_magic_reserve = 2;

    std::vector<std::string> inventory;
    std::vector<std::string> known_spells;
    std::map<std::string, bool> books_actually_read;
    int rifle_uses_per_battle = 0;

    PlayerStats();
    void DisplayStats(const domain::ItemManager& item_manager) const;
    void ResetForNewAttempt(const PlayerStats& initial_config);
    bool HasItem(const std::string& item_name) const;
    bool KnowsSpell(const std::string& spell_name) const;
};

struct MonsterData {
    std::string id;
    std::string name;
    int initial_hp = 0;
    int current_hp = 0;
    int attack = 0;
    int defense = 0;
    int damage_to_player = 0;
    std::string book_key;
    std::vector<std::string> favorable_actions;
    std::vector<std::string> unfavorable_actions;
    std::vector<std::string> neutral_actions;
    std::string special_rule;

    MonsterData();
    void DisplayInfo(const domain::Player& player, const domain::BookManager& book_manager) const;
    double GetActionCoefficient(const std::string& action_name) const;
    void ResetHp();
};

struct ItemData {
    std::string id;
    std::string name;
    std::string type;
    int cost_steps = 0;
    int cost_slots = 0;
    std::string prep_effect_type;
    int prep_effect_value = 0;
    std::string battle_action_name;
    std::string check_stat;
    int damage = 0;
    int uses_per_battle = 0;
    std::string book_key;

    ItemData();
};

struct SpellData {
    std::string name;
    std::string type;
    int cost_steps = 0;
    bool requires_magic_access_to_learn = false;
    std::string battle_action_name;
    std::string check_stat;
    int damage = 0;
    int mana_cost = 0;
    bool instant_win = false;
    bool requires_magic_access_to_use = false;

    SpellData();
};

struct PreparationActionData {
    std::string menu_text;
    int cost_steps = 0;
    std::string effect_type;
    std::string target_stat;
    int effect_value = 0;
    int max_value = 0;
    bool requires_magic_access = false;
    std::string requires_book;
    std::string book_key;

    PreparationActionData();
};

struct GameData {
    PlayerStats initial_player_stats;
    std::vector<MonsterData> monsters;
    std::vector<ItemData> items;
    std::vector<SpellData> spells;
    std::map<std::string, std::string> book_contents;
    std::vector<PreparationActionData> preparation_actions;

    struct BattleActionInfo {
        std::string name;
        std::string check_stat;
        int damage = 0;
        int mana_cost = 0;
        bool is_instant_win = false;
        bool is_spell = false;
        bool is_weapon = false;
        int weapon_uses_total = 0;
        std::string original_item_name;
    };

    const ItemData* FindItem(const std::string& item_id) const;
    const SpellData* FindSpell(const std::string& name) const;
    std::unique_ptr<BattleActionInfo> FindBattleActionDetails(const PlayerStats& player, const std::string& action_display_name) const;
};

#endif 