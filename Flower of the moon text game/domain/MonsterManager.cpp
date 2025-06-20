#include "MonsterManager.h"

namespace domain {

void MonsterManager::addMonster(const Monster& monster) {
    monsters[monster.name] = monster;
}

const Monster* MonsterManager::getMonster(const std::string& name) const {
    auto it = monsters.find(name);
    if (it != monsters.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Monster> MonsterManager::getAllMonsters() const {
    std::vector<Monster> result;
    for (const auto& pair : monsters) {
        result.push_back(pair.second);
    }
    return result;
}

} // namespace domain 