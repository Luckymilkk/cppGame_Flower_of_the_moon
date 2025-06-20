#ifndef ENGINE_GAME_ENGINE_H_
#define ENGINE_GAME_ENGINE_H_

#include "../domain/Player.h"
#include "../domain/ItemManager.h"
#include "../domain/MonsterManager.h"
#include "../domain/SpellManager.h"
#include "../domain/BookManager.h"
#include "../domain/ActionManager.h"
#include "../data_structures.h"
#include <memory>

namespace engine {

class GameEngine {
public:
    GameEngine();
    void Run();
private:
    std::unique_ptr<domain::Player> player_;
    domain::ItemManager item_manager_;
    domain::MonsterManager monster_manager_;
    domain::SpellManager spell_manager_;
    domain::BookManager book_manager_;
    domain::ActionManager action_manager_;
    void PreparationPhase();
    bool BattlePhase();
    void ShowFinalStats();
};

} // namespace engine 

#endif  // ENGINE_GAME_ENGINE_H_ 