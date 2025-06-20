#include "data_structures.h" // Обязательно для связи с объявлениями
#include <iostream>           // Для std::cout и т.д.
#include <iomanip>            // Для std::setprecision, std::fixed
#include <algorithm>          // Для std::find
#include <memory>             // Для std::make_unique (если используется в findBattleActionDetails)

// --- Конструкторы (если были вынесены, или для инициализации по умолчанию) ---
PlayerStats::PlayerStats() : hp(0), maxHp(0), steps(0), inventorySlots(0), attempts(0),
agility(0), accuracy(0), stamina(0), intelligence(0), combatExperience(0),
magicControl(0), magicExperience(0), psychicStability(0),
magicAccess(false), magicReserve(0), maxMagicReserve(0), rifleUsesPerBattle(0) {
}

MonsterData::MonsterData() : initialHp(0), currentHp(0), damageToPlayer(0) {}

ItemData::ItemData() : costSteps(0), costSlots(0), prepEffectValue(0), damage(0), usesPerBattle(0) {}

SpellData::SpellData() : costSteps(0), requiresMagicAccessToLearn(false), damage(0), manaCost(0), instantWin(false), requiresMagicAccessToUse(false) {}

PreparationActionData::PreparationActionData() : costSteps(0), effectValue(0), maxValue(0), requiresMagicAccess(false) {}


// --- Реализации методов PlayerStats ---
void PlayerStats::displayStats(const domain::ItemManager& itemManager) const {
    std::cout << "\n=== СТАТИСТИКА ИГРОКА ===" << std::endl;
    std::cout << "Здоровье: " << hp << "/" << maxHp << std::endl;
    if (magicAccess) {
        std::cout << "Магический резерв: " << magicReserve << "/" << maxMagicReserve << std::endl;
    }
    std::cout << "Боевой опыт: " << combatExperience << std::endl;
    std::cout << "Выносливость: " << stamina << std::endl;
    std::cout << "Ум: " << intelligence << std::endl;
    std::cout << "\nИнвентарь:" << std::endl;
    if (inventory.empty()) {
        std::cout << "Пусто" << std::endl;
    }
    else {
        for (const auto& itemId : inventory) {
            const auto* item = itemManager.getItem(itemId);
            if (item) {
                std::cout << "- " << item->name << std::endl;
            }
        }
    }
    std::cout << "\nИзвестные заклинания:" << std::endl;
    if (knownSpells.empty()) {
        std::cout << "Нет" << std::endl;
    }
    else {
        for (const auto& spell : knownSpells) {
            std::cout << "- " << spell << std::endl;
        }
    }
    std::cout << "=========================" << std::endl;
}

void PlayerStats::resetForNewAttempt(const PlayerStats& initialConfig) {
    // Восстанавливаем HP и ману до ИЗНАЧАЛЬНЫХ максимальных значений
    hp = initialConfig.maxHp;
    maxHp = initialConfig.maxHp;
    magicReserve = initialConfig.maxMagicReserve;
    maxMagicReserve = initialConfig.maxMagicReserve;

    // Также сбрасываем временные боевые состояния
    rifleUsesPerBattle = 0;
}

bool PlayerStats::hasItem(const std::string& itemName) const {
    return std::find(inventory.begin(), inventory.end(), itemName) != inventory.end();
}

bool PlayerStats::knowsSpell(const std::string& spellName) const {
    return std::find(knownSpells.begin(), knownSpells.end(), spellName) != knownSpells.end();
}

// --- Реализации методов MonsterData ---
void MonsterData::displayInfo(const PlayerStats& player, const std::map<std::string, std::string>& bookContentsMap) const {
    std::cout << "\n--- Противник: " << name << " ---" << std::endl;
    bool bookWasReadByPlayer = false;
    auto bookIt = player.booksActuallyRead.find(bookKey); // bookKey - это имя книги, например "книга о волках"
    if (bookIt != player.booksActuallyRead.end() && bookIt->second) {
        bookWasReadByPlayer = true;
    }

    if (bookWasReadByPlayer) {
        auto contentIt = bookContentsMap.find(bookKey); // Ищем содержимое по тому же ключу
        if (contentIt != bookContentsMap.end()) {
            std::cout << contentIt->second << std::endl;
        }
        std::cout << "Здоровье: " << currentHp << "/" << initialHp << std::endl;
    }
    else {
        std::cout << "Информация о монстре скрыта (прочитайте соответствующую книгу)." << std::endl;
        std::cout << "Здоровье: " << currentHp << "/" << initialHp << std::endl; // HP показываем всегда
    }
    std::cout << "--------------------------" << std::endl;
}

double MonsterData::getActionCoefficient(const std::string& actionName) const {
    if (std::find(favorableActions.begin(), favorableActions.end(), actionName) != favorableActions.end()) {
        return 2.0;
    }
    if (std::find(unfavorableActions.begin(), unfavorableActions.end(), actionName) != unfavorableActions.end()) {
        return 0.5;
    }
    if (std::find(neutralActions.begin(), neutralActions.end(), actionName) != neutralActions.end()) {
        return 1.0;
    }
    // У Кентавра особое правило, обрабатывается здесь
    if (specialRule == "AllOthersUnfavorable" &&
        std::find(favorableActions.begin(), favorableActions.end(), actionName) == favorableActions.end() &&
        std::find(neutralActions.begin(), neutralActions.end(), actionName) == neutralActions.end()) {
        return 0.5;
    }
    return 1.0; // По умолчанию нейтрально, если не найдено в списках и нет спец. правила
}

void MonsterData::resetHp() {
    currentHp = initialHp;
}

// --- Реализации методов GameData ---
const ItemData* GameData::findItem(const std::string& itemId) const {
    for (const auto& item : items) {
        if (item.id == itemId) {
            return &item;
        }
    }
    return nullptr;
}

const SpellData* GameData::findSpell(const std::string& name) const {
    for (const auto& spell : spells) {
        if (spell.name == name) return &spell;
    }
    return nullptr;
}

std::unique_ptr<GameData::BattleActionInfo> GameData::findBattleActionDetails(const PlayerStats& player, const std::string& actionDisplayName) const {
    // Поиск среди оружия в инвентаре
    for (const auto& itemId : player.inventory) {
        const ItemData* item = findItem(itemId);
        if (item && item->type == "weapon" && item->battleActionName == actionDisplayName) {
            auto info = std::make_unique<BattleActionInfo>();
            info->name = item->battleActionName;
            info->checkStat = item->checkStat;
            info->damage = item->damage;
            info->manaCost = 0;
            info->isInstantWin = false;
            info->isSpell = false;
            info->isWeapon = true;
            info->weaponUsesTotal = item->usesPerBattle;
            info->originalItemName = item->name;
            return info;
        }
    }
    // Поиск среди изученных заклинаний
    for (const auto& spellName : player.knownSpells) {
        const SpellData* battleSpellDef = nullptr;
        for (const auto& s_def : this->spells) { // this->spells для явного указания члена класса
            if (s_def.name == spellName && s_def.type == "battle_spell" && s_def.battleActionName == actionDisplayName) {
                battleSpellDef = &s_def;
                break;
            }
        }
        if (battleSpellDef) {
            auto info = std::make_unique<BattleActionInfo>();
            info->name = battleSpellDef->battleActionName;
            info->checkStat = battleSpellDef->checkStat;
            info->damage = battleSpellDef->damage;
            info->manaCost = battleSpellDef->manaCost;
            info->isInstantWin = battleSpellDef->instantWin;
            info->isSpell = true;
            info->isWeapon = false;
            info->weaponUsesTotal = 0;
            return info;
        }
    }
    // Базовые действия
    if (actionDisplayName == "удар рукой") {
        auto info = std::make_unique<BattleActionInfo>();
        info->name = "удар рукой"; info->checkStat = "combatExperience"; info->damage = 0;
        info->isInstantWin = false; info->isSpell = false; info->isWeapon = false; info->weaponUsesTotal = 0;
        return info;
    }
    if (actionDisplayName == "бегство") {
        auto info = std::make_unique<BattleActionInfo>();
        info->name = "бегство"; info->checkStat = "stamina"; info->damage = 0;
        info->isInstantWin = true; info->isSpell = false; info->isWeapon = false; info->weaponUsesTotal = 0;
        return info;
    }
    if (actionDisplayName == "маскировка") {
        auto info = std::make_unique<BattleActionInfo>();
        info->name = "маскировка"; info->checkStat = "intelligence"; info->damage = 0;
        info->isInstantWin = true; info->isSpell = false; info->isWeapon = false; info->weaponUsesTotal = 0;
        return info;
    }
    return nullptr; // Действие не найдено
}