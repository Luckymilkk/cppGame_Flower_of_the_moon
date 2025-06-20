#include "DataLoader.h"
#include "../utils.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace data {

bool DataLoader::loadItems(const std::string& filename, domain::ItemManager& manager) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл предметов: " << filename << std::endl;
        return false;
    }

    try {
        domain::Item currentItem;
        std::string line;
        while (std::getline(file, line)) {
            line = TrimString(line);
            if (line.empty() || line[0] == '#') continue;

            if (line == "---") {
                if (!currentItem.name.empty()) {
                    manager.AddItem(currentItem);
                    currentItem = domain::Item();
                }
                continue;
            }

            auto delimiterPos = line.find(":");
            if (delimiterPos == std::string::npos) continue;
            
            std::string key = TrimString(line.substr(0, delimiterPos));
            std::string value = TrimString(line.substr(delimiterPos + 1));

            if (key == "name") currentItem.name = value;
            else if (key == "description") currentItem.description = value;
            else if (key == "id") currentItem.id = value;
            else if (key == "type") currentItem.type = value;
            else if (key == "prepEffectType") currentItem.prepEffectType = value;
            else if (key == "bookKey") currentItem.bookKey = value;
            else if (key == "battleActionName") currentItem.battleActionName = value;
            else if (key == "checkStat") currentItem.checkStat = value;
            else if (key == "costSteps") currentItem.costSteps = std::stoi(value);
            else if (key == "costSlots") currentItem.costSlots = std::stoi(value);
            else if (key == "prepEffectValue") currentItem.prepEffectValue = std::stoi(value);
            else if (key == "damage") currentItem.damage = std::stoi(value);
            else if (key == "usesPerBattle") currentItem.usesPerBattle = std::stoi(value);
            else if (key == "requiresMagicAccess") currentItem.requiresMagicAccessToUse = (value == "true" || value == "1");
        }

        if (!currentItem.name.empty()) {
            manager.AddItem(currentItem);
        }

        file.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке предметов: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool DataLoader::loadMonsters(const std::string& filename, domain::MonsterManager& manager) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл монстров: " << filename << std::endl;
        return false;
    }

    try {
        domain::Monster currentMonster;
        std::string line;
        while (std::getline(file, line)) {
            line = TrimString(line);
            if (line.empty() || line[0] == '#') continue;

            if (line == "---") {
                if (!currentMonster.name.empty()) {
                    manager.AddMonster(currentMonster);
                    currentMonster = domain::Monster();
                }
                continue;
            }

            auto delimiterPos = line.find(":");
            if (delimiterPos == std::string::npos) continue;
            
            std::string key = TrimString(line.substr(0, delimiterPos));
            std::string value = TrimString(line.substr(delimiterPos + 1));

            if (key == "name") currentMonster.name = value;
            else if (key == "health") currentMonster.health = std::stoi(value);
            else if (key == "attack") currentMonster.attack = std::stoi(value);
            else if (key == "defense") currentMonster.defense = std::stoi(value);
            else if (key == "bookKey") currentMonster.bookKey = value;
            else if (key == "favorableActions") {
                std::istringstream ss(value);
                std::string action;
                while (std::getline(ss, action, ',')) {
                    currentMonster.favorableActions.push_back(TrimString(action));
                }
            }
            else if (key == "unfavorableActions") {
                std::istringstream ss(value);
                std::string action;
                while (std::getline(ss, action, ',')) {
                    currentMonster.unfavorableActions.push_back(TrimString(action));
                }
            }
            else if (key == "neutralActions") {
                std::istringstream ss(value);
                std::string action;
                while (std::getline(ss, action, ',')) {
                    if (!action.empty()) {
                        currentMonster.neutralActions.push_back(TrimString(action));
                    }
                }
            }
        }

        if (!currentMonster.name.empty()) {
            manager.AddMonster(currentMonster);
        }

        file.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке монстров: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool DataLoader::loadSpells(const std::string& filename, domain::SpellManager& manager) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл заклинаний: " << filename << std::endl;
        return false;
    }

    try {
        domain::Spell currentSpell;
        std::string line;
        while (std::getline(file, line)) {
            line = TrimString(line);
            if (line.empty() || line[0] == '#') continue;

            if (line == "---") {
                if (!currentSpell.name.empty()) {
                    manager.AddSpell(currentSpell);
                    currentSpell = domain::Spell();
                }
                continue;
            }

            auto delimiterPos = line.find(":");
            if (delimiterPos == std::string::npos) continue;
            
            std::string key = TrimString(line.substr(0, delimiterPos));
            std::string value = TrimString(line.substr(delimiterPos + 1));

            if (key == "name") currentSpell.name = value;
            else if (key == "id") currentSpell.id = value;
            else if (key == "description") currentSpell.description = value;
            else if (key == "type") currentSpell.type = value;
            else if (key == "costSteps") currentSpell.costSteps = std::stoi(value);
            else if (key == "requiresMagicAccessToLearn") currentSpell.requiresMagicAccessToLearn = (value == "true" || value == "1");
            else if (key == "manaCost") currentSpell.manaCost = std::stoi(value);
        }

        if (!currentSpell.name.empty()) {
            manager.AddSpell(currentSpell);
        }

        file.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке заклинаний: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool DataLoader::loadBooks(const std::string& filename, domain::BookManager& manager) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл книг: " << filename << std::endl;
        return false;
    }

    try {
        domain::Book currentBook;
        std::string line;
        while (std::getline(file, line)) {
            line = TrimString(line);
            if (line.empty() || line[0] == '#') continue;

            if (line == "---") {
                if (!currentBook.title.empty()) {
                    manager.AddBook(currentBook);
                    currentBook = domain::Book();
                }
                continue;
            }

            auto delimiterPos = line.find(":");
            if (delimiterPos == std::string::npos) continue;
            
            std::string key = TrimString(line.substr(0, delimiterPos));
            std::string value = TrimString(line.substr(delimiterPos + 1));

            if (key == "title") currentBook.title = value;
            else if (key == "id") currentBook.id = value;
            else if (key == "content") currentBook.content = value;
        }

        if (!currentBook.title.empty()) {
            manager.AddBook(currentBook);
        }

        file.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке книг: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool DataLoader::loadActions(const std::string& filename, domain::ActionManager& manager) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл действий: " << filename << std::endl;
        return false;
    }

    try {
        domain::Action currentAction;
        std::string line;
        while (std::getline(file, line)) {
            line = TrimString(line);
            if (line.empty() || line[0] == '#') continue;

            if (line == "---") {
                if (!currentAction.name.empty()) {
                    manager.AddAction(currentAction);
                    currentAction = domain::Action();
                }
                continue;
            }

            auto delimiterPos = line.find(":");
            if (delimiterPos == std::string::npos) continue;
            
            std::string key = TrimString(line.substr(0, delimiterPos));
            std::string value = TrimString(line.substr(delimiterPos + 1));

            if (key == "name") currentAction.name = value;
            else if (key == "id") currentAction.id = value;
            else if (key == "description") currentAction.description = value;
            else if (key == "menuText") currentAction.menuText = value;
            else if (key == "effectType") currentAction.effectType = value;
            else if (key == "targetStat") currentAction.targetStat = value;
            else if (key == "effectValue") currentAction.effectValue = std::stoi(value);
            else if (key == "maxValue") currentAction.maxValue = std::stoi(value);
            else if (key == "costSteps") currentAction.costSteps = std::stoi(value);
            else if (key == "requiresMagicAccess") currentAction.requiresMagicAccess = (value == "true" || value == "1");
            else if (key == "bookKey") currentAction.bookKey = value;
            else if (key == "requiresBook") currentAction.requiresBook = value;
        }

        if (!currentAction.name.empty()) {
            manager.AddAction(currentAction);
        }

        file.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке действий: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

bool DataLoader::loadPlayer(const std::string& filename, domain::Player& player) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл игрока: " << filename << std::endl;
        return false;
    }

    try {
        std::string line;
        while (std::getline(file, line)) {
            line = TrimString(line);
            if (line.empty() || line[0] == '#') continue;

            auto delimiterPos = line.find(":");
            if (delimiterPos == std::string::npos) continue;
            
            std::string key = TrimString(line.substr(0, delimiterPos));
            std::string value = TrimString(line.substr(delimiterPos + 1));

            if (key == "name") player.name = value;
            else if (key == "health") player.health = std::stoi(value);
            else if (key == "max_health") player.max_health = std::stoi(value);
            else if (key == "mana") player.mana = std::stoi(value);
            else if (key == "magic_reserve") player.magic_reserve = std::stoi(value);
            else if (key == "max_magic_reserve") player.max_magic_reserve = std::stoi(value);
            else if (key == "experience") player.experience = std::stoi(value);
            else if (key == "level") player.level = std::stoi(value);
            else if (key == "steps") player.steps = std::stoi(value);
            else if (key == "inventory_slots") player.inventory_slots = std::stoi(value);
            else if (key == "attempts") player.attempts = std::stoi(value);
            else if (key == "magic_access") player.magic_access = (value == "true" || value == "1");
            else if (key == "agility") player.agility = std::stoi(value);
            else if (key == "accuracy") player.accuracy = std::stoi(value);
            else if (key == "stamina") player.stamina = std::stoi(value);
            else if (key == "intelligence") player.intelligence = std::stoi(value);
            else if (key == "combat_experience") player.combat_experience = std::stoi(value);
            else if (key == "magic_control") player.magic_control = std::stoi(value);
            else if (key == "magic_experience") player.magic_experience = std::stoi(value);
            else if (key == "psychic_stability") player.psychic_stability = std::stoi(value);
            else if (key == "inventory") {
                std::istringstream ss(value);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    // ... существующий код ...
                }
            }
        }

        file.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке данных игрока: " << e.what() << std::endl;
        file.close();
        return false;
    }
}

} // namespace data 