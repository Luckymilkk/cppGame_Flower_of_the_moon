#pragma once
#include "../domain/ItemManager.h"
#include "../domain/MonsterManager.h"
#include "../domain/SpellManager.h"
#include "../domain/BookManager.h"
#include "../domain/ActionManager.h"
#include "../domain/Player.h"
#include <string>

namespace data {

class DataLoader {
public:
    static bool loadItems(const std::string& filename, domain::ItemManager& manager);
    static bool loadMonsters(const std::string& filename, domain::MonsterManager& manager);
    static bool loadSpells(const std::string& filename, domain::SpellManager& manager);
    static bool loadBooks(const std::string& filename, domain::BookManager& manager);
    static bool loadActions(const std::string& filename, domain::ActionManager& manager);
    static bool loadPlayer(const std::string& filename, domain::Player& player);
};

} // namespace data 