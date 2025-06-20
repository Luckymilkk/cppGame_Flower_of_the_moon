#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include "ItemManager.h"

namespace domain {

class Player {
public:
    // Конструктор с инициализацией значений по умолчанию
    Player() : 
        health(100),
        maxHealth(100),
        mana(50),
        magicReserve(50),
        maxMagicReserve(50),
        experience(0),
        level(1),
        steps(15),
        inventorySlots(5),
        attempts(0),
        magicAccess(false),
        agility(2),
        accuracy(0),
        stamina(1),
        intelligence(1),
        combatExperience(2),
        magicControl(2),
        magicExperience(0),
        psychicStability(0),
        rifleUsesPerBattle(0)
    {}

    // Базовые характеристики
    std::string name;
    int health;
    int maxHealth;
    int mana;
    int magicReserve;
    int maxMagicReserve;
    int experience;
    int level;
    int steps;
    int inventorySlots;
    int attempts;
    bool magicAccess;

    // Характеристики персонажа
    int agility;
    int accuracy;
    int stamina;
    int intelligence;
    int combatExperience;
    int magicControl;
    int magicExperience;
    int psychicStability;

    // Коллекции
    std::unordered_map<std::string, int> stats;
    std::vector<std::string> inventory;
    std::vector<std::string> knownSpells;
    std::map<std::string, bool> booksActuallyRead;
    int rifleUsesPerBattle;

    // Методы для управления игроком
    void addItem(const std::string& itemId);
    void removeItem(const std::string& itemId);
    void takeDamage(int amount);
    void heal(int amount);
    void displayStats(const ItemManager& itemManager) const;
    void resetForNewAttempt(const Player& initialConfig);
    bool hasItem(const std::string& itemId) const;
    bool knowsSpell(const std::string& spellName) const;
    
    // Новые методы для работы с характеристиками
    int getStatValue(const std::string& statName) const;
    void modifyStat(const std::string& statName, int value, int maxValue);
    int getUsedInventorySlots(const ItemManager& itemManager) const;
};

} // namespace domain 