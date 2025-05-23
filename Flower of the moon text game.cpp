#define NOMINMAX
#include <iostream>
#include <cctype>
#include <vector>
#include <string>
#include <algorithm> 
#include <limits>    
#include <map>       
#include <iomanip>   
#include <cmath>     
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

bool hasItem(const std::vector<std::string>& inventory, const std::string& item_name) {
    return std::find(inventory.begin(), inventory.end(), item_name) != inventory.end();
}

bool knowsSpell(const std::vector<std::string>& known_spells, const std::string& spell_name) {
    return std::find(known_spells.begin(), known_spells.end(), spell_name) != known_spells.end();
}

int getIntInput(int minVal = std::numeric_limits<int>::min(), int maxVal = std::numeric_limits<int>::max()) {
    int choice;
    while (true) {
        std::cin >> choice;
        if (std::cin.fail()|| choice < minVal || choice > maxVal) {
            std::cout << "Неверный ввод. Пожалуйста, введите число"
                << (minVal != std::numeric_limits<int>::min() ? " от " + std::to_string(minVal) : "")
                << (maxVal != std::numeric_limits<int>::max() ? " до " + std::to_string(maxVal) : "")
                << ": ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка буфера
            return choice;
        }
    }
}

struct Player {
    int hp;
    int maxHp;
    int baseDamage; 
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

    Player() { 
        hp = 5;
        maxHp = 5;
        baseDamage = 20;
        steps = 15;
        inventorySlots = 5;
        attempts = 3;

        agility = 2;
        accuracy = 0;
        stamina = 1;
        intelligence = 1;
        combatExperience = 2;
        magicControl = 2;
        magicExperience = 0;
        psychicStability = 0;

        magicAccess = false;
        magicReserve = 2;
        maxMagicReserve = 2;

        rifleUsesPerBattle = 0; 
       
    }

    void displayStats() const {
        std::cout << "\n--- Характеристики Героя ---" << std::endl;
        std::cout << "Здоровье: " << hp << "/" << maxHp << std::endl;
        std::cout << "Шаги для подготовки: " << steps << std::endl;
        std::cout << "Слоты инвентаря: " << inventory.size() << "/" << inventorySlots << std::endl;
        std::cout << "Попытки: " << attempts << std::endl;
        std::cout << "Ловкость: " << agility << "/3" << std::endl;
        std::cout << "Меткость: " << accuracy << "/3" << std::endl;
        std::cout << "Выносливость: " << stamina << "/3" << std::endl;
        std::cout << "Ум: " << intelligence << "/3" << std::endl;
        std::cout << "Боевой опыт: " << combatExperience << "/3" << std::endl;
        std::cout << "Доступ к магии: " << (magicAccess ? "Есть" : "Нет") << std::endl;
        if (magicAccess) {
            std::cout << "Запас магии: " << magicReserve << "/" << maxMagicReserve << std::endl;
            std::cout << "Контроль магии: " << magicControl << "/3" << std::endl;
            std::cout << "Магический опыт: " << magicExperience << "/3" << std::endl;
            std::cout << "Психическая стабильность: " << psychicStability << "/3" << std::endl;
        }
        std::cout << "Инвентарь: ";
        if (inventory.empty()) {
            std::cout << "пусто";
        }
        else {
            for (size_t i = 0; i < inventory.size(); ++i) {
                std::cout << inventory[i] << (i == inventory.size() - 1 ? "" : ", ");
                auto bookIt = booksActuallyRead.find(inventory[i]);
                if (bookIt != booksActuallyRead.end() && bookIt->second) {
                    std::cout << " (прочитана)";
                }
            }
        }
        std::cout << std::endl;
        std::cout << "Изученные заклинания: ";
        if (knownSpells.empty()) {
            std::cout << "нет";
        }
        else {
            for (size_t i = 0; i < knownSpells.size(); ++i) {
                std::cout << knownSpells[i] << (i == knownSpells.size() - 1 ? "" : ", ");
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

    void resetForNewAttempt() {
        hp = maxHp;
        magicReserve = maxMagicReserve;
    }

    bool hasSpaceForItem(int itemSlots) const {
        return inventory.size() + itemSlots <= inventorySlots;
    }
};

struct Monster {
    std::string name;
    int hp;
    int maxHp;
    int damageToPlayer;
    std::string bookKey; 
    std::vector<std::string> favorableActions;
    std::vector<std::string> unfavorableActions;
    std::vector<std::string> neutralActions;

    Monster(std::string n, int h, int dmg, std::string bk_key,
        std::vector<std::string> fav, std::vector<std::string> unfav,
        std::vector<std::string> neutr = {})
        : name(n), hp(h), maxHp(h), damageToPlayer(dmg), bookKey(bk_key),
        favorableActions(fav), unfavorableActions(unfav), neutralActions(neutr) {
    }

    void displayInfo(const Player& player, const std::map<std::string, std::string>& bookData) const {
        std::cout << "\n--- Противник: " << name << " ---" << std::endl;
        auto it = player.booksActuallyRead.find(bookKey);
        bool bookWasReadByPlayer = (it != player.booksActuallyRead.end() && it->second);

        if (bookWasReadByPlayer) {
            auto desc_it = bookData.find(bookKey);
            if (desc_it != bookData.end()) {
                std::cout << desc_it->second << std::endl;
            }
            std::cout << "Здоровье: " << hp << "/" << maxHp << std::endl;
        }
        else {
            std::cout << "Информация о монстре скрыта (нет соответствующей книги)." << std::endl;
            std::cout << "Здоровье: " << hp << "/" << maxHp << std::endl; 
        }
        std::cout << "--------------------------" << std::endl;
    }

    double getActionCoefficient(const std::string& actionName) const {
        if (std::find(favorableActions.begin(), favorableActions.end(), actionName) != favorableActions.end()) {
            return 2.0;
        }
        if (std::find(unfavorableActions.begin(), unfavorableActions.end(), actionName) != unfavorableActions.end()) {
            return 0.5;
        }
        if (std::find(neutralActions.begin(), neutralActions.end(), actionName) != neutralActions.end()) {
            return 1.0;
        }
        if (name == "Кентавр" && actionName != "ослепление" && 
            std::find(favorableActions.begin(), favorableActions.end(), actionName) == favorableActions.end()) {
            return 0.5; 
        }
        return 1.0; 
    }
};

std::map<std::string, std::string> bookContents = {
    {"книга о волках", "Волк: XP 150. Удачные действия: ослепление, выстрел из ружья, магический луч. Провальные действия: удар рукой, маскировка, бегство."},
    {"книга о кентавре", "Кентавр: XP 450. Урон = 1. Бегство, маскировка, приручение - удачные действия. Ослепление - нейтральное. Все остальные - провальные."},
    {"книга о грифоне", "Грифон: XP 350. Удачные действия: удар мечом, выстрел из лука, выстрел из ружья, ослепление. Провальные: удар рукой, бегство."}
};

void preparationPhase(Player& player) {
    std::cout << "\n--- ФАЗА ПОДГОТОВКИ ---" << std::endl;
    while (player.steps > 0) {
        player.displayStats();
        std::cout << "\nОсталось шагов: " << player.steps << std::endl;
        std::cout << "Выберите действие:" << std::endl;
        std::cout << "1. Взять книгу о монстре (1 шаг, 1 слот)" << std::endl;
        std::cout << "2. Взять сумку (+2 слота, 1 шаг)" << std::endl;
        std::cout << "3. Взять меч (1 шаг, 1 слот)" << std::endl;
        std::cout << "4. Взять топор (1 шаг, 1 слот)" << std::endl;
        std::cout << "5. Взять кнут (1 шаг, 1 слот)" << std::endl;
        std::cout << "6. Взять ружье (1 шаг, 1 слот, 2 выстрела/бой)" << std::endl;
        std::cout << "7. Взять лук и стрелы (2 шага, 2 слота)" << std::endl;
        std::cout << "8. Взять нож (1 шаг, 1 слот)" << std::endl;
        std::cout << "9. Прокачать ловкость (+1, 1 шаг, макс 3)" << std::endl;
        std::cout << "10. Прокачать меткость (+1, 1 шаг, макс 3)" << std::endl;
        std::cout << "11. Прокачать выносливость (+1, 1 шаг, макс 3)" << std::endl;
        std::cout << "12. Прокачать ум (+1, 1 шаг, макс 3)" << std::endl;
        std::cout << "13. Прокачать боевой опыт (+1, 1 шаг, макс 3)" << std::endl;
        std::cout << "14. Восстановить доступ к магии (1 шаг)" << std::endl;
        if (player.magicAccess) {
            std::cout << "15. Освоить магию 'Световой луч' (1 шаг)" << std::endl;
            std::cout << "16. Освоить магию 'Ослепление' (1 шаг)" << std::endl;
            std::cout << "17. Освоить магию 'Приручение' (1 шаг)" << std::endl;
            std::cout << "18. Прокачать контроль магии (+1, 1 шаг, макс 3)" << std::endl;
            std::cout << "19. Прокачать магический опыт (+1, 1 шаг, макс 3)" << std::endl;
            std::cout << "20. Прокачать психическую стабильность (+1, 1 шаг, макс 3)" << std::endl;
        }
        std::cout << "21. Прочитать книгу из инвентаря (0 шагов)";
        std::cout << "0. Закончить подготовку досрочно" << std::endl;
        std::cout << "Ваш выбор: ";
        int choice = getIntInput();

        if (choice == 0) {
            std::cout << "Подготовка завершена досрочно." << std::endl;
            break;
        }

        bool actionTakenThisTurn = false; 

        switch (choice) {
        case 1: { 
            if (player.hasSpaceForItem(1)) {
                std::cout << "Какую книгу взять?" << std::endl;
                int bookIdx = 1;
                std::vector<std::string> bookKeys;
                for (const auto& pair : bookContents) {
                    if (!player.booksActuallyRead[pair.first] && !hasItem(player.inventory, pair.first)) { 
                        std::cout << "  " << bookIdx++ << ". " << pair.first << std::endl;
                        bookKeys.push_back(pair.first);
                    }
                }
                if (bookKeys.empty()) {
                    std::cout << "Все доступные книги уже взяты или прочитаны." << std::endl;
                    continue;
                }
                std::cout << "Ваш выбор (0 для отмены): ";
                int bookChoice = getIntInput(0, bookKeys.size());
                if (bookChoice > 0 && bookChoice <= bookKeys.size()) {
                    std::string bookName = bookKeys[bookChoice - 1];
                    player.inventory.push_back(bookName); 
                    player.booksActuallyRead[bookName] = true;
                    actionTakenThisTurn = true;
                    std::cout << bookName << " добавлена в инвентарь и прочитана." << std::endl;
                }
                else {
                    std::cout << "Отмена взятия книги." << std::endl;
                }
            }
            else {
                std::cout << "Недостаточно места в инвентаре!" << std::endl;
            }
            break;
        }
        case 2: 
            if (!hasItem(player.inventory, "сумка")) {
                player.inventorySlots += 2;
                actionTakenThisTurn = true;
                std::cout << "Сумка взята. +2 слота." << std::endl;
            }
            else {
                std::cout << "У вас уже есть сумка." << std::endl;
            }
            break;
           
        case 3: case 4: case 5: case 6: case 8: {
            std::string itemName;
            int itemSlots = 1;
            if (choice == 3) itemName = "меч";
            if (choice == 4) itemName = "топор";
            if (choice == 5) itemName = "кнут";
            if (choice == 6) itemName = "ружье";
            if (choice == 8) itemName = "нож";

            if (player.hasSpaceForItem(itemSlots)) {
                if (!hasItem(player.inventory, itemName)) {
                    player.inventory.push_back(itemName);
                    actionTakenThisTurn = true;
                    std::cout << itemName << " добавлен в инвентарь." << std::endl;
                }
                else {
                    std::cout << "У вас уже есть " << itemName << "." << std::endl;
                }
            }
            else {
                std::cout << "Недостаточно места в инвентаре!" << std::endl;
            }
            break;
        }
        case 7: { 
            if (player.steps >= 2) {
                if (player.hasSpaceForItem(2)) {
                    if (!hasItem(player.inventory, "лук и стрелы")) {
                        player.inventory.push_back("лук и стрелы");
                        player.steps--; 
                        actionTakenThisTurn = true;
                        std::cout << "Лук и стрелы добавлены в инвентарь." << std::endl;
                    }
                    else {
                        std::cout << "У вас уже есть лук и стрелы." << std::endl;
                    }
                }
                else {
                    std::cout << "Недостаточно места в инвентаре!" << std::endl;
                }
            }
            else {
                std::cout << "Недостаточно шагов для этого действия!" << std::endl;
            }
            break;
        }
          
        case 9:  if (player.agility < 3) { player.agility++; actionTakenThisTurn = true; std::cout << "Ловкость увеличена." << std::endl; }
              else { std::cout << "Ловкость уже максимальна." << std::endl; } break;
        case 10: if (player.accuracy < 3) { player.accuracy++; actionTakenThisTurn = true; std::cout << "Меткость увеличена." << std::endl; }
               else { std::cout << "Меткость уже максимальна." << std::endl; } break;
        case 11: if (player.stamina < 3) { player.stamina++; actionTakenThisTurn = true; std::cout << "Выносливость увеличена." << std::endl; }
               else { std::cout << "Выносливость уже максимальна." << std::endl; } break;
        case 12: if (player.intelligence < 3) { player.intelligence++; actionTakenThisTurn = true; std::cout << "Ум увеличен." << std::endl; }
               else { std::cout << "Ум уже максимален." << std::endl; } break;
        case 13: if (player.combatExperience < 3) { player.combatExperience++; actionTakenThisTurn = true; std::cout << "Боевой опыт увеличен." << std::endl; }
               else { std::cout << "Боевой опыт уже максимален." << std::endl; } break;
        case 14:
            if (!player.magicAccess) {
                player.magicAccess = true;
                actionTakenThisTurn = true;
                std::cout << "Доступ к магии восстановлен!" << std::endl;
            }
            else {
                std::cout << "Доступ к магии уже есть." << std::endl;
            }
            break;
           
        case 15: case 16: case 17: {
            if (player.magicAccess) {
                std::string spellName;
                if (choice == 15) spellName = "Световой луч";
                if (choice == 16) spellName = "Ослепление";
                if (choice == 17) spellName = "Приручение";

                if (!knowsSpell(player.knownSpells, spellName)) {
                    player.knownSpells.push_back(spellName);
                    actionTakenThisTurn = true;
                    std::cout << "Магия '" << spellName << "' освоена." << std::endl;
                }
                else {
                    std::cout << "Вы уже знаете магию '" << spellName << "'." << std::endl;
                }
            }
            else {
                std::cout << "Сначала нужно восстановить доступ к магии (действие 14)." << std::endl;
            }
            break;
        }
        case 18: if (player.magicAccess) { if (player.magicControl < 3) { player.magicControl++; actionTakenThisTurn = true; std::cout << "Контроль магии увеличен." << std::endl; } else { std::cout << "Контроль магии уже максимален." << std::endl; } }
               else { std::cout << "Нет доступа к магии." << std::endl; } break;
        case 19: if (player.magicAccess) { if (player.magicExperience < 3) { player.magicExperience++; actionTakenThisTurn = true; std::cout << "Магический опыт увеличен." << std::endl; } else { std::cout << "Магический опыт уже максимален." << std::endl; } }
               else { std::cout << "Нет доступа к магии." << std::endl; } break;
        case 20: if (player.magicAccess) { if (player.psychicStability < 3) { player.psychicStability++; actionTakenThisTurn = true; std::cout << "Психическая стабильность увеличена." << std::endl; } else { std::cout << "Психическая стабильность уже максимальна." << std::endl; } }
               else { std::cout << "Нет доступа к магии." << std::endl; } break;
        case 21: {
            std::vector<std::string> booksInInventory;
            std::cout << "Какую книгу прочитать?" << std::endl;
            int bookIdx = 1;
            for (const std::string& item : player.inventory) {
                if (bookContents.count(item)) { 
                    std::cout << "  " << bookIdx++ << ". " << item;
                    if (player.booksActuallyRead[item]) std::cout << " (уже читали)";
                    std::cout << std::endl;
                    booksInInventory.push_back(item);
                }
            }
            if (booksInInventory.empty()) {
                std::cout << "У вас нет книг в инвентаре." << std::endl;
                continue; 
            }
            std::cout << "Ваш выбор (0 для отмены): ";
            int readChoice = getIntInput(0, booksInInventory.size());
            if (readChoice > 0 && readChoice <= booksInInventory.size()) {
                std::string bookToRead = booksInInventory[readChoice - 1];
                std::cout << "\n--- Содержимое книги: " << bookToRead << " ---" << std::endl;
                std::cout << bookContents[bookToRead] << std::endl;
                std::cout << "------------------------------------" << std::endl;
                player.booksActuallyRead[bookToRead] = true; 
               
            }
            else {
                std::cout << "Отмена чтения книги." << std::endl;
            }
            break;
        }
        default:
            std::cout << "Неверный выбор." << std::endl;
            break;
        }
        if (actionTakenThisTurn) {
            player.steps--;
        }
    }
    if (player.steps <= 0) {
        std::cout << "\nШаги для подготовки закончились!" << std::endl;
    }
    std::cout << "--- ПОДГОТОВКА ЗАВЕРШЕНА ---" << std::endl;
}
bool runSingleBattle(Player& player, Monster& currentMonster, const std::map<std::string, std::string>& bookData) {
    std::cout << "\n--- НАЧИНАЕТСЯ БОЙ С " << currentMonster.name << "! ---" << std::endl;
    currentMonster.hp = currentMonster.maxHp; 
    if (hasItem(player.inventory, "ружье")) {
        player.rifleUsesPerBattle = 2;
    }
    else {
        player.rifleUsesPerBattle = 0;
    }

    while (player.hp > 0 && currentMonster.hp > 0) {
        currentMonster.displayInfo(player, bookData);
        std::cout << "Ваше здоровье: " << player.hp << "/" << player.maxHp;
        if (player.magicAccess) std::cout << ", Мана: " << player.magicReserve << "/" << player.maxMagicReserve;
        std::cout << std::endl;

        std::cout << "\nВыберите действие в бою:" << std::endl;
        std::cout << "1. Удар рукой (Проверка: Боевой опыт)" << std::endl;
        std::cout << "2. Бегство (Проверка: Выносливость)" << std::endl;
        std::cout << "3. Маскировка (Проверка: Ум)" << std::endl;
        if (hasItem(player.inventory, "меч")) std::cout << "4. Удар мечом (Проверка: Ловкость, Урон: +40)" << std::endl;
        if (hasItem(player.inventory, "лук и стрелы")) std::cout << "5. Выстрел из лука (Проверка: Меткость, Урон: +50)" << std::endl;
        if (hasItem(player.inventory, "ружье")) std::cout << "6. Выстрел из ружья (Осталось: " << player.rifleUsesPerBattle << ", Проверка: Меткость, Урон: +40)" << std::endl;
        if (hasItem(player.inventory, "кнут")) std::cout << "7. Удар кнутом (Проверка: Меткость, Урон: +40)" << std::endl;
        if (hasItem(player.inventory, "нож")) std::cout << "8. Удар ножом (Проверка: Боевой опыт, Урон: +15)" << std::endl;
        if (hasItem(player.inventory, "топор")) std::cout << "9. Удар топором (Проверка: Ловкость, Урон: +35)" << std::endl;
        if (player.magicAccess) {
            if (knowsSpell(player.knownSpells, "Световой луч")) std::cout << "10. Магический луч (Мана: -1, Проверка: Контроль магии, Урон: +80)" << std::endl;
            if (knowsSpell(player.knownSpells, "Ослепление")) std::cout << "11. Ослепление (Мана: -1, Проверка: Магический опыт)" << std::endl;
            if (knowsSpell(player.knownSpells, "Приручение")) std::cout << "12. Приручение (Мана: -1, Проверка: Психическая стабильность)" << std::endl;
        }
        std::cout << "Ваш выбор: ";
        int choice = getIntInput();

        std::string actionName;
        int statValue = 0;
        int damageDealt = 0;
        bool instantWinAction = false;
        bool requiresMagic = false;
        bool validAction = true;

        switch (choice) {
        case 1: actionName = "удар рукой"; statValue = player.combatExperience; damageDealt = 0; break;
        case 2: actionName = "бегство"; statValue = player.stamina; instantWinAction = true; break;
        case 3: actionName = "маскировка"; statValue = player.intelligence; instantWinAction = true; break;
        case 4:
            if (hasItem(player.inventory, "меч")) { actionName = "удар мечом"; statValue = player.agility; damageDealt = 40; }
            else { std::cout << "У вас нет меча!" << std::endl; validAction = false; } break;
        case 5:
            if (hasItem(player.inventory, "лук и стрелы")) { actionName = "выстрел из лука"; statValue = player.accuracy; damageDealt = 50; }
            else { std::cout << "У вас нет лука и стрел!" << std::endl; validAction = false; } break;
        case 6:
            if (hasItem(player.inventory, "ружье")) {
                if (player.rifleUsesPerBattle > 0) { actionName = "выстрел из ружья"; statValue = player.accuracy; damageDealt = 40; }
                else { std::cout << "Выстрелы из ружья закончились в этом бою!" << std::endl; validAction = false; }
            }
            else { std::cout << "У вас нет ружья!" << std::endl; validAction = false; } break;
        case 7:
            if (hasItem(player.inventory, "кнут")) { actionName = "удар кнутом"; statValue = player.accuracy; damageDealt = 40; }
            else { std::cout << "У вас нет кнута!" << std::endl; validAction = false; } break;
        case 8:
            if (hasItem(player.inventory, "нож")) { actionName = "удар ножом"; statValue = player.combatExperience; damageDealt = 15; }
            else { std::cout << "У вас нет ножа!" << std::endl; validAction = false; } break;
        case 9:
            if (hasItem(player.inventory, "топор")) { actionName = "удар топором"; statValue = player.agility; damageDealt = 35; }
            else { std::cout << "У вас нет топора!" << std::endl; validAction = false; } break;
        case 10:
            if (player.magicAccess && knowsSpell(player.knownSpells, "Световой луч")) {
                if (player.magicReserve > 0) { actionName = "магический луч"; statValue = player.magicControl; damageDealt = 80; requiresMagic = true; }
                else { std::cout << "Недостаточно маны!" << std::endl; validAction = false; }
            }
            else { std::cout << "Вы не знаете это заклинание или нет доступа к магии!" << std::endl; validAction = false; } break;
        case 11:
            if (player.magicAccess && knowsSpell(player.knownSpells, "Ослепление")) {
                if (player.magicReserve > 0) { actionName = "ослепление"; statValue = player.magicExperience; instantWinAction = true; requiresMagic = true; }
                else { std::cout << "Недостаточно маны!" << std::endl; validAction = false; }
            }
            else { std::cout << "Вы не знаете это заклинание или нет доступа к магии!" << std::endl; validAction = false; } break;
        case 12:
            if (player.magicAccess && knowsSpell(player.knownSpells, "Приручение")) {
                if (player.magicReserve > 0) { actionName = "приручение"; statValue = player.psychicStability; instantWinAction = true; requiresMagic = true; }
                else { std::cout << "Недостаточно маны!" << std::endl; validAction = false; }
            }
            else { std::cout << "Вы не знаете это заклинание или нет доступа к магии!" << std::endl; validAction = false; } break;
        default: std::cout << "Неверное действие." << std::endl; validAction = false; break;
        }

        if (!validAction) {
            continue;
        }

        double coefficient = currentMonster.getActionCoefficient(actionName);
        double resultValue = coefficient * static_cast<double>(statValue);

        bool monsterTurnSkipped = false;
        bool monsterTakesDamage = false;

        std::cout << "Вы выбрали: " << actionName << ". Коэффициент: " << coefficient << ", Стат: " << statValue << ", Результат: " << std::fixed << std::setprecision(1) << resultValue << std::endl;

        const double epsilon = 1e-5; 
        if (resultValue > 1.5 + epsilon) { 
            std::cout << "Успешное действие!" << std::endl;
            monsterTakesDamage = true;
            monsterTurnSkipped = true;
            if (instantWinAction) {
                std::cout << currentMonster.name << " побежден с помощью '" << actionName << "'!" << std::endl;
                currentMonster.hp = 0;
            }
        }
        else if (resultValue < 1.5 - epsilon) { 
            std::cout << "Действие провалено!" << std::endl;
            monsterTakesDamage = false; 
        }
        else { 
            std::cout << "Действие нейтрально." << std::endl;
            monsterTakesDamage = true;
            
        }
        if (monsterTakesDamage && currentMonster.hp > 0 && !instantWinAction) {
            currentMonster.hp -= damageDealt;
            std::cout << currentMonster.name << " получает " << damageDealt << " урона." << std::endl;
            if (currentMonster.hp <= 0) {
                std::cout << currentMonster.name << " побежден!" << std::endl;
            }
        }

        if (requiresMagic) {
            player.magicReserve--;
        }
        if (actionName == "выстрел из ружья" && choice == 6) {
            player.rifleUsesPerBattle--;
        }

        if (currentMonster.hp > 0 && !monsterTurnSkipped) {
            std::cout << currentMonster.name << " атакует!" << std::endl;
            player.hp -= currentMonster.damageToPlayer;
            std::cout << "Вы получаете " << currentMonster.damageToPlayer << " урона." << std::endl;
            if (player.hp <= 0) {
                std::cout << "Вы были побеждены..." << std::endl;
                return false; 
            }
        }
    } 

    if (currentMonster.hp <= 0) {
        return true;
    }
    return false;
}


bool battlePhase(Player& player, std::vector<Monster>& monsters, const std::map<std::string, std::string>& bookData) {
    std::cout << "\n--- НАЧИНАЕТСЯ ФАЗА СРАЖЕНИЙ ---" << std::endl;
    int monstersDefeated = 0;

    for (Monster& monster : monsters) {
        bool battleResult = runSingleBattle(player, monster, bookData);
        if (battleResult) {
            std::cout << monster.name << " повержен! Готовьтесь к следующему..." << std::endl;
            monstersDefeated++;
        }
        else {
            std::cout << "Вы проиграли бой с " << monster.name << "." << std::endl;
            player.attempts--;
            if (player.attempts > 0) {
                std::cout << "У вас осталось " << player.attempts << " попыток. Бои начнутся сначала." << std::endl;
                player.resetForNewAttempt(); 
                return false;
            }
            else {
                std::cout << "Попытки закончились. Игра окончена." << std::endl;
                return false; 
            }
        }
    }

    return monstersDefeated == monsters.size(); 
}


int main() {
#if defined(_WIN32) || defined(_WIN64)
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
     std::locale::global(std::locale(""));

    std::cout << "Добро пожаловать в деревню эльфов!" << std::endl;
    std::cout <<"вам нужно доказать, что вы друг и свою полезность," << std::endl;
    std::cout << "для этого нужно подготовиться к бою и победить монстров" << std::endl;

    bool playAgain = true;
    while (playAgain) {
        Player player; 

        preparationPhase(player);

        if (player.steps > 0 && player.attempts > 0) { 
            std::cout << "\nПодготовка завершена. Ваши финальные характеристики перед боем:" << std::endl;
            player.displayStats();
            std::cout << "\nНажмите Enter для начала сражений...";
            std::cin.get();
        }
        else if (player.attempts <= 0) {
            std::cout << "\nИгра окончена, так как закончились попытки еще до начала боев." << std::endl;
            playAgain = false; 
            continue;
        }


        std::vector<Monster> monsters;
        monsters.emplace_back("Волк", 150, 1, "книга о волках",
            std::vector<std::string>{"ослепление", "выстрел из ружья", "магический луч"},
            std::vector<std::string>{"удар рукой", "маскировка", "бегство"});

        monsters.emplace_back("Кентавр", 450, 1, "книга о кентавре",
            std::vector<std::string>{"бегство", "маскировка", "приручение"},
            std::vector<std::string>{/* "Все остальные - провальные" обрабатывается в getActionCoefficient */},
            std::vector<std::string>{"ослепление"});

        monsters.emplace_back("Грифон", 350, 1, "книга о грифоне",
            std::vector<std::string>{"удар мечом", "выстрел из лука", "выстрел из ружья", "ослепление"},
            std::vector<std::string>{"удар рукой", "бегство"});

        bool allBattlesWon = false;
        while (player.attempts > 0 && !allBattlesWon) {
            allBattlesWon = battlePhase(player, monsters, bookContents);
            if (!allBattlesWon && player.attempts > 0) {
                std::cout << "\nГотовимся к новой попытке..." << std::endl;
                std::cout << "Нажмите Enter, чтобы продолжить...";
                std::cin.get();
            }
        }


        if (allBattlesWon) {
            std::cout << "\n\n***********" << std::endl;
            std::cout << "ПОЗДРАВЛЯЕМ! Вы победили всех монстров!" << std::endl;
            std::cout << "***********" << std::endl;
        }
        else {
            std::cout << "\n\n=======================================" << std::endl;
            std::cout << "УВЫ! Вы проиграли. Попытки закончились." << std::endl;
            std::cout << "=======================================" << std::endl;
        }

        std::cout << "\nХотите сыграть еще раз? (y/n): ";
        char repeatChoice;
        std::cin >> repeatChoice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
        if (tolower(repeatChoice) != 'y') {
            playAgain = false;
        }
    }

    std::cout << "Спасибо за игру! Нажмите Enter для выхода." << std::endl;
    std::cin.get();
    return 0;
}
