#include "Player.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace domain {

Player::Player()
    : health(5),
      max_health(5),
      mana(2),
      magic_reserve(2),
      max_magic_reserve(2),
      experience(0),
      level(1),
      steps(15),
      inventory_slots(5),
      attempts(0),
      magic_access(false),
      agility(2),
      accuracy(0),
      stamina(1),
      intelligence(1),
      combat_experience(2),
      magic_control(2),
      magic_experience(0),
      psychic_stability(0),
      rifle_uses_per_battle(0) {}

void Player::AddItem(const std::string& item_id) {
    inventory.push_back(item_id);
}

void Player::RemoveItem(const std::string& item_id) {
    auto it = std::find(inventory.begin(), inventory.end(), item_id);
    if (it != inventory.end()) {
        inventory.erase(it);
    }
}

void Player::TakeDamage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}

void Player::Heal(int amount) {
    health += amount;
}

void Player::DisplayStats(const ItemManager& item_manager) const {
    std::cout << "\n--- Характеристики Героя ---" << std::endl;
    std::cout << "Здоровье: " << health << "/" << max_health << std::endl;
    std::cout << "Мана: " << mana << "/" << magic_reserve << std::endl;
    std::cout << "Уровень: " << level << " (Опыт: " << experience << ")" << std::endl;
    std::cout << "Шаги: " << steps << std::endl;
    std::cout << "Доступ к магии: " << (magic_access ? "Да" : "Нет") << std::endl;
    
    std::cout << "\n--- Базовые характеристики ---" << std::endl;
    std::cout << "Ловкость: " << agility << std::endl;
    std::cout << "Точность: " << accuracy << std::endl;
    std::cout << "Выносливость: " << stamina << std::endl;
    std::cout << "Интеллект: " << intelligence << std::endl;
    std::cout << "Боевой опыт: " << combat_experience << std::endl;
    std::cout << "Контроль магии: " << magic_control << std::endl;
    std::cout << "Магический опыт: " << magic_experience << std::endl;
    std::cout << "Психическая стабильность: " << psychic_stability << std::endl;

    std::cout << "\n--- Инвентарь и умения ---" << std::endl;
    std::cout << "Инвентарь: ";
    if (inventory.empty()) {
        std::cout << "пусто";
    } else {
        bool first = true;
        for (const auto& item_id : inventory) {
            if (!first) std::cout << ", ";
            const Item* item = item_manager.GetItem(item_id);
            if (item) {
                std::cout << item->name;
            } else {
                std::cout << item_id << "(не найден)";
            }
            first = false;
        }
    }
    std::cout << std::endl;

    std::cout << "Изученные заклинания: ";
    if (known_spells.empty()) {
        std::cout << "нет";
    } else {
        bool first = true;
        for (const auto& spell : known_spells) {
            if (!first) std::cout << ", ";
            std::cout << spell;
            first = false;
        }
    }
    std::cout << std::endl;

    std::cout << "Прочитанные книги: ";
    bool first_book = true;
    for (const auto& pair : books_actually_read) {
        if (pair.second) {
            if (!first_book) std::cout << ", ";
            std::cout << pair.first;
            first_book = false;
        }
    }
    if (first_book) std::cout << "нет";
    std::cout << std::endl;
    std::cout << "--------------------------" << std::endl;
}

void Player::ResetForNewAttempt(const Player& initial_config) {
    health = initial_config.health;
    mana = initial_config.mana;
}

bool Player::HasItem(const std::string& item_id) const {
    return std::find(inventory.begin(), inventory.end(), item_id) != inventory.end();
}

bool Player::KnowsSpell(const std::string& spell_name) const {
    return std::find(known_spells.begin(), known_spells.end(), spell_name) != known_spells.end();
}

int Player::GetStatValue(const std::string& stat_name) const {
    if (stat_name == "agility") return agility;
    if (stat_name == "accuracy") return accuracy;
    if (stat_name == "stamina") return stamina;
    if (stat_name == "intelligence") return intelligence;
    if (stat_name == "combat_experience") return combat_experience;
    if (stat_name == "magic_control") return magic_control;
    if (stat_name == "magic_experience") return magic_experience;
    if (stat_name == "psychic_stability") return psychic_stability;
    std::cerr << "Warning: Unknown stat '" << stat_name << "'" << std::endl;
    return 0;
}

void Player::ModifyStat(const std::string& stat_name, int value, int max_value) {
    int* stat_ptr = nullptr;
    if (stat_name == "agility") stat_ptr = &agility;
    else if (stat_name == "accuracy") stat_ptr = &accuracy;
    else if (stat_name == "stamina") stat_ptr = &stamina;
    else if (stat_name == "intelligence") stat_ptr = &intelligence;
    else if (stat_name == "combat_experience") stat_ptr = &combat_experience;
    else if (stat_name == "magic_control") stat_ptr = &magic_control;
    else if (stat_name == "magic_experience") stat_ptr = &magic_experience;
    else if (stat_name == "psychic_stability") stat_ptr = &psychic_stability;

    if (stat_ptr) {
        if (*stat_ptr < max_value) {
            *stat_ptr += value;
            if (*stat_ptr > max_value) *stat_ptr = max_value;
            std::cout << "Статистика '" << stat_name << "' увеличена до " << *stat_ptr << "." << std::endl;
        }
        else {
            std::cout << "Статистика '" << stat_name << "' уже максимальна (" << *stat_ptr << "/" << max_value << ")." << std::endl;
        }
    }
    else {
        std::cerr << "Warning: Attempt to modify unknown stat '" << stat_name << "'" << std::endl;
    }
}

int Player::GetUsedInventorySlots(const ItemManager& item_manager) const {
    int used_slots = 0;
    for (const std::string& item_id : inventory) {
        const Item* item = item_manager.GetItem(item_id);
        if (item) {
            if (item->id != "bag") {
                used_slots += item->costSlots;
            }
        }
    }
    return used_slots;
}

} // namespace domain 