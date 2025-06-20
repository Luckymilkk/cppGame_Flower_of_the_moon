#include "Player.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace domain {

void Player::addItem(const std::string& itemId) {
    inventory.push_back(itemId);
}

void Player::removeItem(const std::string& itemId) {
    auto it = std::find(inventory.begin(), inventory.end(), itemId);
    if (it != inventory.end()) {
        inventory.erase(it);
    }
}

void Player::takeDamage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}

void Player::heal(int amount) {
    health += amount;
}

void Player::displayStats(const ItemManager& itemManager) const {
    std::cout << "\n--- Характеристики Героя ---" << std::endl;
    std::cout << "Здоровье: " << health << "/" << maxHealth << std::endl;
    std::cout << "Мана: " << mana << "/" << magicReserve << std::endl;
    std::cout << "Уровень: " << level << " (Опыт: " << experience << ")" << std::endl;
    std::cout << "Шаги: " << steps << std::endl;
    std::cout << "Доступ к магии: " << (magicAccess ? "Да" : "Нет") << std::endl;
    
    std::cout << "\n--- Базовые характеристики ---" << std::endl;
    std::cout << "Ловкость: " << agility << std::endl;
    std::cout << "Точность: " << accuracy << std::endl;
    std::cout << "Выносливость: " << stamina << std::endl;
    std::cout << "Интеллект: " << intelligence << std::endl;
    std::cout << "Боевой опыт: " << combatExperience << std::endl;
    std::cout << "Контроль магии: " << magicControl << std::endl;
    std::cout << "Магический опыт: " << magicExperience << std::endl;
    std::cout << "Психическая стабильность: " << psychicStability << std::endl;

    std::cout << "\n--- Инвентарь и умения ---" << std::endl;
    std::cout << "Инвентарь: ";
    if (inventory.empty()) {
        std::cout << "пусто";
    } else {
        bool first = true;
        for (const auto& itemId : inventory) {
            if (!first) std::cout << ", ";
            const Item* item = itemManager.getItem(itemId);
            if (item) {
                std::cout << item->name;
            } else {
                std::cout << itemId << "(не найден)";
            }
            first = false;
        }
    }
    std::cout << std::endl;

    std::cout << "Изученные заклинания: ";
    if (knownSpells.empty()) {
        std::cout << "нет";
    } else {
        bool first = true;
        for (const auto& spell : knownSpells) {
            if (!first) std::cout << ", ";
            std::cout << spell;
            first = false;
        }
    }
    std::cout << std::endl;

    std::cout << "Прочитанные книги: ";
    bool firstBook = true;
    for (const auto& pair : booksActuallyRead) {
        if (pair.second) {
            if (!firstBook) std::cout << ", ";
            std::cout << pair.first;
            firstBook = false;
        }
    }
    if (firstBook) std::cout << "нет";
    std::cout << std::endl;
    std::cout << "--------------------------" << std::endl;
}

void Player::resetForNewAttempt(const Player& initialConfig) {
    health = initialConfig.health;
    mana = initialConfig.mana;
}

bool Player::hasItem(const std::string& itemId) const {
    return std::find(inventory.begin(), inventory.end(), itemId) != inventory.end();
}

bool Player::knowsSpell(const std::string& spellName) const {
    return std::find(knownSpells.begin(), knownSpells.end(), spellName) != knownSpells.end();
}

int Player::getStatValue(const std::string& statName) const {
    if (statName == "agility") return agility;
    if (statName == "accuracy") return accuracy;
    if (statName == "stamina") return stamina;
    if (statName == "intelligence") return intelligence;
    if (statName == "combatExperience") return combatExperience;
    if (statName == "magicControl") return magicControl;
    if (statName == "magicExperience") return magicExperience;
    if (statName == "psychicStability") return psychicStability;
    std::cerr << "Предупреждение: Запрошена неизвестная характеристика '" << statName << "'" << std::endl;
    return 0;
}

void Player::modifyStat(const std::string& statName, int value, int maxValue) {
    int* statPtr = nullptr;
    if (statName == "agility") statPtr = &agility;
    else if (statName == "accuracy") statPtr = &accuracy;
    else if (statName == "stamina") statPtr = &stamina;
    else if (statName == "intelligence") statPtr = &intelligence;
    else if (statName == "combatExperience") statPtr = &combatExperience;
    else if (statName == "magicControl") statPtr = &magicControl;
    else if (statName == "magicExperience") statPtr = &magicExperience;
    else if (statName == "psychicStability") statPtr = &psychicStability;

    if (statPtr) {
        if (*statPtr < maxValue) {
            *statPtr += value;
            if (*statPtr > maxValue) *statPtr = maxValue;
            std::cout << "Характеристика '" << statName << "' увеличена до " << *statPtr << "." << std::endl;
        }
        else {
            std::cout << "Характеристика '" << statName << "' уже максимальна (" << *statPtr << "/" << maxValue << ")." << std::endl;
        }
    }
    else {
        std::cerr << "Предупреждение: Попытка изменить неизвестную характеристику '" << statName << "'" << std::endl;
    }
}

int Player::getUsedInventorySlots(const ItemManager& itemManager) const {
    int usedSlots = 0;
    for (const std::string& itemId : inventory) {
        const Item* item = itemManager.getItem(itemId);
        if (item) {
            if (item->id != "bag") {
                usedSlots += item->costSlots;
            }
        }
    }
    return usedSlots;
}

} // namespace domain 