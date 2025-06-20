#define NOMINMAX // Для Windows, чтобы избежать конфликтов min/max макросов
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <map>
#include <iomanip>
#include <cmath>    // Для std::round, std::abs
#include <functional> // для std::function в меню
#include <memory>     // для std::unique_ptr
#include <locale>
#include <clocale>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h> // Для установки кодировки консоли Windows
#endif

#include "config.h"
#include "utils.h"
#include "data_structures.h"
#include "file_parser.h"
#include "engine/GameEngine.h"
#include "domain/Player.h"
#include "domain/ItemManager.h"

using namespace domain;

// --- Вспомогательные функции для игровой логики ---

// Получение текущего значения характеристики игрока по ее строковому имени
int getPlayerStatValueByName(const PlayerStats& player, const std::string& statName) {
    if (statName == "agility") return player.agility;
    if (statName == "accuracy") return player.accuracy;
    if (statName == "stamina") return player.stamina;
    if (statName == "intelligence") return player.intelligence;
    if (statName == "combatExperience") return player.combatExperience;
    if (statName == "magicControl") return player.magicControl;
    if (statName == "magicExperience") return player.magicExperience;
    if (statName == "psychicStability") return player.psychicStability;
    std::cerr << "Предупреждение: Запрошена неизвестная характеристика игрока '" << statName << "'" << std::endl;
    return 0;
}

// Изменение характеристики игрока по ее строковому имени
void modifyPlayerStatByName(PlayerStats& player, const std::string& statName, int value, int maxValue, bool& actionSuccess) {
    actionSuccess = false;
    int* statPtr = nullptr;
    if (statName == "agility") statPtr = &player.agility;
    else if (statName == "accuracy") statPtr = &player.accuracy;
    else if (statName == "stamina") statPtr = &player.stamina;
    else if (statName == "intelligence") statPtr = &player.intelligence;
    else if (statName == "combatExperience") statPtr = &player.combatExperience;
    else if (statName == "magicControl") statPtr = &player.magicControl;
    else if (statName == "magicExperience") statPtr = &player.magicExperience;
    else if (statName == "psychicStability") statPtr = &player.psychicStability;

    if (statPtr) {
        if (*statPtr < maxValue) {
            *statPtr += value;
            if (*statPtr > maxValue) *statPtr = maxValue;
            std::cout << "Характеристика '" << statName << "' увеличена до " << *statPtr << "." << std::endl;
            actionSuccess = true;
        }
        else {
            std::cout << "Характеристика '" << statName << "' уже максимальна (" << *statPtr << "/" << maxValue << ")." << std::endl;
        }
    }
    else {
        std::cerr << "Предупреждение: Попытка изменить неизвестную характеристику '" << statName << "'" << std::endl;
    }
}

// Подсчет количества занятых слотов в инвентаре
int calculateUsedSlots(const PlayerStats& player, const GameData& gameData) {
    int used_slots = 0;
    for (const std::string& itemName : player.inventory) {
        const ItemData* itemDef = gameData.findItem(itemName);
        if (itemDef) {
            // Сумка не занимает слот, а добавляет. Другие предметы занимают.
            if (itemDef->name != "сумка") { // Предполагаем, что имя сумки уникально
                used_slots += itemDef->costSlots;
            }
        }
    }
    return used_slots;
}


// --- Фаза Подготовки ---
void preparationPhase(Player& player, const GameData& gameData, const ItemManager& itemManager) {
    std::cout << "\n--- ФАЗА ПОДГОТОВКИ ---" << std::endl;

    while (player.steps > 0) {
        player.displayStats(itemManager);
        int current_used_slots = player.getUsedInventorySlots(itemManager);
        std::cout << "Занято слотов: " << current_used_slots << "/" << player.inventorySlots << std::endl;
        std::cout << "Осталось шагов: " << player.steps << std::endl;
        std::cout << "Выберите действие:" << std::endl;

        int menuOption = 1;
        std::vector<std::function<void(bool&)>> availableMenuActions;

        // 1. Взять предметы (книги, оружие, спец. предметы типа сумки)
        for (const auto& itemDef : gameData.items) {
            if (player.hasItem(itemDef.id)) continue; // Проверяем по ID

            std::cout << menuOption << ". Взять '" << itemDef.name << "' (Шаги: " << itemDef.costSteps;
            if (itemDef.costSlots > 0) std::cout << ", Слоты: " << itemDef.costSlots;
            if (itemDef.type == "preparation_item" && itemDef.prepEffectType == "add_slots") {
                std::cout << ", Эффект: +" << itemDef.prepEffectValue << " слота инвентаря";
            }
            std::cout << ")" << std::endl;

            availableMenuActions.push_back(
                [&player, &itemDef, &gameData, current_used_slots](bool& actionPerformed) {
                    if (player.steps >= itemDef.costSteps) {
                        if (itemDef.type == "preparation_item" && itemDef.prepEffectType == "add_slots") {
                            player.inventorySlots += itemDef.prepEffectValue;
                            player.addItem(itemDef.id); // Используем ID
                            std::cout << "'" << itemDef.name << "' взята. Слоты инвентаря увеличены до " << player.inventorySlots << "." << std::endl;
                            actionPerformed = true;
                        }
                        else { // Оружие или книга
                            if (current_used_slots + itemDef.costSlots <= player.inventorySlots) {
                                player.addItem(itemDef.id); // Используем ID
                                std::cout << "'" << itemDef.name << "' добавлен в инвентарь." << std::endl;
                                if (itemDef.type == "book") { // Если это книга, сразу отмечаем как прочитанную
                                    player.booksActuallyRead[itemDef.bookKey] = true;
                                    std::cout << "Книга '" << itemDef.name << "' считается прочитанной." << std::endl;
                                }
                                actionPerformed = true;
                            }
                            else {
                                std::cout << "Недостаточно места в инвентаре!" << std::endl;
                            }
                        }
                        if (actionPerformed) player.steps -= itemDef.costSteps;
                    }
                    else {
                        std::cout << "Недостаточно шагов!" << std::endl;
                    }
                });
            menuOption++;
        }

        // 2. Изучить заклинания (тип "preparation_spell")
        for (const auto& spellDef : gameData.spells) {
            if (spellDef.type == "preparation_spell" && !player.knowsSpell(spellDef.name)) {
                if (spellDef.requiresMagicAccessToLearn && !player.magicAccess) continue;

                std::cout << menuOption << ". Освоить магию '" << spellDef.name << "' (Шаги: " << spellDef.costSteps << ")" << std::endl;
                availableMenuActions.push_back(
                    [&player, &spellDef](bool& actionPerformed) {
                        if (player.steps >= spellDef.costSteps) {
                            player.knownSpells.push_back(spellDef.name);
                            player.steps -= spellDef.costSteps;
                            std::cout << "Магия '" << spellDef.name << "' освоена." << std::endl;
                            actionPerformed = true;
                        }
                        else {
                            std::cout << "Недостаточно шагов!" << std::endl;
                        }
                    });
                menuOption++;
            }
        }

        // 3. Общие действия подготовки (из файла preparation_actions.txt)
        for (const auto& prepActionDef : gameData.preparationActions) {
            if (prepActionDef.requiresMagicAccess && !player.magicAccess) continue;
            if (prepActionDef.effectType == "enable_magic_access" && player.magicAccess) continue; // Не предлагать, если уже есть

            // Проверка, не достигнут ли максимум для прокачки стата
            if (prepActionDef.effectType == "increase_stat") {
                int currentStat = player.getStatValue(prepActionDef.targetStat);
                if (currentStat >= prepActionDef.maxValue) continue; // Не предлагать, если стат уже максимален
            }

            std::cout << menuOption << ". " << prepActionDef.menuText << " (Шаги: " << prepActionDef.costSteps << ")" << std::endl;
            availableMenuActions.push_back(
                [&player, &prepActionDef](bool& actionPerformed) {
                    if (player.steps >= prepActionDef.costSteps) {
                        bool successInAction = false;
                        if (prepActionDef.effectType == "increase_stat") {
                            player.modifyStat(prepActionDef.targetStat, prepActionDef.effectValue, prepActionDef.maxValue);
                            successInAction = true;
                        }
                        else if (prepActionDef.effectType == "enable_magic_access") {
                            if (!player.magicAccess) {
                                player.magicAccess = true;
                                std::cout << "Доступ к магии восстановлен!" << std::endl;
                                successInAction = true;
                            }
                            else {
                                std::cout << "Доступ к магии уже есть." << std::endl;
                            }
                        }
                        if (successInAction) {
                            player.steps -= prepActionDef.costSteps;
                            actionPerformed = true;
                        }
                    }
                    else {
                        std::cout << "Недостаточно шагов!" << std::endl;
                    }
                });
            menuOption++;
        }

        // Опция прочитать книгу (если нужно отдельное действие, а не при взятии)
        // По ТЗ, книга считается прочитанной при взятии (см. case 1 в оригинале).
        // Если нужно отдельное действие "прочитать", то логику player.booksActuallyRead[itemDef.bookKey] = true;
        // нужно будет перенести сюда и изменить условие взятия книги.
        // Пока что оставляю как в ТЗ - книга читается при взятии.

        std::cout << "0. Закончить подготовку досрочно" << std::endl;

        int choice = getIntInput("Ваш выбор: ", 0, menuOption - 1);

        if (choice == 0) {
            std::cout << "Подготовка завершена досрочно." << std::endl;
            break;
        }
        if (choice > 0 && choice <= availableMenuActions.size()) {
            bool actionWasPerformed = false;
            availableMenuActions[choice - 1](actionWasPerformed);
            // Шаги уже вычитаются внутри лямбда-функций при успехе
        }
        else {
            std::cout << "Неверный выбор." << std::endl;
        }
        std::cout << std::endl; // Дополнительный отступ для читаемости
    }

    if (player.steps <= 0) {
        std::cout << "\nШаги для подготовки закончились!" << std::endl;
    }
    std::cout << "--- ПОДГОТОВКА ЗАВЕРШЕНА ---" << std::endl;
}


// --- Фаза Боя ---
bool runSingleBattle(PlayerStats& player, MonsterData& currentMonster, const GameData& gameData) {
    std::cout << "\n--- НАЧИНАЕТСЯ БОЙ С " << currentMonster.name << "! ---" << std::endl;
    currentMonster.resetHp(); // Убедимся, что у монстра полное HP в начале боя

    // Сброс использований ружья (если оно есть) в начале каждого боя
    const ItemData* rifleDef = gameData.findItem("gun"); // Используем ID ружья
    if (rifleDef && player.hasItem("gun")) {
        player.rifleUsesPerBattle = rifleDef->usesPerBattle;
    }
    else {
        player.rifleUsesPerBattle = 0;
    }

    while (player.hp > 0 && currentMonster.currentHp > 0) {
        currentMonster.displayInfo(player, gameData.bookContents);
        std::cout << "Ваше здоровье: " << player.hp << "/" << player.maxHp;
        if (player.magicAccess) std::cout << ", Мана: " << player.magicReserve << "/" << player.maxMagicReserve;
        std::cout << std::endl;

        std::cout << "\nВыберите действие в бою:" << std::endl;
        int menuIdx = 1;
        // Сохраняем имена боевых действий, чтобы по индексу меню получить выбранное
        std::vector<std::string> availableBattleActionDisplayNames;

        // 1. Базовые действия (всегда доступны)
        std::cout << menuIdx++ << ". Удар рукой (Проверка: Боевой опыт)" << std::endl;
        availableBattleActionDisplayNames.push_back("удар рукой");
        std::cout << menuIdx++ << ". Бегство (Проверка: Выносливость)" << std::endl;
        availableBattleActionDisplayNames.push_back("бегство");
        std::cout << menuIdx++ << ". Маскировка (Проверка: Ум)" << std::endl;
        availableBattleActionDisplayNames.push_back("маскировка");

        // 2. Оружие из инвентаря
        for (const std::string& itemId : player.inventory) {
            const ItemData* itemDef = gameData.findItem(itemId);
            if (itemDef && itemDef->type == "weapon") {
                std::cout << menuIdx++ << ". " << itemDef->battleActionName
                    << " (Пров: " << itemDef->checkStat << ", Урон: +" << itemDef->damage;
                if (itemDef->usesPerBattle > 0) { // Если использование ограничено (например, ружье)
                    int uses_left = (itemDef->id == "gun") ? player.rifleUsesPerBattle : itemDef->usesPerBattle;
                    std::cout << ", Осталось: " << uses_left;
                }
                std::cout << ")" << std::endl;
                availableBattleActionDisplayNames.push_back(itemDef->battleActionName);
            }
        }

        // 3. Заклинания (если есть доступ к магии и изучены)
        if (player.magicAccess) {
            for (const std::string& spellName : player.knownSpells) {
                // Нам нужна боевая версия этого заклинания
                const SpellData* battleSpellDef = nullptr;
                for (const auto& s_def : gameData.spells) {
                    if (s_def.name == spellName && s_def.type == "battle_spell") {
                        battleSpellDef = &s_def;
                        break;
                    }
                }

                if (battleSpellDef && battleSpellDef->requiresMagicAccessToUse) {
                    std::cout << menuIdx++ << ". " << battleSpellDef->battleActionName
                        << " (Мана: " << battleSpellDef->manaCost
                        << ", Пров: " << battleSpellDef->checkStat;
                    if (battleSpellDef->damage > 0) std::cout << ", Урон: +" << battleSpellDef->damage;
                    if (battleSpellDef->instantWin) std::cout << ", Мгновенная победа";
                    std::cout << ")" << std::endl;
                    availableBattleActionDisplayNames.push_back(battleSpellDef->battleActionName);
                }
            }
        }

        int choice = getIntInput("Ваш выбор: ", 1, menuIdx - 1);
        std::string chosenActionDisplayName = availableBattleActionDisplayNames[choice - 1];

        std::unique_ptr<GameData::BattleActionInfo> actionDetails = gameData.findBattleActionDetails(player, chosenActionDisplayName);

        if (!actionDetails) {
            std::cerr << "Критическая ошибка: Не удалось найти детали для боевого действия '" << chosenActionDisplayName << "'" << std::endl;
            continue; // Пропускаем ход, чтобы избежать падения
        }

        // Проверки перед выполнением действия
        if (actionDetails->isSpell && actionDetails->manaCost > player.magicReserve) {
            std::cout << "Недостаточно маны!" << std::endl;
            continue;
        }
        if (actionDetails->isWeapon && actionDetails->originalItemName == "ружье" && player.rifleUsesPerBattle <= 0) {
            std::cout << "Выстрелы из ружья закончились в этом бою!" << std::endl;
            continue;
        }

        int playerStatVal = getPlayerStatValueByName(player, actionDetails->checkStat);
        double monsterCoefficient = currentMonster.getActionCoefficient(actionDetails->name); // Используем имя действия из BattleActionInfo
        double calculatedResult = monsterCoefficient * static_cast<double>(playerStatVal);

        std::cout << "Вы выбрали: " << actionDetails->name << ". Коэфф.: " << monsterCoefficient
            << ", Ваша хар-ка (" << actionDetails->checkStat << "): " << playerStatVal
            << ", Результат: " << std::fixed << std::setprecision(1) << calculatedResult << std::endl;

        bool monsterTurnSkipped = false;
        bool playerActionDealsDamage = false;

        // Согласно ТЗ: > 1.5 успешно, < 1.5 провалено, == 1.5 нейтрально
        const double successThreshold = 1.5;
        const double epsilon = 1e-5; // для сравнения double

        if (calculatedResult > successThreshold + epsilon) { // Успех
            std::cout << "Успешное действие!" << std::endl;
            playerActionDealsDamage = (actionDetails->damage > 0 || actionDetails->name == "удар рукой"); // Удар рукой может наносить урон (если будет логика базового урона)
            monsterTurnSkipped = true; // Монстр пропускает ход
            if (actionDetails->isInstantWin) {
                std::cout << currentMonster.name << " побежден с помощью '" << actionDetails->name << "'!" << std::endl;
                currentMonster.currentHp = 0;
            }
        }
        else if (calculatedResult < successThreshold - epsilon) { // Провал
            std::cout << "Действие провалено!" << std::endl;
            // Урон не наносится, монстр не пропускает ход
        }
        else { // Нейтрально (результат примерно равен 1.5)
            std::cout << "Действие нейтрально." << std::endl;
            playerActionDealsDamage = (actionDetails->damage > 0 || actionDetails->name == "удар рукой");
            // Монстр не пропускает ход
        }

        if (playerActionDealsDamage && currentMonster.currentHp > 0 && !actionDetails->isInstantWin) {
            // Урон от "удара рукой" по ТЗ 0. Если в будущем будет базовый урон игрока, его нужно будет добавить.
            // Пока что урон только от оружия/заклинаний.
            int damageToMonster = actionDetails->damage;
            if (damageToMonster > 0) {
                currentMonster.currentHp -= damageToMonster;
                std::cout << currentMonster.name << " получает " << damageToMonster << " урона." << std::endl;
                if (currentMonster.currentHp <= 0) {
                    currentMonster.currentHp = 0;
                    std::cout << currentMonster.name << " побежден!" << std::endl;
                }
            }
            else if (actionDetails->name == "удар рукой") {
                std::cout << "Ваш удар рукой не нанес урона." << std::endl;
            }
        }

        // Расход ресурсов
        if (actionDetails->isSpell) {
            player.magicReserve -= actionDetails->manaCost;
        }
        if (actionDetails->isWeapon && actionDetails->originalItemName == "ружье" && actionDetails->weaponUsesTotal > 0) {
            if (calculatedResult > successThreshold + epsilon || std::abs(calculatedResult - successThreshold) < epsilon) { // Тратим заряд при успехе или нейтральном
                player.rifleUsesPerBattle--;
            }
        }

        // Ход монстра
        if (currentMonster.currentHp > 0 && !monsterTurnSkipped) {
            std::cout << currentMonster.name << " атакует!" << std::endl;
            player.hp -= currentMonster.damageToPlayer;
            std::cout << "Вы получаете " << currentMonster.damageToPlayer << " урона." << std::endl;
            if (player.hp <= 0) {
                player.hp = 0; // Чтобы не было отрицательного HP
                std::cout << "Вы были побеждены..." << std::endl;
                return false; // Игрок проиграл бой
            }
        }
        else if (currentMonster.currentHp > 0 && monsterTurnSkipped) {
            std::cout << currentMonster.name << " пропускает ход!" << std::endl;
        }
        std::cout << std::endl; // Дополнительный отступ
    } // конец while (player.hp > 0 && currentMonster.currentHp > 0)

    return currentMonster.currentHp <= 0; // true если монстр побежден, false если игрок
}

// --- Основной цикл боев (через всех монстров) ---
bool battlePhase(PlayerStats& player, const GameData& gameData) {
    std::cout << "\n--- НАЧИНАЕТСЯ ФАЗА СРАЖЕНИЙ ---" << std::endl;
    
    // Сохраняем состояние игрока перед началом всех боев.
    // При провале попытки мы будем откатываться к этому состоянию.
    PlayerStats playerStateBeforeBattles = player;

    while (player.attempts > 0) {
        std::cout << "\n--- ПОПЫТКА #" << (playerStateBeforeBattles.attempts - player.attempts + 1) 
                  << ". Осталось: " << player.attempts << " ---" << std::endl;
        
        // Восстанавливаем игрока до состояния на начало фазы боев
        // (характеристики, инвентарь), но сохраняем текущее количество попыток.
        int currentAttempts = player.attempts;
        player = playerStateBeforeBattles;
        player.attempts = currentAttempts;

        // Сбрасываем HP/ману до максимума для новой попытки
        player.resetForNewAttempt(playerStateBeforeBattles);

        int monstersDefeatedCount = 0;
        bool attemptLost = false;
        
        // Создаем копию списка монстров для этой сессии боев
        std::vector<MonsterData> currentBattleMonsters = gameData.monsters;

        for (MonsterData& monsterToFight : currentBattleMonsters) {
            bool victoryInSingleBattle = runSingleBattle(player, monsterToFight, gameData);
            
            if (victoryInSingleBattle) {
                std::cout << monsterToFight.name << " повержен! Готовьтесь к следующему..." << std::endl;
                monstersDefeatedCount++;
                // HP НЕ восстанавливается между боями в рамках одной попытки
            } else { // Игрок проиграл бой
                std::cout << "Вы были побеждены в бою." << std::endl;
                player.attempts--; // Уменьшаем количество попыток
                attemptLost = true;
                
                if (player.attempts > 0) {
                    std::cout << "Попробуйте еще раз. Бои начнутся с начала." << std::endl;
                } else {
                    std::cout << "У вас не осталось попыток." << std::endl;
                }
                break; // Прерываем текущую серию боев, чтобы начать новую попытку (или закончить игру)
            }
        }

        if (!attemptLost) {
            // Если мы здесь, значит игрок прошел всех монстров не умерев
            std::cout << "\nПоздравляем! Вы победили всех монстров!" << std::endl;
            return true; // Победа в игре
        }
        // Если attemptLost == true, цикл while просто перейдет к следующей итерации (новой попытке)
    }
    
    // Если мы вышли из while, значит все попытки исчерпаны
    std::cout << "Игра окончена." << std::endl;
    return false; // Игрок окончательно проиграл
}


// --- Главная функция игры ---
int main() {
    try {
#if defined(_WIN32) || defined(_WIN64)
        SetConsoleCP(65001);
        SetConsoleOutputCP(65001);
#endif
        std::setlocale(LC_ALL, "ru_RU.UTF-8");
        std::locale::global(std::locale("ru_RU.UTF-8"));

        engine::GameEngine game;
        game.run();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        std::cout << "Нажмите Enter для выхода...";
        std::cin.get();
        return 1;
    }
}


