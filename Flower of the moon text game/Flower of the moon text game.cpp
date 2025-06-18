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

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h> // Для установки кодировки консоли Windows
#endif

#include "config.h"
#include "utils.h"
#include "data_structures.h"
#include "file_parser.h"

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
void preparationPhase(PlayerStats& player, const GameData& gameData) {
    std::cout << "\n--- ФАЗА ПОДГОТОВКИ ---" << std::endl;

    while (player.steps > 0) {
        player.displayStats();
        int current_used_slots = calculateUsedSlots(player, gameData);
        std::cout << "Занято слотов: " << current_used_slots << "/" << player.inventorySlots << std::endl;
        std::cout << "Осталось шагов: " << player.steps << std::endl;
        std::cout << "Выберите действие:" << std::endl;

        int menuOption = 1;
        std::vector<std::function<void(bool&)>> availableMenuActions; // bool& - для отслеживания, было ли выполнено действие

        // 1. Взять предметы (книги, оружие, спец. предметы типа сумки)
        for (const auto& itemDef : gameData.items) {
            if (player.hasItem(itemDef.name)) continue; // Уже есть такой предмет

            // Особая логика для сумки - ее можно взять только один раз (проверяется через player.hasItem)
            // Но если она уже взята, и это preparation_item с эффектом add_slots, ее не предлагаем.

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
                            player.inventory.push_back(itemDef.name); // Добавляем сумку в инвентарь, чтобы ее не предлагали снова
                            std::cout << "'" << itemDef.name << "' взята. Слоты инвентаря увеличены до " << player.inventorySlots << "." << std::endl;
                            actionPerformed = true;
                        }
                        else { // Оружие или книга
                            if (current_used_slots + itemDef.costSlots <= player.inventorySlots) {
                                player.inventory.push_back(itemDef.name);
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
                int currentStat = getPlayerStatValueByName(player, prepActionDef.targetStat);
                if (currentStat >= prepActionDef.maxValue) continue; // Не предлагать, если стат уже максимален
            }


            std::cout << menuOption << ". " << prepActionDef.menuText << " (Шаги: " << prepActionDef.costSteps << ")" << std::endl;
            availableMenuActions.push_back(
                [&player, &prepActionDef](bool& actionPerformed) {
                    if (player.steps >= prepActionDef.costSteps) {
                        bool successInAction = false;
                        if (prepActionDef.effectType == "increase_stat") {
                            modifyPlayerStatByName(player, prepActionDef.targetStat, prepActionDef.effectValue, prepActionDef.maxValue, successInAction);
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
    const ItemData* rifleDef = gameData.findItem("ружье");
    if (rifleDef && player.hasItem("ружье")) {
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
        for (const std::string& itemName : player.inventory) {
            const ItemData* itemDef = gameData.findItem(itemName);
            if (itemDef && itemDef->type == "weapon") {
                std::cout << menuIdx++ << ". " << itemDef->battleActionName
                    << " (Пров: " << itemDef->checkStat << ", Урон: +" << itemDef->damage;
                if (itemDef->usesPerBattle > 0) { // Если использование ограничено (например, ружье)
                    int uses_left = (itemDef->name == "ружье") ? player.rifleUsesPerBattle : itemDef->usesPerBattle; // Пока только ружье отслеживается динамически
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
    int monstersDefeatedCount = 0;

    // Создаем копию списка монстров для этой сессии боев, чтобы их HP сбрасывалось корректно
    std::vector<MonsterData> currentBattleMonsters = gameData.monsters;

    for (MonsterData& monsterToFight : currentBattleMonsters) {
        // Перед каждым новым боем (с новым монстром) HP и мана игрока должны быть полными
        // (если это не первая попытка, то resetForNewAttempt уже был вызван)
        // На всякий случай, можно здесь еще раз восстановить, если логика этого требует.
        // Но по ТЗ, мана восстанавливается только при "новой попытке".
        // HP игрока сохраняется между боями в одной "попытке".

        bool victoryInSingleBattle = runSingleBattle(player, monsterToFight, gameData);
        if (victoryInSingleBattle) {
            std::cout << monsterToFight.name << " повержен! Готовьтесь к следующему..." << std::endl;
            monstersDefeatedCount++;
            // Мана НЕ восстанавливается после каждого монстра, только при новой попытке (согласно ТЗ)
        }
        else { // Игрок проиграл бой
            std::cout << "Вы проиграли бой с " << monsterToFight.name << "." << std::endl;
            player.attempts--; // Уменьшаем количество попыток
            if (player.attempts > 0) {
                std::cout << "У вас осталось " << player.attempts << " попыток. Бои начнутся сначала с первым монстром." << std::endl;
                player.resetForNewAttempt(gameData.initialPlayerStats); // Восстанавливаем HP/ману игрока
                return false; // Завершаем текущую фазу боев, игрок проиграл, но есть попытки
            }
            else {
                std::cout << "Попытки закончились. Игра окончена." << std::endl;
                return false; // Игрок окончательно проиграл
            }
        }
    }
    // Если цикл завершился, значит все монстры в списке побеждены
    return monstersDefeatedCount == currentBattleMonsters.size();
}


// --- Главная функция игры ---
int main() {
#if defined(_WIN32) || defined(_WIN64)
    SetConsoleCP(CP_UTF8); // Установка кодировки ввода для Windows
    SetConsoleOutputCP(CP_UTF8); // Установка кодировки вывода для Windows
#endif
     std::locale::global(std::locale("")); // Для корректной работы с локалью (в т.ч. русским языком)

    GameData gameData; // Здесь будут храниться все загруженные данные
    if (!loadGameData(gameData)) {
        std::cerr << "Критическая ошибка: Не удалось загрузить данные игры. Выход." << std::endl;
        std::cout << "Нажмите Enter для выхода.";
        std::cin.get();
        return 1;
    }

    std::cout << "Добро пожаловать в деревню эльфов!" << std::endl;
    std::cout << "Вам нужно доказать, что вы друг и свою полезность," << std::endl;
    std::cout << "для этого нужно подготовиться к бою и победить монстров." << std::endl;

    bool playGameAgain = true;
    while (playGameAgain) {
        PlayerStats currentPlayer = gameData.initialPlayerStats; // Создаем копию начальных статов для текущей игры

        preparationPhase(currentPlayer, gameData);

        if (currentPlayer.attempts <= 0) { // Если попытки кончились (маловероятно здесь, но для полноты)
            std::cout << "\nИгра окончена, так как закончились попытки еще до начала боев." << std::endl;
            playGameAgain = false;
            continue;
        }

        // Если шаги кончились или игрок решил закончить подготовку досрочно, и есть попытки
        std::cout << "\nПодготовка завершена. Ваши финальные характеристики перед боем:" << std::endl;
        currentPlayer.displayStats();
        std::cout << "\nНажмите Enter для начала сражений...";
        //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка буфера перед get()
        std::cin.get(); // Ожидание нажатия Enter

        bool allMonstersDefeatedInThisRun = false;
        // Цикл попыток для фазы боев
        while (currentPlayer.attempts > 0 && !allMonstersDefeatedInThisRun) {
            // Перед каждой серией боев (если это не первая попытка), HP и мана уже должны быть
            // восстановлены вызовом currentPlayer.resetForNewAttempt() в конце неудачной серии в battlePhase.

            allMonstersDefeatedInThisRun = battlePhase(currentPlayer, gameData);

            if (!allMonstersDefeatedInThisRun && currentPlayer.attempts > 0) { // Проиграл серию, но есть попытки
                std::cout << "\nГотовимся к новой попытке..." << std::endl;
                std::cout << "Нажмите Enter, чтобы продолжить...";
                std::cin.get();
                // Сброс HP/маны уже произошел в battlePhase
            }
        }

        // Результаты игры
        if (allMonstersDefeatedInThisRun) {
            std::cout << "\n\n****************************************" << std::endl;
            std::cout << "ПОЗДРАВЛЯЕМ! Вы победили всех монстров!" << std::endl;
            std::cout << "****************************************" << std::endl;
        }
        else { // Попытки закончились, и не все монстры побеждены
            std::cout << "\n\n=======================================" << std::endl;
            std::cout << "УВЫ! Вы проиграли. Попытки закончились." << std::endl;
            std::cout << "=======================================" << std::endl;
        }

        // Предложение сыграть еще раз
        std::cout << "\nХотите сыграть еще раз? (y/n): ";
        char repeatChoiceChar;
        std::cin >> repeatChoiceChar;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка буфера
        if (tolower(repeatChoiceChar) != 'y') {
            playGameAgain = false;
        }
    }

    std::cout << "Спасибо за игру! Нажмите Enter для выхода." << std::endl;
    std::cin.get();
    return 0;
}
