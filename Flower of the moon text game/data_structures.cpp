#include "data_structures.h" // Обязательно для связи с объявлениями
#include <iostream>           // Для std::cout и т.д.
#include <iomanip>            // Для std::setprecision, std::fixed
#include <algorithm>          // Для std::find
#include <memory>             // Для std::make_unique (если используется в findBattleActionDetails)
#include "domain/Book.h"       // Нужно для доступа к content

// --- PlayerStats ---
PlayerStats::PlayerStats() = default;

void PlayerStats::DisplayStats(const domain::ItemManager& item_manager) const {
    std::cout << "\n=== СТАТИСТИКА ИГРОКА ===" << std::endl;
    std::cout << "Здоровье: " << hp << "/" << max_hp << std::endl;
    if (magic_access) {
        std::cout << "Магический резерв: " << magic_reserve << "/" << max_magic_reserve << std::endl;
    }
    std::cout << "Боевой опыт: " << combat_experience << std::endl;
    std::cout << "Выносливость: " << stamina << std::endl;
    std::cout << "Ум: " << intelligence << std::endl;
    std::cout << "\nИнвентарь:" << std::endl;
    if (inventory.empty()) {
        std::cout << "Пусто" << std::endl;
    }
    else {
        for (const auto& item_id : inventory) {
            const auto* item = item_manager.GetItem(item_id);
            if (item) {
                std::cout << "- " << item->name << std::endl;
            }
        }
    }
    std::cout << "\nИзвестные заклинания:" << std::endl;
    if (known_spells.empty()) {
        std::cout << "Нет" << std::endl;
    }
    else {
        for (const auto& spell : known_spells) {
            std::cout << "- " << spell << std::endl;
        }
    }
    std::cout << "=========================" << std::endl;
}

void PlayerStats::ResetForNewAttempt(const PlayerStats& initial_config) {
    hp = initial_config.max_hp;
    max_hp = initial_config.max_hp;
    magic_reserve = initial_config.max_magic_reserve;
    max_magic_reserve = initial_config.max_magic_reserve;
    rifle_uses_per_battle = 0;
}

bool PlayerStats::HasItem(const std::string& item_name) const {
    return std::find(inventory.begin(), inventory.end(), item_name) != inventory.end();
}

bool PlayerStats::KnowsSpell(const std::string& spell_name) const {
    return std::find(known_spells.begin(), known_spells.end(), spell_name) != known_spells.end();
}

MonsterData::MonsterData() = default;

void MonsterData::DisplayInfo(const domain::Player& player, const domain::BookManager& book_manager) const {
    std::cout << "\n--- Противник: " << name << " ---" << std::endl;
    bool book_was_read_by_player = false;
    auto book_it = player.books_actually_read.find(book_key);
    if (book_it != player.books_actually_read.end() && book_it->second) {
        book_was_read_by_player = true;
    }

    if (book_was_read_by_player) {
        const auto* book = book_manager.GetBook(book_key);
        if (book) {
            std::cout << book->content << std::endl;
        }
        std::cout << "Здоровье: " << current_hp << "/" << initial_hp << std::endl;
    }
    else {
        std::cout << "Информация о монстре скрыта (прочитайте соответствующую книгу)." << std::endl;
    }
    std::cout << "--------------------------" << std::endl;
}

double MonsterData::GetActionCoefficient(const std::string& action_name) const {
    if (std::find(favorable_actions.begin(), favorable_actions.end(), action_name) != favorable_actions.end()) {
        return 2.0;
    }
    if (std::find(unfavorable_actions.begin(), unfavorable_actions.end(), action_name) != unfavorable_actions.end()) {
        return 0.5;
    }
    if (std::find(neutral_actions.begin(), neutral_actions.end(), action_name) != neutral_actions.end()) {
        return 1.0;
    }
    if (special_rule == "AllOthersUnfavorable" &&
        std::find(favorable_actions.begin(), favorable_actions.end(), action_name) == favorable_actions.end() &&
        std::find(neutral_actions.begin(), neutral_actions.end(), action_name) == neutral_actions.end()) {
        return 0.5;
    }
    return 1.0;
}

void MonsterData::ResetHp() {
    current_hp = initial_hp;
}

ItemData::ItemData() = default;

SpellData::SpellData() = default;

PreparationActionData::PreparationActionData() = default;

const ItemData* GameData::FindItem(const std::string& item_id) const {
    for (const auto& item : items) {
        if (item.id == item_id) {
            return &item;
        }
    }
    return nullptr;
}

const SpellData* GameData::FindSpell(const std::string& name) const {
    for (const auto& spell : spells) {
        if (spell.name == name) return &spell;
    }
    return nullptr;
}

std::unique_ptr<GameData::BattleActionInfo> GameData::FindBattleActionDetails(const PlayerStats& player, const std::string& action_display_name) const {
    for (const auto& item_id : player.inventory) {
        const ItemData* item = FindItem(item_id);
        if (item && item->type == "weapon" && item->battle_action_name == action_display_name) {
            auto info = std::make_unique<BattleActionInfo>();
            info->name = item->battle_action_name;
            info->check_stat = item->check_stat;
            info->damage = item->damage;
            info->mana_cost = 0;
            info->is_instant_win = false;
            info->is_spell = false;
            info->is_weapon = true;
            info->weapon_uses_total = item->uses_per_battle;
            info->original_item_name = item->name;
            return info;
        }
    }
    for (const auto& spell_name : player.known_spells) {
        const SpellData* battle_spell_def = nullptr;
        for (const auto& s_def : this->spells) {
            if (s_def.name == spell_name && s_def.type == "battle_spell" && s_def.battle_action_name == action_display_name) {
                battle_spell_def = &s_def;
                break;
            }
        }
        if (battle_spell_def) {
            auto info = std::make_unique<BattleActionInfo>();
            info->name = battle_spell_def->battle_action_name;
            info->check_stat = battle_spell_def->check_stat;
            info->damage = battle_spell_def->damage;
            info->mana_cost = battle_spell_def->mana_cost;
            info->is_instant_win = battle_spell_def->instant_win;
            info->is_spell = true;
            info->is_weapon = false;
            info->weapon_uses_total = 0;
            return info;
        }
    }
    if (action_display_name == "удар рукой") {
        auto info = std::make_unique<BattleActionInfo>();
        info->name = "удар рукой"; info->check_stat = "combat_experience"; info->damage = 0;
        info->is_instant_win = false; info->is_spell = false; info->is_weapon = false; info->weapon_uses_total = 0;
        return info;
    }
    if (action_display_name == "бегство") {
        auto info = std::make_unique<BattleActionInfo>();
        info->name = "бегство"; info->check_stat = "stamina"; info->damage = 0;
        info->is_instant_win = true; info->is_spell = false; info->is_weapon = false; info->weapon_uses_total = 0;
        return info;
    }
    if (action_display_name == "маскировка") {
        auto info = std::make_unique<BattleActionInfo>();
        info->name = "маскировка"; info->check_stat = "intelligence"; info->damage = 0;
        info->is_instant_win = true; info->is_spell = false; info->is_weapon = false; info->weapon_uses_total = 0;
        return info;
    }
    return nullptr;
}