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
    player_ = std::make_unique<domain::Player>();
    
    try {
        if (!data::DataLoader::loadPlayer(PLAYER_FILE, *player_)) {
            throw std::runtime_error("Ошибка загрузки данных игрока");
        }
        if (!data::DataLoader::loadItems(ITEMS_FILE, item_manager_)) {
            throw std::runtime_error("Ошибка загрузки предметов");
        }
        if (!data::DataLoader::loadMonsters(MONSTERS_FILE, monster_manager_)) {
            throw std::runtime_error("Ошибка загрузки монстров");
        }
        if (!data::DataLoader::loadSpells(SPELLS_FILE, spell_manager_)) {
            throw std::runtime_error("Ошибка загрузки заклинаний");
        }
        if (!data::DataLoader::loadBooks(BOOKS_FILE, book_manager_)) {
            throw std::runtime_error("Ошибка загрузки книг");
        }
        if (!data::DataLoader::loadActions(PREPARATION_ACTIONS_FILE, action_manager_)) {
            throw std::runtime_error("Ошибка загрузки действий подготовки");
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Ошибка инициализации игры: " << e.what() << std::endl;
        throw;
    }
}

void GameEngine::Run() {
    std::cout << "Добро пожаловать в игру!" << std::endl;
    int attempts = 3;
    domain::Player initial_player_state = *player_; // Сохраняем начальное состояние

    while (attempts > 0) {
        *player_ = initial_player_state; // Сброс к начальному состоянию
        player_->health = player_->max_health; // Восстанавливаем здоровье до максимума
        player_->mana = player_->magic_reserve;   // Восстанавливаем ману до максимума
        player_->attempts = attempts;
        std::cout << "\n--- ПОПЫТКА " << (4 - attempts) << "/3 ---" << std::endl;
        
        PreparationPhase();
        
        if (BattlePhase()) {
            ShowFinalStats();
            return; // Успешный выход из игры
        } else {
            attempts--;
            if (attempts > 0) {
                std::cout << "\nВы проиграли бой. Попробуйте снова. Осталось попыток: " << attempts << std::endl;
            } else {
                std::cout << "\nВы использовали все попытки. Игра окончена." << std::endl;
            }
        }
    }
    ShowFinalStats();
}

void GameEngine::PreparationPhase() {
    std::cout << "\n--- ФАЗА ПОДГОТОВКИ ---" << std::endl;
    while (player_->steps > 0) {
        player_->DisplayStats(item_manager_);
        int current_used_slots = player_->GetUsedInventorySlots(item_manager_);
        std::cout << "Занято слотов: " << current_used_slots << "/" << player_->inventory_slots << std::endl;
        std::cout << "Осталось шагов: " << player_->steps << std::endl;
        std::cout << "Выберите действие:" << std::endl;
        int menuOption = 1;
        std::vector<std::function<void(bool&)>> actions;

        // 1-3. Взять книги
        struct BookOption { std::string id; std::string name; };
        std::vector<BookOption> books = {
            {"book_wolf", "книга о волках"},
            {"book_centaur", "книга о кентавре"},
            {"book_griffin", "книга о грифоне"}
        };
        for (const auto& book : books) {
            if (!player_->HasItem(book.id) && current_used_slots + 1 <= player_->inventory_slots) {
                std::cout << menuOption << ". Взять '" << book.name << "' (Шаги: 1, Слоты: 1)" << std::endl;
                actions.push_back([this, book](bool& done) {
                    player_->AddItem(book.id);
                    player_->steps -= 1;
                    std::cout << "'" << book.name << "' добавлена в инвентарь." << std::endl;
                    done = true;
                });
                ++menuOption;
            }
        }

        // Прочитать книги
        for (const auto& book_opt : books) {
            if (player_->HasItem(book_opt.id) && !player_->books_actually_read[book_opt.id]) {
                std::cout << menuOption << ". Прочитать '" << book_opt.name << "' (Шаги: 0)" << std::endl;
                actions.push_back([this, book_opt](bool& done) {
                    player_->books_actually_read[book_opt.id] = true;
                    // player_->steps -= 0; // Шаги не тратятся
                    const auto& book_content = book_manager_.GetBook(book_opt.id);
                    if (book_content) {
                        std::cout << "\n=== " << book_content->title << " ===" << std::endl;
                        std::cout << book_content->content << std::endl;
                        std::cout << "====================\n" << std::endl;
                    }
                    else {
                        std::cout << "Ошибка: не удалось найти содержимое книги." << std::endl;
                    }
                    done = true;
                });
                ++menuOption;
            }
        }

        // 4. Взять сумку
        if (!player_->HasItem("bag")) {
            std::cout << menuOption << ". Взять 'сумка' (Шаги: 1, Эффект: +2 слота инвентаря)" << std::endl;
            actions.push_back([this](bool& done) {
                player_->AddItem("bag");
                player_->inventory_slots += 2;
                player_->steps -= 1;
                std::cout << "'Сумка' добавлена. Слоты инвентаря увеличены до " << player_->inventory_slots << "." << std::endl;
                done = true;
            });
            ++menuOption;
        }
        // 5-12. Взять оружие
        struct WeaponOption { std::string id; std::string name; int cost_steps; int cost_slots; };
        std::vector<WeaponOption> weapons = {
            {"sword", "меч", 1, 1},
            {"axe", "топор", 1, 1},
            {"whip", "кнут", 1, 1},
            {"rifle", "ружье", 1, 1},
            {"bow", "лук и стрелы", 2, 2},
            {"knife", "нож", 1, 1}
        };
        for (const auto& weapon : weapons) {
            if (!player_->HasItem(weapon.id) && current_used_slots + weapon.cost_slots <= player_->inventory_slots) {
                std::cout << menuOption << ". Взять '" << weapon.name << "' (Шаги: " << weapon.cost_steps << ", Слоты: " << weapon.cost_slots << ")" << std::endl;
                actions.push_back([this, weapon](bool& done) {
                    player_->AddItem(weapon.id);
                    player_->steps -= weapon.cost_steps;
                    std::cout << "'" << weapon.name << "' добавлен в инвентарь." << std::endl;
                    done = true;
                });
                ++menuOption;
            }
        }
        // 13-14. Прокачка характеристик
        struct StatOption { std::string stat; std::string name; };
        std::vector<StatOption> stats = {
            {"agility", "ловкость"},
            {"accuracy", "меткость"},
            {"stamina", "выносливость"},
            {"intelligence", "ум"},
            {"combat_experience", "боевой опыт"}
        };
        for (const auto& stat : stats) {
            std::cout << menuOption << ". Прокачать " << stat.name << " (Шаги: 1)" << std::endl;
            actions.push_back([this, stat](bool& done) {
                player_->ModifyStat(stat.stat, 1, 3);
                player_->steps -= 1;
                done = true;
            });
            ++menuOption;
        }
        // 14. Восстановить доступ к магии
        if (!player_->magic_access) {
            std::cout << menuOption << ". Восстановить доступ к магии (Шаги: 1)" << std::endl;
            actions.push_back([this](bool& done) {
                player_->magic_access = true;
                player_->steps -= 1;
                std::cout << "Доступ к магии восстановлен!" << std::endl;
                done = true;
            });
            ++menuOption;
        }
        // 15-20. Действия, доступные только после восстановления магии
        if (player_->magic_access) {
            // 15-17. Изучить заклинания
            struct SpellOption { std::string name; std::string display; };
            std::vector<SpellOption> spells = {
                {"световой луч", "Освоить магию световой луч"},
                {"ослепление", "Освоить магию ослепление"},
                {"приручение", "Освоить магию приручение"}
            };
            for (const auto& spell : spells) {
                if (!player_->KnowsSpell(spell.name)) {
                    std::cout << menuOption << ". " << spell.display << " (Шаги: 1)" << std::endl;
                    actions.push_back([this, spell](bool& done) {
                        player_->known_spells.push_back(spell.name);
                        player_->steps -= 1;
                        std::cout << "Магия '" << spell.name << "' освоена." << std::endl;
                        done = true;
                    });
                    ++menuOption;
                }
            }
            // 18-20. Прокачка магических характеристик
            struct MagicStatOption { std::string stat; std::string name; };
            std::vector<MagicStatOption> magic_stats = {
                {"magic_control", "контроль магии"},
                {"magic_experience", "магический опыт"},
                {"psychic_stability", "психическая стабильность"}
            };
            for (const auto& stat : magic_stats) {
                std::cout << menuOption << ". Прокачать " << stat.name << " (Шаги: 1)" << std::endl;
                actions.push_back([this, stat](bool& done) {
                    player_->ModifyStat(stat.stat, 1, 3);
                    player_->steps -= 1;
                    done = true;
                });
                ++menuOption;
            }
        }
        std::cout << menuOption << ". Закончить подготовку досрочно" << std::endl;
        actions.push_back([](bool& done) { done = false; });
        int choice = GetIntInput("Ваш выбор (введите номер): ", 1, menuOption);
        if (choice == menuOption) {
            std::cout << "Подготовка завершена досрочно." << std::endl;
            break;
        }
        bool actionDone = false;
        actions[choice - 1](actionDone);
        if (choice == menuOption) {
            std::cout << "Подготовка завершена досрочно." << std::endl;
            break;
        }
        if (actionDone) continue;
    }
    std::cout << "--- ПОДГОТОВКА ЗАВЕРШЕНА ---" << std::endl;
}

// Новая структура для описания боевого действия
struct BattleAction {
    std::string name;
    std::string check_stat;
    int damage;
    int mana_cost;
    bool is_instant_win;
    std::function<bool(domain::Player&)> requirement;
    
    BattleAction(std::string n, std::string cs, int d, int mc, bool iw, std::function<bool(domain::Player&)> req)
        : name(n), check_stat(cs), damage(d), mana_cost(mc), is_instant_win(iw), requirement(req) {}
};

bool GameEngine::BattlePhase() {
    std::vector<BattleAction> all_actions = {
        {"Удар рукой", "combat_experience", 0, 0, false, [](domain::Player& p){ return true; }},
        {"Бегство", "stamina", 0, 0, true, [](domain::Player& p){ return true; }},
        {"Маскировка", "intelligence", 0, 0, true, [](domain::Player& p){ return true; }},
        {"Удар мечом", "agility", 40, 0, false, [](domain::Player& p){ return p.HasItem("sword"); }},
        {"Выстрел из лука", "accuracy", 50, 0, false, [](domain::Player& p){ return p.HasItem("bow"); }},
        {"Выстрел из ружья", "accuracy", 40, 0, false, [](domain::Player& p){ return p.HasItem("rifle") && p.rifle_uses_per_battle < 2; }},
        {"Удар кнутом", "accuracy", 40, 0, false, [](domain::Player& p){ return p.HasItem("whip"); }},
        {"Удар ножом", "combat_experience", 15, 0, false, [](domain::Player& p){ return p.HasItem("knife"); }},
        {"Удар топором", "agility", 35, 0, false, [](domain::Player& p){ return p.HasItem("axe"); }},
        {"Магический луч", "magic_control", 80, 1, false, [](domain::Player& p){ return p.KnowsSpell("световой луч") && p.mana > 0; }},
        {"Ослепление", "magic_experience", 0, 1, true, [](domain::Player& p){ return p.KnowsSpell("ослепление") && p.mana > 0; }},
        {"Приручение", "psychic_stability", 0, 1, true, [](domain::Player& p){ return p.KnowsSpell("приручение") && p.mana > 0; }}
    };

    std::cout << "\n--- ФАЗА БОЯ ---" << std::endl;
    std::cout << "Чтобы выиграть, вам нужно победить 3 монстра!" << std::endl;
    int monstersDefeated = 0;
    
    auto all_monsters = monster_manager_.GetAllMonsters(); // Получаем монстров из менеджера

    for (const auto& monster_prototype : all_monsters) {
        if (monstersDefeated >= 3) break;

        // Создаем боевую копию монстра на основе данных из менеджера
        MonsterData current_monster;
        current_monster.name = monster_prototype.name;
        current_monster.initial_hp = monster_prototype.health;
        current_monster.current_hp = monster_prototype.health;
        current_monster.book_key = monster_prototype.bookKey;
        current_monster.favorable_actions = monster_prototype.favorableActions;
        current_monster.unfavorable_actions = monster_prototype.unfavorableActions;
        current_monster.neutral_actions = monster_prototype.neutralActions;
        
        player_->rifle_uses_per_battle = 0; 

        std::cout << "\n========================================" << std::endl;
        std::cout << "Вас атакует монстр: " << current_monster.name << "!" << std::endl;
        std::cout << "========================================" << std::endl;

        if (player_->books_actually_read[current_monster.book_key]) {
            std::cout << "\nИнформация из книги:" << std::endl;
            const auto& book = book_manager_.GetBook(current_monster.book_key);
            if (book) std::cout << book->content << std::endl;
        }

        while (current_monster.current_hp > 0 && player_->health > 0) {
            std::cout << "\n--- ВАШ ХОД ---" << std::endl;
            player_->DisplayStats(item_manager_);
            current_monster.DisplayInfo(*player_, book_manager_);
            std::cout << "Здоровье монстра: " << current_monster.current_hp << "/" << current_monster.initial_hp << std::endl;
            
            std::vector<BattleAction> available_actions;
            std::cout << "\nВыберите действие:" << std::endl;
            int menu_idx = 1;
            for (const auto& action : all_actions) {
                if (action.requirement(*player_)) {
                    std::cout << menu_idx << ". " << action.name << " (Проверка: " << action.check_stat << ")" << std::endl;
                    available_actions.push_back(action);
                    menu_idx++;
                }
            }
            
            int choice = GetIntInput("Ваш выбор: ", 1, static_cast<int>(available_actions.size()));
            const auto& chosen_action = available_actions[choice - 1];
            
            double coef = current_monster.GetActionCoefficient(chosen_action.name);
            double result = coef * player_->GetStatValue(chosen_action.check_stat);
            
            bool success = false;
            if (result > 1.5) {
                std::cout << "Успех! ";
                success = true;
            } else if (std::abs(result - 1.5) < 1e-9) { // Сравнение double чисел
                std::cout << "Нейтрально. ";
                success = true; 
            } else { // result < 1.5
                std::cout << "Провал... ";
                success = false;
            }

            if (success) {
                if (chosen_action.is_instant_win) {
                    std::cout << "Вы мгновенно побеждаете монстра!" << std::endl;
                    current_monster.current_hp = 0;
                } else {
                    current_monster.current_hp -= chosen_action.damage;
                    std::cout << "Вы нанесли " << chosen_action.damage << " урона." << std::endl;
                }
                player_->mana -= chosen_action.mana_cost;
                if (chosen_action.name == "Выстрел из ружья") {
                    player_->rifle_uses_per_battle++;
                }
            }

            // Монстр атакует при провале или нейтральном результате, если он еще жив
            if ((!success || std::abs(result - 1.5) < 1e-9) && current_monster.current_hp > 0) {
                std::cout << "Монстр атакует в ответ!" << std::endl;
                player_->TakeDamage(1);
            }
        }

        if (player_->health > 0) {
            std::cout << "\n*** Вы победили " << current_monster.name << "! ***" << std::endl;
            monstersDefeated++;
        } else {
            std::cout << "\nВы были побеждены..." << std::endl;
            // Логика попыток будет здесь
            break;
        }
    }

    if (monstersDefeated >= 3) {
        std::cout << "\n\n*** ПОЗДРАВЛЯЕМ! ВЫ ПРОШЛИ ИГРУ! ***" << std::endl;
        return true; // Победа
    } else if (player_->health > 0) {
        std::cout << "\n\nВы не смогли победить всех монстров. Попробуйте снова!" << std::endl;
        return false; // Поражение
    } else {
        std::cout << "\n\n--- ИГРА ОКОНЧЕНА ---" << std::endl;
        return false; // Поражение
    }
}

void GameEngine::ShowFinalStats() {
    std::cout << "\n--- Финальные характеристики игрока ---" << std::endl;
    player_->DisplayStats(item_manager_);
    std::cout << "Спасибо за игру!" << std::endl;
}

} // namespace engine 