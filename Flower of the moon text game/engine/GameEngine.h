#pragma once
#include "../domain/Player.h"
#include "../domain/ItemManager.h"
#include "../domain/MonsterManager.h"
#include "../domain/SpellManager.h"
#include "../domain/BookManager.h"
#include "../domain/ActionManager.h"
#include <memory>

namespace engine {

class GameEngine {
public:
    GameEngine();
    void run();
private:
    std::unique_ptr<domain::Player> player;
    domain::ItemManager itemManager;
    domain::MonsterManager monsterManager;
    domain::SpellManager spellManager;
    domain::BookManager bookManager;
    domain::ActionManager actionManager;
    void preparationPhase();
    void battlePhase();
    void showFinalStats();
};

} // namespace engine 