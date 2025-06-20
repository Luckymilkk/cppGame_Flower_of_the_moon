#include "GameEngine.h"
#include "../data/DataLoader.h"
#include "../config.h"
#include <iostream>
#include "../utils.h"
#include <functional>
#include <random>
#include <cstdlib>
#include <algorithm>

namespace engine {

GameEngine::GameEngine() {
    player = std::make_unique<domain::Player>();
    
    // Загрузка данных из файлов с проверкой
    try {
        if (!data::DataLoader::loadPlayer(PLAYER_FILE, *player)) {
            throw std::runtime_error("Ошибка загрузки данных игрока");
        }
        if (!data::DataLoader::loadItems(ITEMS_FILE, itemManager)) {
            throw std::runtime_error("Ошибка загрузки предметов");
        }
        if (!data::DataLoader::loadMonsters(MONSTERS_FILE, monsterManager)) {
            throw std::runtime_error("Ошибка загрузки монстров");
        }
        if (!data::DataLoader::loadSpells(SPELLS_FILE, spellManager)) {
            throw std::runtime_error("Ошибка загрузки заклинаний");
        }
        if (!data::DataLoader::loadBooks(BOOKS_FILE, bookManager)) {
            throw std::runtime_error("Ошибка загрузки книг");
        }
        if (!data::DataLoader::loadActions(PREPARATION_ACTIONS_FILE, actionManager)) {
            throw std::runtime_error("Ошибка загрузки действий подготовки");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка инициализации игры: " << e.what() << std::endl;
        throw; // Пробрасываем исключение дальше
    }
}

void GameEngine::run() {
    std::cout << "Добро пожаловать в игру!" << std::endl;
    preparationPhase();
    battlePhase();
    showFinalStats();
}

void GameEngine::preparationPhase() {
    std::cout << "\n--- ФАЗА ПОДГОТОВКИ ---" << std::endl;
    while (player->steps > 0) {
        // Отображаем текущее состояние
        player->displayStats(itemManager);
        int current_used_slots = player->getUsedInventorySlots(itemManager);
        std::cout << "Занято слотов: " << current_used_slots << "/" << player->inventorySlots << std::endl;
        std::cout << "Осталось шагов: " << player->steps << std::endl;
        std::cout << "Выберите действие:" << std::endl;
        
        int menuOption = 1;
        std::vector<std::function<void(bool&)>> availableMenuActions;

        // 1. Взять предметы
        for (const auto& item : itemManager.getAllItems()) {
            if (player->hasItem(item.id)) continue;

            std::cout << menuOption << ". Взять '" << item.name << "' (Шаги: " << item.costSteps;
            if (item.costSlots > 0) std::cout << ", Слоты: " << item.costSlots;
            if (item.type == "preparation_item" && item.prepEffectType == "add_slots") {
                std::cout << ", Эффект: +" << item.prepEffectValue << " слота инвентаря";
            }
            std::cout << ")" << std::endl;

            availableMenuActions.push_back([this, item, current_used_slots](bool& actionPerformed) {
                if (player->steps >= item.costSteps) {
                    if (item.type == "preparation_item" && item.prepEffectType == "add_slots") {
                        player->inventorySlots += item.prepEffectValue;
                        player->addItem(item.id);
                        std::cout << "'" << item.name << "' взята. Слоты инвентаря увеличены до " << player->inventorySlots << "." << std::endl;
                        actionPerformed = true;
                    }
                    else { // Оружие или книга
                        if (current_used_slots + item.costSlots <= player->inventorySlots) {
                            player->addItem(item.id);
                            std::cout << "'" << item.name << "' добавлен в инвентарь." << std::endl;
                            if (item.type == "book") {
                                if (!item.bookKey.empty()) {
                                    std::cout << "Теперь вы можете прочитать книгу (это действие бесплатно)." << std::endl;
                                } else {
                                    std::cout << "Ошибка: у книги '" << item.name << "' не задан bookKey!" << std::endl;
                                }
                            }
                            actionPerformed = true;
                        }
                        else {
                            std::cout << "Недостаточно места в инвентаре!" << std::endl;
                        }
                    }
                    if (actionPerformed) {
                        player->steps -= item.costSteps;
                        std::cout << "\nОбновленная информация:" << std::endl;
                        player->displayStats(itemManager);
                        int updated_slots = player->getUsedInventorySlots(itemManager);
                        std::cout << "Занято слотов: " << updated_slots << "/" << player->inventorySlots << std::endl;
                        std::cout << "Осталось шагов: " << player->steps << std::endl;
                    }
                }
                else {
                    std::cout << "Недостаточно шагов!" << std::endl;
                }
            });
            ++menuOption;
        }

        // 2. Изучить заклинания
        for (const auto& spell : spellManager.getAllSpells()) {
            if (spell.type == "preparation_spell" && !player->knowsSpell(spell.name)) {
                if (spell.requiresMagicAccessToLearn && !player->magicAccess) continue;

                std::cout << menuOption << ". Освоить магию '" << spell.name << "' (Шаги: " << spell.costSteps << ")" << std::endl;
                availableMenuActions.push_back([this, spell](bool& actionPerformed) {
                    if (player->steps >= spell.costSteps) {
                        player->knownSpells.push_back(spell.name);
                        player->steps -= spell.costSteps;
                        std::cout << "Магия '" << spell.name << "' освоена." << std::endl;
                        actionPerformed = true;
                        
                        std::cout << "\nОбновленная информация:" << std::endl;
                        player->displayStats(itemManager);
                        int updated_slots = player->getUsedInventorySlots(itemManager);
                        std::cout << "Занято слотов: " << updated_slots << "/" << player->inventorySlots << std::endl;
                        std::cout << "Осталось шагов: " << player->steps << std::endl;
                    }
                    else {
                        std::cout << "Недостаточно шагов!" << std::endl;
                    }
                });
                ++menuOption;
            }
        }

        // 3. Подготовительные действия
        for (const auto& action : actionManager.getAllActions()) {
            if (action.requiresMagicAccess && !player->magicAccess) continue;
            if (action.effectType == "enable_magic_access" && player->magicAccess) continue;

            // Пропускаем действия чтения книг, если у игрока нет соответствующей книги
            if (!action.requiresBook.empty() && !player->hasItem(action.requiresBook)) continue;
            // Пропускаем действия чтения книг, если книга уже прочитана
            if (!action.bookKey.empty() && player->booksActuallyRead[action.bookKey]) continue;

            if (action.effectType == "increase_stat") {
                int currentStat = player->getStatValue(action.targetStat);
                if (currentStat >= action.maxValue) continue;
            }

            std::cout << menuOption << ". " << action.menuText << " (Шаги: " << action.costSteps << ")" << std::endl;
            availableMenuActions.push_back([this, action](bool& actionPerformed) {
                if (player->steps >= action.costSteps) {
                    bool successInAction = false;
                    if (action.effectType == "increase_stat") {
                        player->modifyStat(action.targetStat, action.effectValue, action.maxValue);
                        successInAction = true;
                    }
                    else if (action.effectType == "enable_magic_access") {
                        if (!player->magicAccess) {
                            player->magicAccess = true;
                            std::cout << "Доступ к магии восстановлен!" << std::endl;
                            successInAction = true;
                        }
                        else {
                            std::cout << "Доступ к магии уже есть." << std::endl;
                        }
                    }
                    else if (action.effectType == "read_book") {
                        // Отладочная информация
                        std::cout << "Проверка чтения книги:" << std::endl;
                        std::cout << "requiresBook: " << action.requiresBook << std::endl;
                        std::cout << "bookKey: " << action.bookKey << std::endl;
                        std::cout << "Книга в инвентаре: " << (player->hasItem(action.requiresBook) ? "да" : "нет") << std::endl;
                        std::cout << "Книга уже прочитана: " << (player->booksActuallyRead[action.bookKey] ? "да" : "нет") << std::endl;

                        // Проверяем, есть ли у игрока нужная книга
                        if (!action.requiresBook.empty() && player->hasItem(action.requiresBook)) {
                            // Проверяем, не прочитана ли уже книга
                            if (!action.bookKey.empty() && !player->booksActuallyRead[action.bookKey]) {
                                player->booksActuallyRead[action.bookKey] = true;
                                // Используем requiresBook вместо bookKey для поиска книги
                                const auto* book = bookManager.getBook(action.requiresBook);
                                if (book) {
                                    std::cout << "\n=== " << book->title << " ===" << std::endl;
                                    std::cout << book->content << std::endl;
                                    
                                    // Устанавливаем флаг успешного выполнения сразу после вывода содержимого книги
                                    successInAction = true;
                                    
                                    // Поиск монстра по bookKey
                                    for (const auto& monster : monsterManager.getAllMonsters()) {
                                        if (monster.bookKey == action.bookKey) {
                                            std::cout << "\nУдачные действия против монстра:";
                                            for (const auto& action : monster.favorableActions) {
                                                std::cout << " " << action;
                                            }
                                            
                                            std::cout << "\nПровальные действия против монстра:";
                                            for (const auto& action : monster.unfavorableActions) {
                                                std::cout << " " << action;
                                            }
                                            
                                            if (!monster.neutralActions.empty()) {
                                                std::cout << "\nНейтральные действия против монстра:";
                                                for (const auto& action : monster.neutralActions) {
                                                    std::cout << " " << action;
                                                }
                                            }
                                            std::cout << std::endl;
                                            break;
                                        }
                                    }
                                    
                                    std::cout << "====================\n" << std::endl;
                                } else {
                                    std::cout << "Ошибка: книга не найдена в базе данных." << std::endl;
                                }
                            } else {
                                std::cout << "Вы уже прочитали эту книгу." << std::endl;
                            }
                        } else {
                            std::cout << "У вас нет нужной книги для этого действия." << std::endl;
                        }
                    }
                    if (successInAction) {
                        player->steps -= action.costSteps;
                        actionPerformed = true;
                        
                        std::cout << "\nОбновленная информация:" << std::endl;
                        player->displayStats(itemManager);
                        int updated_slots = player->getUsedInventorySlots(itemManager);
                        std::cout << "Занято слотов: " << updated_slots << "/" << player->inventorySlots << std::endl;
                        std::cout << "Осталось шагов: " << player->steps << std::endl;
                    }
                }
                else {
                    std::cout << "Недостаточно шагов!" << std::endl;
                }
            });
            ++menuOption;
        }

        std::cout << "0. Закончить подготовку досрочно" << std::endl;
        int choice = getIntInput("Ваш выбор (введите номер): ", 0, menuOption - 1);
        if (choice == 0) {
            std::cout << "Подготовка завершена досрочно." << std::endl;
            break;
        }
        if (choice > 0 && choice <= availableMenuActions.size()) {
            bool actionWasPerformed = false;
            availableMenuActions[choice - 1](actionWasPerformed);
            if (!actionWasPerformed) {
                std::cout << "Действие не было выполнено." << std::endl;
            }
            std::cout << "\nНажмите Enter для продолжения...";
            std::cin.get();
        } else {
            std::cout << "Неверный выбор." << std::endl;
        }
    }
    if (player->steps <= 0) {
        std::cout << "\nШаги для подготовки закончились!" << std::endl;
    }
    std::cout << "--- ПОДГОТОВКА ЗАВЕРШЕНА ---" << std::endl;
}

void GameEngine::battlePhase() {
    std::cout << "\n--- ФАЗА БОЯ ---" << std::endl;
    std::cout << "Чтобы выиграть, вам нужно победить 3 монстра!" << std::endl;
    int monstersDefeated = 0;
    player->rifleUsesPerBattle = 0; // Сбрасываем счетчик использования ружья

    for (auto& monster : monsterManager.getAllMonsters()) {
        if (monstersDefeated >= 3) break;
        
        std::cout << "\nВас атакует монстр: " << monster.name << "!" << std::endl;

        // Проверяем, есть ли у игрока книга об этом монстре
        if (!monster.bookKey.empty()) {
            for (const auto& book : bookManager.getAllBooks()) {
                if (book.id == monster.bookKey && player->hasItem(book.id)) {
                    std::cout << "\nВы знаете характеристики этого монстра из книги:" << std::endl;
                    std::cout << "Здоровье: " << monster.health << std::endl;
                    std::cout << "Урон: " << monster.attack << std::endl;
                    std::cout << "Защита: " << monster.defense << std::endl;
                    
                    std::cout << "\nУдачные действия:";
                    for (const auto& action : monster.favorableActions) {
                        std::cout << " " << action;
                    }
                    
                    std::cout << "\nПровальные действия:";
                    for (const auto& action : monster.unfavorableActions) {
                        std::cout << " " << action;
                    }
                    
                    if (!monster.neutralActions.empty()) {
                        std::cout << "\nНейтральные действия:";
                        for (const auto& action : monster.neutralActions) {
                            std::cout << " " << action;
                        }
                    }
                    std::cout << std::endl;
                    break;
                }
            }
        }

        int lastChoice = 0; // Переменная для хранения последнего выбора
        std::string lastAction; // Переменная для хранения названия последнего действия
        
        while (monster.isAlive() && player->health > 0) {
            std::cout << "\nВаше здоровье: " << player->health << ", Здоровье монстра: " << monster.health << std::endl;
            std::cout << "Доступные действия:" << std::endl;
            std::cout << "1. Удар рукой (проверка: опыт в боях)" << std::endl;
            std::cout << "2. Бегство (проверка: выносливость)" << std::endl;
            std::cout << "3. Маскировка (проверка: ум)" << std::endl;
            
            if (player->hasItem("sword"))
                std::cout << "4. Удар мечом (проверка: ловкость)" << std::endl;
            
            if (player->hasItem("bow") && player->hasItem("arrows"))
                std::cout << "5. Выстрел из лука (проверка: меткость)" << std::endl;
            
            if (player->hasItem("rifle") && player->rifleUsesPerBattle < 2)
                std::cout << "6. Выстрел из ружья (проверка: меткость)" << std::endl;
            
            if (player->hasItem("whip"))
                std::cout << "7. Удар кнутом (проверка: меткость)" << std::endl;
            
            if (player->hasItem("knife"))
                std::cout << "8. Удар ножом (проверка: опыт в боях)" << std::endl;
            
            if (player->hasItem("axe"))
                std::cout << "9. Удар топором (проверка: ловкость)" << std::endl;
            
            if (player->knowsSpell("световой луч") && player->magicReserve > 0)
                std::cout << "10. Магический луч (проверка: контроль магии)" << std::endl;
            
            if (player->knowsSpell("ослепление") && player->magicReserve > 0)
                std::cout << "11. Ослепление (проверка: опыт магии)" << std::endl;
            
            if (player->knowsSpell("приручение") && player->magicReserve > 0)
                std::cout << "12. Приручение (проверка: психическая стабильность)" << std::endl;
            
            std::cout << "0. Сдаться" << std::endl;

            lastChoice = getIntInput("Ваш выбор: ", 0, 12);
            bool actionSuccess = false;
            float actionValue = 0.0f;
            float actionCoefficient = 1.0f; // По умолчанию действие нейтральное

            // Определяем название действия
            switch (lastChoice) {
                case 1: lastAction = "удар рукой"; break;
                case 2: lastAction = "бегство"; break;
                case 3: lastAction = "маскировка"; break;
                case 4: lastAction = "удар мечом"; break;
                case 5: lastAction = "выстрел из лука"; break;
                case 6: lastAction = "выстрел из ружья"; break;
                case 7: lastAction = "удар кнутом"; break;
                case 8: lastAction = "удар ножом"; break;
                case 9: lastAction = "удар топором"; break;
                case 10: lastAction = "магический луч"; break;
                case 11: lastAction = "ослепление"; break;
                case 12: lastAction = "приручение"; break;
                case 0: 
                    std::cout << "Вы сдались!" << std::endl;
                    player->health = 0;
                    return;
            }

            // Определяем коэффициент действия
            if (std::find(monster.favorableActions.begin(), monster.favorableActions.end(), lastAction) != monster.favorableActions.end()) {
                actionCoefficient = 2.0f;
            } else if (std::find(monster.unfavorableActions.begin(), monster.unfavorableActions.end(), lastAction) != monster.unfavorableActions.end()) {
                actionCoefficient = 0.5f;
            }

            switch (lastChoice) {
                case 1: // Удар рукой
                    actionValue = actionCoefficient * player->combatExperience;
                    if (actionValue > 1.5f) {
                        monster.takeDamage(10);
                        std::cout << "Вы успешно ударили монстра!" << std::endl;
                        actionSuccess = true;
                    }
                    break;

                case 2: // Бегство
                    actionValue = actionCoefficient * player->stamina;
                    if (actionValue > 1.5f) {
                        std::cout << "Вы успешно сбежали от монстра!" << std::endl;
                        monster.health = 0;
                        monstersDefeated++;
                        actionSuccess = true;
                    }
                    break;

                case 3: // Маскировка
                    actionValue = actionCoefficient * player->intelligence;
                    if (actionValue > 1.5f) {
                        std::cout << "Вы успешно обманули монстра!" << std::endl;
                        monster.health = 0;
                        monstersDefeated++;
                        actionSuccess = true;
                    }
                    break;

                case 4: // Удар мечом
                    if (player->hasItem("sword")) {
                        actionValue = actionCoefficient * player->agility;
                        if (actionValue > 1.5f) {
                            monster.takeDamage(40);
                            std::cout << "Вы успешно атаковали мечом!" << std::endl;
                            actionSuccess = true;
                        }
                    }
                    break;

                case 5: // Выстрел из лука
                    if (player->hasItem("bow") && player->hasItem("arrows")) {
                        actionValue = actionCoefficient * player->accuracy;
                        if (actionValue > 1.5f) {
                            monster.takeDamage(50);
                            std::cout << "Вы успешно выстрелили из лука!" << std::endl;
                            actionSuccess = true;
                        }
                    }
                    break;

                case 6: // Выстрел из ружья
                    if (player->hasItem("rifle") && player->rifleUsesPerBattle < 2) {
                        actionValue = actionCoefficient * player->accuracy;
                        if (actionValue > 1.5f) {
                            monster.takeDamage(40);
                            std::cout << "Вы успешно выстрелили из ружья!" << std::endl;
                            actionSuccess = true;
                        }
                        player->rifleUsesPerBattle++;
                    }
                    break;

                case 7: // Удар кнутом
                    if (player->hasItem("whip")) {
                        actionValue = actionCoefficient * player->accuracy;
                        if (actionValue > 1.5f) {
                            monster.takeDamage(40);
                            std::cout << "Вы успешно атаковали кнутом!" << std::endl;
                            actionSuccess = true;
                        }
                    }
                    break;

                case 8: // Удар ножом
                    if (player->hasItem("knife")) {
                        actionValue = actionCoefficient * player->combatExperience;
                        if (actionValue > 1.5f) {
                            monster.takeDamage(15);
                            std::cout << "Вы успешно атаковали ножом!" << std::endl;
                            actionSuccess = true;
                        }
                    }
                    break;

                case 9: // Удар топором
                    if (player->hasItem("axe")) {
                        actionValue = actionCoefficient * player->agility;
                        if (actionValue > 1.5f) {
                            monster.takeDamage(35);
                            std::cout << "Вы успешно атаковали топором!" << std::endl;
                            actionSuccess = true;
                        }
                    }
                    break;

                case 10: // Магический луч
                    if (player->knowsSpell("световой луч") && player->magicReserve > 0) {
                        actionValue = actionCoefficient * player->magicControl;
                        if (actionValue > 1.5f) {
                            monster.takeDamage(80);
                            std::cout << "Вы успешно использовали магический луч!" << std::endl;
                            actionSuccess = true;
                        }
                        player->magicReserve--;
                    }
                    break;

                case 11: // Ослепление
                    if (player->knowsSpell("ослепление") && player->magicReserve > 0) {
                        actionValue = actionCoefficient * player->magicExperience;
                        if (actionValue > 1.5f) {
                            std::cout << "Вы успешно ослепили монстра!" << std::endl;
                            monster.health = 0;
                            monstersDefeated++;
                            actionSuccess = true;
                        }
                        player->magicReserve--;
                    }
                    break;

                case 12: // Приручение
                    if (player->knowsSpell("приручение") && player->magicReserve > 0) {
                        actionValue = actionCoefficient * player->psychicStability;
                        if (actionValue > 1.5f) {
                            std::cout << "Вы успешно приручили монстра!" << std::endl;
                            monster.health = 0;
                            monstersDefeated++;
                            actionSuccess = true;
                        }
                        player->magicReserve--;
                    }
                    break;
            }

            if (actionValue == 1.5f) {
                std::cout << "Действие нейтральное!" << std::endl;
                if (monster.isAlive()) {
                    player->takeDamage(1);
                    std::cout << "Монстр атакует! Вы получаете 1 урона." << std::endl;
                }
            } else if (!actionSuccess && lastChoice != 0) {
                std::cout << "Действие провалено!" << std::endl;
                player->takeDamage(1);
                std::cout << "Монстр атакует! Вы получаете 1 урона." << std::endl;
            } else if (actionSuccess) {
                std::cout << "Действие успешно! Монстр пропускает ход." << std::endl;
            }
        }

        if (player->health <= 0) {
            std::cout << "Вы проиграли бой!" << std::endl;
            return;
        } else if (monster.health <= 0) {
            std::cout << "Монстр повержен!" << std::endl;
            if (lastChoice != 2 && lastChoice != 3 && lastChoice != 11 && lastChoice != 12) {
                monstersDefeated++;
            }
        }
    }

    if (monstersDefeated >= 3) {
        std::cout << "\nПоздравляем! Вы победили необходимое количество монстров!" << std::endl;
    } else {
        std::cout << "\nВы не смогли победить необходимое количество монстров." << std::endl;
    }
}

void GameEngine::showFinalStats() {
    std::cout << "\n--- Финальные характеристики игрока ---" << std::endl;
    player->displayStats(itemManager);
    std::cout << "Спасибо за игру!" << std::endl;
}

} // namespace engine 