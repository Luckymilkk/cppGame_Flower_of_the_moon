#include "file_parser.h"
#include "utils.h" // для splitString, trimString
#include <fstream>
#include<codecvt>
#include <sstream>
#include <iostream> // Для сообщений об ошибках и отладки

// Вспомогательная функция для парсинга строки формата "ключ: значение"
bool parseKeyValue(const std::string& line, std::string& key, std::string& value) {
    size_t delimiterPos = line.find(':');
    if (delimiterPos == std::string::npos) {
        return false; // Разделитель ':' не найден
    }
    key = trimString(line.substr(0, delimiterPos));
    value = trimString(line.substr(delimiterPos + 1));
    return true;
}

bool loadPlayerStatsFromFile(PlayerStats& playerStats, const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл характеристик игрока: " << filename << std::endl;
        return false;
    }
    std::string line, key, value;
    while (std::getline(file, line)) {
        line = trimString(line);
        if (line.empty() || line[0] == '#') continue; // Пропускаем пустые строки и комментарии

        if (parseKeyValue(line, key, value)) {
            try {
                if (key == "hp") playerStats.hp = std::stoi(value);
                else if (key == "maxHp") playerStats.maxHp = std::stoi(value);
                else if (key == "steps") playerStats.steps = std::stoi(value);
                else if (key == "inventorySlots") playerStats.inventorySlots = std::stoi(value);
                else if (key == "attempts") playerStats.attempts = std::stoi(value);
                else if (key == "agility") playerStats.agility = std::stoi(value);
                else if (key == "accuracy") playerStats.accuracy = std::stoi(value);
                else if (key == "stamina") playerStats.stamina = std::stoi(value);
                else if (key == "intelligence") playerStats.intelligence = std::stoi(value);
                else if (key == "combatExperience") playerStats.combatExperience = std::stoi(value);
                else if (key == "magicControl") playerStats.magicControl = std::stoi(value);
                else if (key == "magicExperience") playerStats.magicExperience = std::stoi(value);
                else if (key == "psychicStability") playerStats.psychicStability = std::stoi(value);
                else if (key == "magicAccess") playerStats.magicAccess = (value == "true" || value == "1");
                else if (key == "magicReserve") playerStats.magicReserve = std::stoi(value);
                else if (key == "maxMagicReserve") playerStats.maxMagicReserve = std::stoi(value);
            }
            catch (const std::exception& e) {
                std::cerr << "Ошибка парсинга player_stats.txt для ключа '" << key << "': " << e.what() << std::endl;
            }
        }
    }
    file.close();
    return true;
}

bool loadMonstersFromFile(std::vector<MonsterData>& monsters, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл монстров: " << filename << std::endl;
        return false;
    }
    std::string line, key, value;
    MonsterData currentMonster;
    bool inMonsterBlock = false; // Флаг, что мы находимся внутри блока данных одного монстра

    while (std::getline(file, line)) {
        line = trimString(line);
        if (line.empty() || line[0] == '#') continue;

        if (line == "---") { // Разделитель между монстрами
            if (inMonsterBlock && !currentMonster.name.empty()) { // Если были данные для предыдущего
                currentMonster.resetHp(); // Устанавливаем currentHp = initialHp
                monsters.push_back(currentMonster);
            }
            currentMonster = MonsterData(); // Сбрасываем для нового монстра
            inMonsterBlock = true;
            continue;
        }

        if (inMonsterBlock && parseKeyValue(line, key, value)) {
            try {
                if (key == "name") currentMonster.name = value;
                else if (key == "hp") currentMonster.initialHp = std::stoi(value);
                else if (key == "damageToPlayer") currentMonster.damageToPlayer = std::stoi(value);
                else if (key == "bookKey") currentMonster.bookKey = value;
                else if (key == "favorableActions") currentMonster.favorableActions = splitString(value, ',');
                else if (key == "unfavorableActions") currentMonster.unfavorableActions = splitString(value, ',');
                else if (key == "neutralActions") currentMonster.neutralActions = splitString(value, ',');
                else if (key == "specialRule") currentMonster.specialRule = value;
            }
            catch (const std::exception& e) {
                std::cerr << "Ошибка парсинга monsters.txt для монстра '" << currentMonster.name << "', ключ '" << key << "': " << e.what() << std::endl;
            }
        }
    }
    // Добавляем последнего монстра, если файл не заканчивается на "---"
    if (inMonsterBlock && !currentMonster.name.empty()) {
        currentMonster.resetHp();
        monsters.push_back(currentMonster);
    }
    file.close();
    return true;
}

bool loadItemsFromFile(std::vector<ItemData>& items, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл предметов: " << filename << std::endl;
        return false;
    }
    std::string line, key, value;
    ItemData currentItem;
    bool inItemBlock = false;

    while (std::getline(file, line)) {
        line = trimString(line);
        if (line.empty() || line[0] == '#') continue;

        if (line == "---") {
            if (inItemBlock && !currentItem.name.empty()) {
                items.push_back(currentItem);
            }
            currentItem = ItemData();
            inItemBlock = true;
            continue;
        }

        if (inItemBlock && parseKeyValue(line, key, value)) {
            try {
                if (key == "name") currentItem.name = value;
                else if (key == "type") currentItem.type = value;
                else if (key == "costSteps") currentItem.costSteps = std::stoi(value);
                else if (key == "costSlots") currentItem.costSlots = std::stoi(value);
                else if (key == "prepEffectType") currentItem.prepEffectType = value;
                else if (key == "prepEffectValue") currentItem.prepEffectValue = std::stoi(value);
                else if (key == "battleActionName") currentItem.battleActionName = value;
                else if (key == "checkStat") currentItem.checkStat = value;
                else if (key == "damage") currentItem.damage = std::stoi(value);
                else if (key == "usesPerBattle") currentItem.usesPerBattle = std::stoi(value);
                else if (key == "bookKey") currentItem.bookKey = value;
            }
            catch (const std::exception& e) {
                std::cerr << "Ошибка парсинга items.txt для предмета '" << currentItem.name << "', ключ '" << key << "': " << e.what() << std::endl;
            }
        }
    }
    if (inItemBlock && !currentItem.name.empty()) {
        items.push_back(currentItem);
    }
    file.close();
    return true;
}

bool loadSpellsFromFile(std::vector<SpellData>& spells, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл заклинаний: " << filename << std::endl;
        return false;
    }
    std::string line, key, value;
    SpellData currentSpell;
    bool inSpellBlock = false;

    while (std::getline(file, line)) {
        line = trimString(line);
        if (line.empty() || line[0] == '#') continue;

        if (line == "---") {
            if (inSpellBlock && !currentSpell.name.empty()) {
                spells.push_back(currentSpell);
            }
            currentSpell = SpellData();
            inSpellBlock = true;
            continue;
        }
        if (inSpellBlock && parseKeyValue(line, key, value)) {
            try {
                if (key == "name") currentSpell.name = value;
                else if (key == "type") currentSpell.type = value;
                else if (key == "costSteps") currentSpell.costSteps = std::stoi(value);
                else if (key == "requiresMagicAccessToLearn") currentSpell.requiresMagicAccessToLearn = (value == "true" || value == "1");
                else if (key == "battleActionName") currentSpell.battleActionName = value;
                else if (key == "checkStat") currentSpell.checkStat = value;
                else if (key == "damage") currentSpell.damage = std::stoi(value);
                else if (key == "manaCost") currentSpell.manaCost = std::stoi(value);
                else if (key == "instantWin") currentSpell.instantWin = (value == "true" || value == "1");
                else if (key == "requiresMagicAccessToUse") currentSpell.requiresMagicAccessToUse = (value == "true" || value == "1");
            }
            catch (const std::exception& e) {
                std::cerr << "Ошибка парсинга spells.txt для заклинания '" << currentSpell.name << "', ключ '" << key << "': " << e.what() << std::endl;
            }
        }
    }
    if (inSpellBlock && !currentSpell.name.empty()) {
        spells.push_back(currentSpell);
    }
    file.close();
    return true;
}

bool loadBooksFromFile(std::map<std::string, std::string>& bookContents, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл книг: " << filename << std::endl;
        return false;
    }
    std::string line;
    std::string currentBookKey;
    std::string currentBookText;
    bool inBookBlock = false;

    while (std::getline(file, line)) {
        std::string trimmedLine = trimString(line);
        if (trimmedLine.empty() || trimmedLine[0] == '#') continue;

        if (trimmedLine == "---") {
            if (inBookBlock && !currentBookKey.empty()) {
                bookContents[currentBookKey] = trimString(currentBookText);
            }
            currentBookKey.clear();
            currentBookText.clear();
            inBookBlock = true;
            continue;
        }

        if (inBookBlock) {
            std::string key, value;
            // Ключ и контент могут быть на разных строках, но "key:" всегда первый
            if (parseKeyValue(line, key, value)) { // Используем оригинальную 'line', не 'trimmedLine' для value
                if (key == "key") {
                    currentBookKey = value; // value уже trim-нуто parseKeyValue
                }
                else if (key == "content" && !currentBookKey.empty()) {
                    currentBookText += value + "\n"; // Добавляем с новой строки, если контент многострочный
                }
                else if (!currentBookKey.empty() && key != "key") { // Если это продолжение content без "content:"
                    currentBookText += line + "\n"; // Добавляем всю строку (не trim-нутую)
                }
            }
            else if (!currentBookKey.empty()) { // Если это просто строка контента без "ключ:"
                currentBookText += line + "\n";
            }
        }
    }
    if (inBookBlock && !currentBookKey.empty()) {
        bookContents[currentBookKey] = trimString(currentBookText);
    }
    file.close();
    return true;
}

bool loadPreparationActionsFromFile(std::vector<PreparationActionData>& actions, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл действий подготовки: " << filename << std::endl;
        return false;
    }
    std::string line, key, value;
    PreparationActionData currentAction;
    bool inActionBlock = false;

    while (std::getline(file, line)) {
        line = trimString(line);
        if (line.empty() || line[0] == '#') continue;

        if (line == "---") {
            if (inActionBlock && !currentAction.menuText.empty()) {
                actions.push_back(currentAction);
            }
            currentAction = PreparationActionData();
            inActionBlock = true;
            continue;
        }
        if (inActionBlock && parseKeyValue(line, key, value)) {
            try {
                if (key == "menuText") currentAction.menuText = value;
                else if (key == "costSteps") currentAction.costSteps = std::stoi(value);
                else if (key == "effectType") currentAction.effectType = value;
                else if (key == "targetStat") currentAction.targetStat = value;
                else if (key == "effectValue") currentAction.effectValue = std::stoi(value);
                else if (key == "maxValue") currentAction.maxValue = std::stoi(value);
                else if (key == "requiresMagicAccess") currentAction.requiresMagicAccess = (value == "true" || value == "1");
            }
            catch (const std::exception& e) {
                std::cerr << "Ошибка парсинга preparation_actions.txt для '" << currentAction.menuText << "', ключ '" << key << "': " << e.what() << std::endl;
            }
        }
    }
    if (inActionBlock && !currentAction.menuText.empty()) {
        actions.push_back(currentAction);
    }
    file.close();
    return true;
}

// Главная функция загрузки
bool loadGameData(GameData& gameData) {
    std::cout << "Загрузка данных игры..." << std::endl;
    bool success = true;

    if (!loadPlayerStatsFromFile(gameData.initialPlayerStats, PLAYER_FILE)) success = false;
    else std::cout << "Характеристики игрока загружены." << std::endl;

    if (!loadMonstersFromFile(gameData.monsters, MONSTERS_FILE)) success = false;
    else std::cout << "Монстры загружены: " << gameData.monsters.size() << std::endl;

    if (!loadItemsFromFile(gameData.items, ITEMS_FILE)) success = false;
    else std::cout << "Предметы загружены: " << gameData.items.size() << std::endl;

    if (!loadSpellsFromFile(gameData.spells, SPELLS_FILE)) success = false;
    else std::cout << "Заклинания загружены: " << gameData.spells.size() << std::endl;

    if (!loadBooksFromFile(gameData.bookContents, BOOKS_FILE)) success = false;
    else std::cout << "Книги загружены: " << gameData.bookContents.size() << std::endl;

    if (!loadPreparationActionsFromFile(gameData.preparationActions, PREPARATION_ACTIONS_FILE)) success = false;
    else std::cout << "Действия подготовки загружены: " << gameData.preparationActions.size() << std::endl;

    if (success) std::cout << "Все данные успешно загружены." << std::endl;
    else std::cerr << "Во время загрузки данных произошли ошибки." << std::endl;

    return success;
}