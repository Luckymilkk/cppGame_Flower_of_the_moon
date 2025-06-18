#pragma once
#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <string>
#include <vector>
#include <map>
#include <memory> // для std::unique_ptr
// НЕ НУЖНЫ здесь: <iostream>, <iomanip>, <algorithm> для реализаций, они уйдут в .cpp

// --- Структуры для хранения данных ---

struct PlayerStats {
    int hp;
    int maxHp;
    int steps;
    int inventorySlots;
    int attempts;

    int agility;
    int accuracy;
    int stamina;
    int intelligence;
    int combatExperience;
    int magicControl;
    int magicExperience;
    int psychicStability;

    bool magicAccess;
    int magicReserve;
    int maxMagicReserve;

    std::vector<std::string> inventory;
    std::vector<std::string> knownSpells;
    std::map<std::string, bool> booksActuallyRead;
    int rifleUsesPerBattle;

    PlayerStats(); // Конструктор может остаться inline, если он простой, или тоже вынесен

    // ОБЪЯВЛЕНИЯ методов
    void displayStats() const;
    void resetForNewAttempt(const PlayerStats& initialConfig);
    bool hasItem(const std::string& itemName) const;
    bool knowsSpell(const std::string& spellName) const;
};

struct MonsterData {
    std::string name;
    int initialHp;
    int currentHp;
    int damageToPlayer;
    std::string bookKey;
    std::vector<std::string> favorableActions;
    std::vector<std::string> unfavorableActions;
    std::vector<std::string> neutralActions;
    std::string specialRule;

    MonsterData(); // Конструктор

    // ОБЪЯВЛЕНИЯ методов
    void displayInfo(const PlayerStats& player, const std::map<std::string, std::string>& bookContents) const;
    double getActionCoefficient(const std::string& actionName) const;
    void resetHp();
};

struct ItemData {
    // ... (содержимое ItemData как было) ...
    std::string name;
    std::string type;
    int costSteps;
    int costSlots;
    std::string prepEffectType;
    int prepEffectValue;
    std::string battleActionName;
    std::string checkStat;
    int damage;
    int usesPerBattle;
    std::string bookKey;

    ItemData(); // Конструктор
};

struct SpellData {
    // ... (содержимое SpellData как было) ...
    std::string name;
    std::string type;
    int costSteps;
    bool requiresMagicAccessToLearn;
    std::string battleActionName;
    std::string checkStat;
    int damage;
    int manaCost;
    bool instantWin;
    bool requiresMagicAccessToUse;

    SpellData(); // Конструктор
};

struct PreparationActionData {
    // ... (содержимое PreparationActionData как было) ...
    std::string menuText;
    int costSteps;
    std::string effectType;
    std::string targetStat;
    int effectValue;
    int maxValue;
    bool requiresMagicAccess;

    PreparationActionData(); // Конструктор
};

struct GameData {
    PlayerStats initialPlayerStats;
    std::vector<MonsterData> monsters;
    std::vector<ItemData> items;
    std::vector<SpellData> spells;
    std::map<std::string, std::string> bookContents;
    std::vector<PreparationActionData> preparationActions;

    struct BattleActionInfo {
        std::string name;
        std::string checkStat;
        int damage;
        int manaCost;
        bool isInstantWin;
        bool isSpell;
        bool isWeapon;
        int weaponUsesTotal;
        std::string originalItemName;
    };

    // ОБЪЯВЛЕНИЯ методов
    const ItemData* findItem(const std::string& name) const;
    const SpellData* findSpell(const std::string& name) const;
    std::unique_ptr<BattleActionInfo> findBattleActionDetails(const PlayerStats& player, const std::string& actionDisplayName) const;
};

#endif // DATA_STRUCTURES_H